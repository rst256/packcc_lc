package.path = [[?.lua;?\init.lua;]]..package.path
te = require'ast_lua.te'

-- print('AUXIL', AUXIL)
AUXIL:scope_add("int", { is_type=true })
AUXIL:scope_add("short", { is_type=true })
AUXIL:scope_add("long", { is_type=true })
AUXIL:scope_add("char", { is_type=true })

function printf(fmt, ...)
	io.write(fmt:format(...))
end

function new_asn(mt, this) 
	local this = this or {}
	for k, v in pairs(vars) do print(k,v) this[k] = v end
	return setmetatable(this, mt)
end

te_primary_mt = { __concat=function(a, b) return tostring(a)..tostring(b) end}
te_pointer_mt = { __concat=function(a, b) return tostring(a)..tostring(b) end}
te_array_mt = { __concat=function(a, b) return tostring(a)..tostring(b) end}
te_mt = { __concat=function(a, b) return tostring(a)..tostring(b) end}
binop_mt = { __index={} }
unop_mt = { __index={} }
call_mt = { __index={} }
assign_mt = { __index={} }
define_mt = { __concat=function(a, b) return tostring(a)..tostring(b) end}
block_mt = { __concat=function(a, b) return tostring(a)..tostring(b) end}
if_then_mt = { __concat=function(a, b) return tostring(a)..tostring(b) end}

type_expr_ops = {
	
}
function te_primary(base_type, qualifier) 
	return setmetatable({ base_type=base_type, qualifier=qualifier }, 
		te_primary_mt)
end

function te_pointer(base_type, qualifier) 
	return setmetatable({ base_type=base_type, qualifier=qualifier }, 
		te_pointer_mt)
end

function te_array(base_type, dim, qualifier) 
	return setmetatable({ base_type=base_type, dim=dim, qualifier=qualifier }, 
		te_array_mt)
end

function assign(var, value) 
	return setmetatable({ var=var, value=value }, assign_mt)
end

function define(te, var, value) 
	return setmetatable({ te=te, var=var, value=value }, define_mt)
end

function binop(l, op, r) 
	local lv, rv = tonumber(l), tonumber(r)
	if lv and rv then
		return load('return '..l..' '..op..' '..r..' ;', '', 't')()
	end
	return setmetatable({ larg=l, op=op, rarg=r }, binop_mt)
end

function unop(op, arg) 
	if tonumber(arg) then
		return load('return '..op..' '..arg..' ;', '', 't')()
	end
	return setmetatable({ arg=arg, op=op }, unop_mt)
end

function block(p, b, e) 
	return setmetatable({ auxil=p, b=b or '', e=e or '' }, block_mt)
end

function call(fn, ...) 
	return setmetatable({ fn=fn }, call_mt)
end

function if_then(cond, th, el) 
	return setmetatable({ cond=cond, th=th, el=el }, if_then_mt)
end

function te_primary_mt:__tostring() 
	local s = ''
	if self.qualifier then s = s..tostring(self.qualifier)..' ' end
	s = s..tostring(self.base_type)
	return s
end

function te_pointer_mt:__tostring() 
	local s = tostring(self.base_type)
	if self.qualifier then s = s..' '..tostring(self.qualifier) end
	s = s..'* '
	return s
end

function te_array_mt:__tostring() 
	local s = tostring(self.base_type)
	if self.qualifier then s = s..tostring(self.qualifier)..' ' end
	s = s..'['..self.dim..']'
	return s
end

function binop_mt:__tostring() 
	return '('..tostring(self.larg)..' '..self.op..' '..tostring(self.rarg)..')'
end

function is_pure(e) 
	if type(e)=='table' then return e:is_pure() end
	return true--type(e)~='userdata' --and fal tonumber(e)~=nil
end

function binop_mt.__index:is_pure() 
	return is_pure(self.larg) and is_pure(self.rarg) 
end

function unop_mt:__tostring() 
	return tostring(self.op)..tostring(self.arg)
end

function unop_mt.__index:is_pure() 
	return self.op~='++' and self.op~='--' and is_pure(self.arg) 
end

function call_mt.__index:is_pure() 
	if self.args then
		for k=1, #self.args do
			if not is_pure(self.args[k]) then return false end
		end
	end
	return is_pure(self.fn) 
end


function assign_mt:__tostring() 
	return tostring(self.var)..' = '..tostring(self.value)
end

function define_mt:__tostring() 
	return tostring(self.te)..': '..tostring(self.var)..
		(self.value~=nil and ' = '..tostring(self.value)..' @'..tostring(is_pure(self.value)) or '')
end

function call_mt:__tostring() 
	local s = tostring(self.fn)..'('
	if self.args then
		for k=1, #self.args do
			s = s..tostring(self.args[k])..(k==#self.args and '' or ', ')
		end
	end
	return s..')'
end

function block_mt:__tostring() 
	local s = self.b..'\n'
	for k=1, #self do
		s = s..('\t'):rep(self.auxil:get_level())..tostring(self[k])..'\n'
	end
	return s..('\t'):rep(self.auxil:get_level()-1)..self.e
end

function if_then_mt:__tostring() 
	local s = 'if '..tostring(self.cond)..' then '..tostring(self.th)
	if self.el then s = s..' else '..tostring(self.el) end
	return s..'end'
end