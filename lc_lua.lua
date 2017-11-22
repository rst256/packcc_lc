function pcc_action_stats_0(self, auxil, capture, vars)
	 local _ENV = setmetatable({ vars=vars}, {__index=function(s,k) return rawget(_G, k) or rawget(vars, k) or rawget(capture, k) end })
    print(auxil:pos(e.s), e) 

end

function pcc_action_stat_0(self, auxil, capture, vars)
	 local _ENV = setmetatable({ vars=vars}, {__index=function(s,k) return rawget(_G, k) or rawget(vars, k) or rawget(capture, k) end })
    e.s=_0s return e 

end

function pcc_action_block_0(self, auxil, capture, vars)
	 local _ENV = setmetatable({ vars=vars}, {__index=function(s,k) return rawget(_G, k) or rawget(vars, k) or rawget(capture, k) end })
    auxil:scope_sub() return block(auxil:clone(), 'do', 'end') 

end

function pcc_action_block_1(self, auxil, capture, vars)
	 local _ENV = setmetatable({ vars=vars}, {__index=function(s,k) return rawget(_G, k) or rawget(vars, k) or rawget(capture, k) end })
    table.insert(self, e) 

end

function pcc_action_block_2(self, auxil, capture, vars)
	 local _ENV = setmetatable({ vars=vars}, {__index=function(s,k) return rawget(_G, k) or rawget(vars, k) or rawget(capture, k) end })
    auxil:scope_up() 

end

function pcc_action__block_0(self, auxil, capture, vars)
	 local _ENV = setmetatable({ vars=vars}, {__index=function(s,k) return rawget(_G, k) or rawget(vars, k) or rawget(capture, k) end })
    auxil:scope_sub() return block(auxil:clone()) 

end

function pcc_action__block_1(self, auxil, capture, vars)
	 local _ENV = setmetatable({ vars=vars}, {__index=function(s,k) return rawget(_G, k) or rawget(vars, k) or rawget(capture, k) end })
    table.insert(self, e) 

end

function pcc_action__block_2(self, auxil, capture, vars)
	 local _ENV = setmetatable({ vars=vars}, {__index=function(s,k) return rawget(_G, k) or rawget(vars, k) or rawget(capture, k) end })
    auxil:scope_up() 

end

function pcc_action_if_then_0(self, auxil, capture, vars)
	 local _ENV = setmetatable({ vars=vars}, {__index=function(s,k) return rawget(_G, k) or rawget(vars, k) or rawget(capture, k) end })
    return if_then(cond, th, el) 

end

function pcc_action_assign_0(self, auxil, capture, vars)
	 local _ENV = setmetatable({ vars=vars}, {__index=function(s,k) return rawget(_G, k) or rawget(vars, k) or rawget(capture, k) end })
    print('error expr') 

end

function pcc_action_assign_1(self, auxil, capture, vars)
	 local _ENV = setmetatable({ vars=vars}, {__index=function(s,k) return rawget(_G, k) or rawget(vars, k) or rawget(capture, k) end })
    self = assign(var, value) 
    --if not auxil:scope_find(var) then 
    --print(auxil:pos(_0s), "undefined: ", var) 
    -- end
    return self 

end

function pcc_action_define_0(self, auxil, capture, vars)
	 local _ENV = setmetatable({ vars=vars}, {__index=function(s,k) return rawget(_G, k) or rawget(vars, k) or rawget(capture, k) end })
    print('error expr') 

end

function pcc_action_define_1(self, auxil, capture, vars)
	 local _ENV = setmetatable({ vars=vars}, {__index=function(s,k) return rawget(_G, k) or rawget(vars, k) or rawget(capture, k) end })
    self = define(te, _1, value) 
    if not auxil:scope_add(_1, te) then 
            print(auxil:pos(_0s), "redefine: ", _1, auxil:scope_find(_1)) 
    end
    return self 

end

function pcc_action_expression_0(self, auxil, capture, vars)
	 local _ENV = setmetatable({ vars=vars}, {__index=function(s,k) return rawget(_G, k) or rawget(vars, k) or rawget(capture, k) end })
    return ee 

end

function pcc_action_expression_1(self, auxil, capture, vars)
	 local _ENV = setmetatable({ vars=vars}, {__index=function(s,k) return rawget(_G, k) or rawget(vars, k) or rawget(capture, k) end })
    return e 

end

function pcc_action_logic_0(self, auxil, capture, vars)
	 local _ENV = setmetatable({ vars=vars}, {__index=function(s,k) return rawget(_G, k) or rawget(vars, k) or rawget(capture, k) end })
    return binop(l, _1, r); 

end

function pcc_action_logic_1(self, auxil, capture, vars)
	 local _ENV = setmetatable({ vars=vars}, {__index=function(s,k) return rawget(_G, k) or rawget(vars, k) or rawget(capture, k) end })
    return e 

end

function pcc_action_bitwise_0(self, auxil, capture, vars)
	 local _ENV = setmetatable({ vars=vars}, {__index=function(s,k) return rawget(_G, k) or rawget(vars, k) or rawget(capture, k) end })
    return binop(l, _1, r); 

end

function pcc_action_bitwise_1(self, auxil, capture, vars)
	 local _ENV = setmetatable({ vars=vars}, {__index=function(s,k) return rawget(_G, k) or rawget(vars, k) or rawget(capture, k) end })
    return e 

end

function pcc_action_compare_0(self, auxil, capture, vars)
	 local _ENV = setmetatable({ vars=vars}, {__index=function(s,k) return rawget(_G, k) or rawget(vars, k) or rawget(capture, k) end })
    return binop(vars.l, _1, vars.r); 

end

function pcc_action_compare_1(self, auxil, capture, vars)
	 local _ENV = setmetatable({ vars=vars}, {__index=function(s,k) return rawget(_G, k) or rawget(vars, k) or rawget(capture, k) end })
    return e 

end

function pcc_action_term_0(self, auxil, capture, vars)
	 local _ENV = setmetatable({ vars=vars}, {__index=function(s,k) return rawget(_G, k) or rawget(vars, k) or rawget(capture, k) end })
    return binop(vars.l, _1, vars.r); 

end

function pcc_action_term_1(self, auxil, capture, vars)
	 local _ENV = setmetatable({ vars=vars}, {__index=function(s,k) return rawget(_G, k) or rawget(vars, k) or rawget(capture, k) end })
    return e 

end

function pcc_action_factor_0(self, auxil, capture, vars)
	 local _ENV = setmetatable({ vars=vars}, {__index=function(s,k) return rawget(_G, k) or rawget(vars, k) or rawget(capture, k) end })
    return binop(larg, _1, rarg) 

end

function pcc_action_factor_1(self, auxil, capture, vars)
	 local _ENV = setmetatable({ vars=vars}, {__index=function(s,k) return rawget(_G, k) or rawget(vars, k) or rawget(capture, k) end })
    return e 

end

function pcc_action_unop_0(self, auxil, capture, vars)
	 local _ENV = setmetatable({ vars=vars}, {__index=function(s,k) return rawget(_G, k) or rawget(vars, k) or rawget(capture, k) end })
    return unop(_1, arg) 

end

function pcc_action_unop_1(self, auxil, capture, vars)
	 local _ENV = setmetatable({ vars=vars}, {__index=function(s,k) return rawget(_G, k) or rawget(vars, k) or rawget(capture, k) end })
    return e 

end

function pcc_action_primary_0(self, auxil, capture, vars)
	 local _ENV = setmetatable({ vars=vars}, {__index=function(s,k) return rawget(_G, k) or rawget(vars, k) or rawget(capture, k) end })
    return tonumber(_0) 

end

function pcc_action_primary_1(self, auxil, capture, vars)
	 local _ENV = setmetatable({ vars=vars}, {__index=function(s,k) return rawget(_G, k) or rawget(vars, k) or rawget(capture, k) end })
    return tonumber(_0) 

end

function pcc_action_primary_2(self, auxil, capture, vars)
	 local _ENV = setmetatable({ vars=vars}, {__index=function(s,k) return rawget(_G, k) or rawget(vars, k) or rawget(capture, k) end })
    return tonumber(_0) 

end

function pcc_action_primary_3(self, auxil, capture, vars)
	 local _ENV = setmetatable({ vars=vars}, {__index=function(s,k) return rawget(_G, k) or rawget(vars, k) or rawget(capture, k) end })
    return tonumber(_0) 

end

function pcc_action_primary_4(self, auxil, capture, vars)
	 local _ENV = setmetatable({ vars=vars}, {__index=function(s,k) return rawget(_G, k) or rawget(vars, k) or rawget(capture, k) end })
    return (_0) 

end

function pcc_action_primary_5(self, auxil, capture, vars)
	 local _ENV = setmetatable({ vars=vars}, {__index=function(s,k) return rawget(_G, k) or rawget(vars, k) or rawget(capture, k) end })
    return e 

end

function pcc_action_primary_6(self, auxil, capture, vars)
	 local _ENV = setmetatable({ vars=vars}, {__index=function(s,k) return rawget(_G, k) or rawget(vars, k) or rawget(capture, k) end })
    return e 

end

function pcc_action_primary_7(self, auxil, capture, vars)
	 local _ENV = setmetatable({ vars=vars}, {__index=function(s,k) return rawget(_G, k) or rawget(vars, k) or rawget(capture, k) end })
    return e 

end

function pcc_action_call_0(self, auxil, capture, vars)
	 local _ENV = setmetatable({ vars=vars}, {__index=function(s,k) return rawget(_G, k) or rawget(vars, k) or rawget(capture, k) end })
    return call(fn) 

end

function pcc_action_call_1(self, auxil, capture, vars)
	 local _ENV = setmetatable({ vars=vars}, {__index=function(s,k) return rawget(_G, k) or rawget(vars, k) or rawget(capture, k) end })
    self.args = { vars.first } 

end

function pcc_action_call_2(self, auxil, capture, vars)
	 local _ENV = setmetatable({ vars=vars}, {__index=function(s,k) return rawget(_G, k) or rawget(vars, k) or rawget(capture, k) end })
    table.insert(self.args, vars.next) 

end

function pcc_action_lvalue_0(self, auxil, capture, vars)
	 local _ENV = setmetatable({ vars=vars}, {__index=function(s,k) return rawget(_G, k) or rawget(vars, k) or rawget(capture, k) end })

end

function pcc_action_lvalue_1(self, auxil, capture, vars)
	 local _ENV = setmetatable({ vars=vars}, {__index=function(s,k) return rawget(_G, k) or rawget(vars, k) or rawget(capture, k) end })

end

function pcc_action_lvalue_2(self, auxil, capture, vars)
	 local _ENV = setmetatable({ vars=vars}, {__index=function(s,k) return rawget(_G, k) or rawget(vars, k) or rawget(capture, k) end })
    --print(_0, auxil:get_ident(_0), auxil:scope_find(_0)) 
    return _0--{ type='ident', name=_0 } 

end

function pcc_action_lvalue_3(self, auxil, capture, vars)
	 local _ENV = setmetatable({ vars=vars}, {__index=function(s,k) return rawget(_G, k) or rawget(vars, k) or rawget(capture, k) end })
    return unop('@', obj)  

end

function pcc_action_type_expr_0(self, auxil, capture, vars)
	 local _ENV = setmetatable({ vars=vars}, {__index=function(s,k) return rawget(_G, k) or rawget(vars, k) or rawget(capture, k) end })
    return te_pointer(te) 

end

function pcc_action_type_expr_1(self, auxil, capture, vars)
	 local _ENV = setmetatable({ vars=vars}, {__index=function(s,k) return rawget(_G, k) or rawget(vars, k) or rawget(capture, k) end })
    return te_pointer(te, 'const') 

end

function pcc_action_type_expr_2(self, auxil, capture, vars)
	 local _ENV = setmetatable({ vars=vars}, {__index=function(s,k) return rawget(_G, k) or rawget(vars, k) or rawget(capture, k) end })
    return te_array(te, dim) 

end

function pcc_action_type_expr_3(self, auxil, capture, vars)
	 local _ENV = setmetatable({ vars=vars}, {__index=function(s,k) return rawget(_G, k) or rawget(vars, k) or rawget(capture, k) end })
    return te_primary(te, 'const') 

end

function pcc_action_type_expr_4(self, auxil, capture, vars)
	 local _ENV = setmetatable({ vars=vars}, {__index=function(s,k) return rawget(_G, k) or rawget(vars, k) or rawget(capture, k) end })
    return te_primary(_0) 

end

