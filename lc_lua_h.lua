package.path = [[?.lua;?\init.lua;]]..package.path
te = require'ast_lua.te'


local scalar_type_mt = { __index={ is_type=true }, __newindex=print }

function scalar_type(sizeof)
	return setmetatable({ sizeof=sizeof }, scalar_type_mt)
end


type_void = AUXIL:scope_add("void", scalar_type(0))
type_int = AUXIL:scope_add("int", scalar_type(4))
type_short = AUXIL:scope_add("short", scalar_type(2))
type_long = AUXIL:scope_add("long", scalar_type(8))
type_char = AUXIL:scope_add("char", scalar_type(1))
type_string = te.pointer( te.primary(type_char, 'const') )




function printf(fmt, ...)
	io.write(fmt:format(...))
end

binop_mt = { __index={} }
unop_mt = { __index={} }
call_mt = { __index={} }
assign_mt = { __index={} }
define_mt = { __index={} }
block_mt = { __metatable="block" }
if_then_mt = { __concat=function(a, b) return tostring(a)..tostring(b) end}



function assign(var, value)
	return setmetatable({ var=var, value=value }, assign_mt)
end

function define(t, var, value)
	local this = setmetatable({ te=t, var=var, value=value },
		define_mt)
	local var_sym = AUXIL:scope_add(var, {
		define_in_node=this,
		type_expr=t,
		is_initialized=value~=nil
	})
	if not var_sym then
		print("redefine: ", var, AUXIL:scope_find(var))
	end

	return this
end

function binop(l, op, r)
	local lv, rv = tonumber(l), tonumber(r)
	if lv and rv then
		return load('return '..l..' '..op..' '..r..' ;', '', 't')()
	end
	return setmetatable({ larg=l, op=op, rarg=r }, binop_mt)
end

function typeof(expr)
	if tonumber(expr) then
		print(math.tointeger(expr))
		return type_int
	end
	if type(expr)=='string' then
		return type_string
	end
	local sym = expr.type_expr or
		assert(AUXIL:scope_find(expr),
			tostring(expr)..'\n'..AUXIL:pos(_0e)
		).type_expr
	return sym
end

function unop(op, uarg)
	-- if tonumber(arg) then
	-- 	return load('return '..op..' '..arg..' ;', '', 't')()
	-- endand type(uarg)~='table'
	local sym = typeof(uarg)
	if op=='@'  then

		if not sym.is_pointer then
			error('\n'..AUXIL:pos(_0e)..
				'@ on non-pointer '..tostring(uarg))
		end

		if sym.base_type.sizeof==0 then
			error('\n'..AUXIL:pos(_0e)..
				'@ on void pointer '..tostring(uarg))
		end

		return setmetatable({ arg=uarg, op=op,
			type_expr=sym.base_type
		}, unop_mt)
	end
	return setmetatable({
		arg=uarg, op=op, type_expr=sym
	}, unop_mt)
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

--function te_primary_mt:__tostring()
--	local s = ''
--	if self.qualifier then s = s..tostring(self.qualifier)..' ' end
--	s = s..tostring(self.base_type)
--	return s
--end

--function te_pointer_mt:__tostring()
--	local s = tostring(self.base_type)
--	if self.qualifier then s = s..' '..tostring(self.qualifier) end
--	s = s..'* '
--	return s
--end

--function te_array_mt:__tostring()
--	local s = tostring(self.base_type)
--	if self.qualifier then s = s..tostring(self.qualifier)..' ' end
--	s = s..'['..self.dim..']'
--	return s
--end

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

--function unop_mt.__index:typeof()
--	if self.op=='@' then
--		return self.arg:typeof()
--	end
--	return self.arg:typeof()
--end

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
		(self.value~=nil and ' = '..tostring(self.value) or '')
end

function define_mt.__index:pos()
	return AUXIL:pos(self.s)
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