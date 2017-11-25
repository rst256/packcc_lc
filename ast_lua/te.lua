local M = {}

local complementation_gen = {
	__tostring = function(parent, child)
			return function(self) return parent(self)..child(self) end
	end,

}

local function complementation(base, this)
	local this = this or {}
	for k, v in pairs(base) do
		if this[k]==nil then
			this[k] = v
		else
			this[k] = complementation_gen[k](v, this[k])
		end
	end
	return this
end

local te_mt = {}
local primary_mt = { __index={} }
local pointer_mt = { __index={ is_pointer=true } }
local array_mt =   { __index={} }
local ftype_mt =   { __index={} }

function M.primary(base_type, qualifier)
	return setmetatable({
		base_type=base_type,
		qualifier=qualifier,
		sizeof=base_type.sizeof
	}, primary_mt)
end

function M.pointer(base_type, qualifier)
	return setmetatable({
		base_type=base_type,
		qualifier=qualifier,
		sizeof=4
	}, pointer_mt)
end

function M.array(base_type, dim, qualifier)
	return setmetatable({
		base_type=base_type,
		dim=dim,
		qualifier=qualifier,
		sizeof=base_type.sizeof*(tonumber(tostring(dim)) or -1)
	}, array_mt)
end

function M.ftype(ret_type, args)
	return setmetatable({
		ret_type=ret_type and ret_type or nil,
		is_func=true,
		args=args and args or nil,
		sizeof=4
	}, ftype_mt)
end

function te_mt:__tostring()
	if self.qualifier then return tostring(self.qualifier)..' ' end
	return ''
end

function primary_mt:__tostring()
	local s = ( self.qualifier~=nil and tostring(self.qualifier)..' ' or '')
	if self.base_type.is_scalar then 
		s = s..tostring(self.base_type)
	else
		s = s..'('..tostring(self.base_type)..')' 		
	end
	return s
end

function pointer_mt:__tostring()
	local s = tostring(self.base_type)
	s = s..'*'
	if self.qualifier then s = s..' '..tostring(self.qualifier) end
	return s
	-- if self.qualifier then return ' '..tostring(self.qualifier)..'*' end
	-- return '*'
end

function array_mt:__tostring()
	local s = tostring(self.base_type)
	if self.qualifier then s = s..tostring(self.qualifier)..' ' end
	s = s..'['..tostring(self.dim)..']'
	return s
end

function ftype_mt:__tostring()
	local s = ( self.qualifier~=nil and tostring(self.qualifier)..' ' or '')..
		'function'..(self.ret_type~=nil and ' '..tostring(self.ret_type)..':' or '')..' ('
	if self.args then
		for k=1, #self.args do
			s = s..tostring(self.args[k])..
				(k==#self.args and '' or ', ')
		end
	end
	s = s..')'
	return s
end

-- complementation(te_mt, primary_mt)
-- complementation(primary_mt, pointer_mt)
-- print(M.primary('int', 'const')..' 666')
-- print(M.pointer('int', 'const')..' 666')


return M