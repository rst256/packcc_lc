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

local te_mt = { __concat=function(a, b) return tostring(a)..tostring(b) end}
local primary_mt = {}
local pointer_mt = {}
local te_array_mt = { __concat=function(a, b) return tostring(a)..tostring(b) end}

function M.primary(base_type, qualifier) 
	return setmetatable({ base_type=base_type, qualifier=qualifier }, 
		primary_mt)
end

function M.pointer(base_type, qualifier) 
	return setmetatable({ base_type=base_type, qualifier=qualifier }, 
		pointer_mt)
end

function M.te_array(base_type, dim, qualifier) 
	return setmetatable({ base_type=base_type, dim=dim, qualifier=qualifier }, 
		te_array_mt)
end


function te_mt:__tostring() 
	if self.qualifier then return tostring(self.qualifier)..' ' end
	return ''
end

function primary_mt:__tostring() 
	return tostring(self.base_type)
end

function pointer_mt:__tostring() 
	-- if self.qualifier then return ' '..tostring(self.qualifier)..'*' end
	return '*'
end

function te_array_mt:__tostring() 
	local s = tostring(self.base_type)
	if self.qualifier then s = s..tostring(self.qualifier)..' ' end
	s = s..'['..self.dim..']'
	return s
end

complementation(te_mt, primary_mt)
complementation(primary_mt, pointer_mt)
-- print(M.primary('int', 'const')..' 666')
-- print(M.pointer('int', 'const')..' 666')


return M