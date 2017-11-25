package.path = [[?.lua;?\init.lua;]]..package.path
te = require'ast_lua.te'


local scalar_type_mt = { __index={ is_type=true, is_scalar=true }, __newindex=print }

function scalar_type(sizeof)
	return setmetatable({ sizeof=sizeof }, scalar_type_mt)
end


type_void = AUXIL:scope_add("void", scalar_type(0))
type_int = AUXIL:scope_add("int", scalar_type(4))
type_short = AUXIL:scope_add("short", scalar_type(2))
type_long = AUXIL:scope_add("long", scalar_type(8))
type_double = AUXIL:scope_add("double", scalar_type(10))
type_char = AUXIL:scope_add("char", scalar_type(1))
type_string = te.pointer( te.primary(type_char, 'const') )


function asn_tmpl(tmpl, mt_name, this)
	assert(tmpl)
	return setmetatable(this or {}, {
		__metatable=mt_name,
		__tostring=function(self) 
			local t = setmetatable({}, {
				__index=function(s, k) return tostring(self[k] or '') end
			})
			return (string.gsub(tmpl, "%$([%w_]+)", t))
		end
	})
end

function asn_simple(text, mt_name)
	assert(text)
	return setmetatable({}, {
		__metatable=mt_name,
		__tostring=function() return text end
	})
end

break_cmd = asn_simple('break', 'break_cmd')
continue_cmd = asn_simple('continue', 'continue_cmd')

function asn_list(this, sep, mt_name, prefix)
	local sep = sep or ''
	local prefix = prefix or ''
	return setmetatable(this or {}, {
		__metatable=mt_name,
		__tostring=function(self)
			local s = prefix
			for k=1, #self do
				s = s..tostring(self[k])..	(k==#self and '' or sep)
			end
			return s
		end
	})
end

function printf(fmt, ...)
	io.write(fmt:format(...))
end

literal_mt = { __index={ is_literal=true } }
binop_mt = { __index={ type_expr=type_int } }
unop_mt = { __index={} }
call_mt = { __index={ type_expr=type_int } }
assign_mt = { __index={} }
define_mt = { __index={} }
define_args_mt = { __index={ type_expr=type_int } }
define_func_mt = { __index={ type_expr=type_int } }
block_mt = { __metatable="block" }
if_then_mt = { __concat=function(a, b) return tostring(a)..tostring(b) end}

function symof(expr)
	if tonumber(expr) or type(expr)=='string' then
		return nil
	end
	return sym
end

function typeof(expr)
	if tonumber(expr) then
		return math.tointeger(expr)~=nil and type_int or type_double
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


function literal(v, ltype)
	return setmetatable({
		type_expr=ltype or (tonumber(v)~=nil and
			(math.tointeger(v)~=nil and type_int or type_double)
				or (type(v)=='string' and type_string or nil)
		),
		value=v
	}, literal_mt)
end

function assign(var, value)
--	local sym = AUXIL:scope_find(var)
	var.is_uninitialized = nil
	return setmetatable({
		var=var,
		value=value,
		type_expr=typeof(value)
	}, assign_mt)
end

function define(t, var, value)
	local this = setmetatable({ te=t, var=var, value=value },
		define_mt)
	local var_sym = AUXIL:scope_add(var, {
		define_in_node=this,
		type_expr=t,
		is_uninitialized=value==nil,
		is_lvalue=true
	})
	if not var_sym then
		print("redefine: ", var, AUXIL:scope_find(var))
	end

	return this
end

function define_args(...)
	return setmetatable({...}, define_args_mt)
end

function define_func(this)--fn, args, ret_type, body, scope)
	local this = setmetatable(this, define_func_mt)

	local var_sym = AUXIL:scope_add(this.fn, {
		define_in_node=this,
		type_expr=
			te.ftype(this.ret_type or false, this.args or false),
		is_func=true
	})
	if not var_sym then
		print("redefine: ", var, AUXIL:scope_find(var))
	end

	-- this.scope = scope

	return this
end

function binop(l, op, r)
	if l.is_literal and r.is_literal then
		return literal(load('return '..
			tostring(l)..' '..op..' '..tostring(r)..' ;',
		'', 't')())
	end
	return setmetatable({ larg=l, op=op, rarg=r,

	}, binop_mt)
end


function unop(op, uarg)
	local sym = typeof(uarg)

	if uarg.is_uninitialized then
		print('\n'..AUXIL:pos(_0e)..
			'unop on uninitialized value '..tostring(uarg))
	end

	if op=='@' then

		if not sym.is_pointer then
			error('\n'..AUXIL:pos(_0e)..
				'@ on non-pointer '..tostring(uarg))
		end

		if sym.base_type.sizeof==0 then
			error('\n'..AUXIL:pos(_0e)..
				'@ on void pointer '..tostring(uarg))
		end

		return setmetatable({
			arg=uarg,
			op=op,
			type_expr=sym.base_type,
			is_lvalue=true
		}, unop_mt)

	elseif op=='$' then

		if not uarg.is_lvalue then
			error('\n'..AUXIL:pos(_0e)..
				'$ on not lvalue '..tostring(uarg))
		end

		return setmetatable({ arg=uarg, op=op,
			type_expr=te.pointer(sym)
		}, unop_mt)

	elseif op=='--' or op=='++' then

		if not uarg.is_lvalue then
			error('\n'..AUXIL:pos(_0e)..
				'$ on not lvalue '..tostring(uarg))
		end

		return setmetatable({ arg=uarg, op=op,
			type_expr=sym
		}, unop_mt)

	end

	return setmetatable({
		arg=uarg, op=op, type_expr=sym
	}, unop_mt)
end

function block(p, b, e)
	return setmetatable({ auxil=p, b=b or '', e=e or '' }, block_mt)
end

-- function define_args(p, b, e)
-- 	return setmetatable({ auxil=p, b=b or '', e=e or '' }, define_args_mt)
-- end

function call(fn, ...)
	return setmetatable({ fn=fn }, call_mt)
end

function if_then(cond, th, el)
	return setmetatable({ cond=cond, th=th, el=el }, if_then_mt)
end




function literal_mt:__tostring()
	return tostring(self.value)
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

function define_func_mt:__tostring()
	local s = 'function '..
		(self.ret_type~=nil and tostring(self.ret_type)..': ' or '')..
		tostring(self.fn)..'('..tostring(self.args or '')..') '..
		tostring(self.body or ' ')..'end'
	-- if self.args then
	-- 	-- for k=1, #self.args do
	-- 	-- 	s = s..tostring(self.args[k])..
	-- 	-- 		(k==#self.args and '' or ', ')
	-- 	-- end
	-- end
	return s
		
end

function define_args_mt:__tostring()
	local s = ''
	for k=1, #self do
		s = s..tostring(self[k])..	(k==#self and '' or ', ')
	end
	return s
end

function define_func_mt.__index:pos()
	return AUXIL:pos(self.s)
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