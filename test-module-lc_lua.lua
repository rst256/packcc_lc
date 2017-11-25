l=require'lc_lua1'
p=l.parse_file'lc_lua-test.lc'

function block_tostring(self)
	local s = self.b..'\n'
	for k=1, #self do
		s = s..AUXIL:pos(self[k].s)..('\t'):rep(self.auxil:get_level())..
		(getmetatable(self[k])=='block' and
			block_tostring(self[k]) or tostring(self[k]))
		..'\n'
	end
	return s..('\t'):rep(self.auxil:get_level()-1)..self.e
end

print(block_tostring(p))
--local pe = p[#p-6]
--print(pe)
--print(pe.var)
--sym = AUXIL:scope_find(pe.var)
--print(sym, sym())
--print(sym.define_in_node:pos())
--print(sym.type_expr)
--print('scope')
--for s, ss in AUXIL:scope_pairs() do print(ss, s) end


print'===================================='
i = AUXIL:scope_first()
while i do
	if i.define_in_node then
		printf("%-25s %-12s  %3d  %s\n", i.define_in_node:pos(), tostring(i), i.type_expr.sizeof, tostring(i.type_expr))
	end
	i = l.scope_next(i)
end

--print('sub scope')
--i = p[#p-3].scope:scope_first()
--while i do
--	if i.define_in_node then
--		printf("%-25s %-12s  %3d  %s\n",
--			i.define_in_node:pos(), tostring(i),
--			i.type_expr.sizeof, tostring(i.type_expr)
--		)
--	end
--	i = l.scope_next(i)
--end


--print('auxil')
--i = p[#p-4].body.auxil:scope_first()
--while i do
--	if i.define_in_node then
--		printf("%-25s %-12s  %3d  %s\n",
--			i.define_in_node:pos(), tostring(i),
--			i.type_expr.sizeof, tostring(i.type_expr)
--		)
--	end
--	i = l.scope_next(i)
--end
