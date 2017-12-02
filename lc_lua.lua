function pcc_action_stats_0(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    return block(auxil:clone()) 

end

function pcc_action_stats_1(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    table.insert(self, e) --print(auxil:pos(e.start_pos), e) 

end

function pcc_action_stat_0(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    e.start_pos=_0s e.end_pos=_0e return e 

end

function pcc_action_block_0(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    auxil:scope_sub() return block(auxil:clone(), 'do', 'end') 

end

function pcc_action_block_1(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    table.insert(self, e) 

end

function pcc_action_block_2(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    auxil:scope_up() 

end

function pcc_action__block_0(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    auxil:scope_sub() return block(auxil:clone()) 

end

function pcc_action__block_1(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    table.insert(self, e) 

end

function pcc_action__block_2(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    auxil:scope_up() 

end

function pcc_action_if_then_0(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    return if_then(cond, th, el) 

end

function pcc_action_for_loop_0(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    auxil:scope_sub() 

end

function pcc_action_for_loop_1(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    auxil:scope_up()
    return asn_tmpl("for $loop_vars in $iter do $body end", 'for_loop', {
            loop_vars=loop_vars, iter=iter, body=body
    }) 

end

function pcc_action_while_loop_0(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    return asn_tmpl("while $while_cond do $body end", 'while_loop', {
            while_cond=while_cond, body=body
    }) 

end

function pcc_action_repeat_loop_0(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    return asn_tmpl("repeat $body until $until_cond", 'repeat_loop', {
            until_cond=until_cond, body=body
    }) 

end

function pcc_action_assign_0(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    print('error expr') 

end

function pcc_action_assign_1(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    self = assign(var, value) 
    return self 

end

function pcc_action_define_0(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    return define(e) 

end

function pcc_action_define_1(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    print('error expr') 

end

function pcc_action_define_2(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    self:add(_1, fv) 

end

function pcc_action_define_3(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    print('error expr') 

end

function pcc_action_define_4(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    self:add(_2, nv) 

end

function pcc_action_define_func_0(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    AUXIL:scope_sub() 

end

function pcc_action_define_func_1(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    local scope=AUXIL:clone() 
    auxil:scope_up() 
    return define_func{ 
            fn=_1, args=fn_args, ret_type=rt, 
            body=body, scope=scope 
    }

end

function pcc_action_define_func_2(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    AUXIL:scope_sub() 

end

function pcc_action_define_func_3(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    local scope=AUXIL:clone() 
    auxil:scope_up() 
    return define_func{ 
            fn=_2, args=fn_args,
            body=body, scope=scope 
    }

end

function pcc_action_define_args_0(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    return asn_list({ f }, ', ', 'define_args') 

end

function pcc_action_define_args_1(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    table.insert(self, n) 

end

function pcc_action_define_arg_0(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    return define(t):add(_1) 

end

function pcc_action_ret_stat_0(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    return asn_list({}, ', ', 'ret_stat', 'return ') 

end

function pcc_action_ret_stat_1(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    table.insert(self, vars.first) 

end

function pcc_action_ret_stat_2(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    table.insert(self, vars.next) 

end

function pcc_action_break_cmd_0(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    return asn_simple('break', 'break_cmd') 

end

function pcc_action_continue_cmd_0(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    return asn_simple('continue', 'continue_cmd') 

end

function pcc_action_expression_0(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    return ee 

end

function pcc_action_expression_1(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    return e 

end

function pcc_action_logic_0(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    return binop(larg, _1, rarg); 

end

function pcc_action_logic_1(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    return e 

end

function pcc_action_bitwise_0(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    return binop(larg, _1, rarg); 

end

function pcc_action_bitwise_1(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    return e 

end

function pcc_action_compare_0(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    return binop(larg, _1, rarg); 

end

function pcc_action_compare_1(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    return e 

end

function pcc_action_term_0(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    return binop(vars.l, _1, vars.r); 

end

function pcc_action_term_1(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    return e 

end

function pcc_action_factor_0(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    return binop(larg, _1, rarg) 

end

function pcc_action_factor_1(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    return e 

end

function pcc_action_unop_0(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    return unop(_1, Uarg) 

end

function pcc_action_unop_1(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    return e 

end

function pcc_action_unop_stat_0(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    return unop(_1, Uarg) 

end

function pcc_action_primary_0(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    return literal(_0) 

end

function pcc_action_primary_1(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    return literal(_0) 

end

function pcc_action_primary_2(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    return literal(_0) 

end

function pcc_action_primary_3(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    return literal(_0) 

end

function pcc_action_primary_4(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    return literal(_0) 

end

function pcc_action_primary_5(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    return e 

end

function pcc_action_primary_6(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    return assert(auxil:scope_find(e), '\n'..AUXIL:pos(_0e)..tostring(e)..' undefined') 

end

function pcc_action_primary_7(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    return e 

end

function pcc_action_call_0(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    return { vars.first } 

end

function pcc_action_call_1(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    table.insert(self, vars.next) 

end

function pcc_action_call_2(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    return call(fn, table.unpack(self or {})) 

end

function pcc_action__lvalue_0(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })

end

function pcc_action__lvalue_1(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })

end

function pcc_action__lvalue_2(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    return assert(auxil:scope_find(_0), 
            '\n'..AUXIL:pos(_0e)..tostring(_0)..' undefined') 

end

function pcc_action__lvalue_3(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    return unop('@', obj)  

end

function pcc_action_lvalue_0(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })

end

function pcc_action_lvalue_1(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })

end

function pcc_action_lvalue_2(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    return auxil:get_ident(_0) 

end

function pcc_action_lvalue_3(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    return unop('@', obj)  

end

function pcc_action_type_expr_0(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    e.qualifier='const' return e 

end

function pcc_action_type_expr_1(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    return te.pointer(e) 

end

function pcc_action_type_expr_2(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    return te.array(e, dim) 

end

function pcc_action_type_expr_3(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    return e 

end

function pcc_action_type_primary_0(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    e.qualifier='const' return e 

end

function pcc_action_type_primary_1(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    return e 

end

function pcc_action_base_type_expr_0(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    return e 

end

function pcc_action_base_type_expr_1(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    local sym = AUXIL:scope_find(_0)
    if sym then assert(sym.is_type)
            return te.primary(sym)
    end
    return te.primary(_0) 

end

function pcc_action_base_type_expr_2(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    return te.primary(e) 

end

function pcc_action_type_func_0(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    return te.ftype(rt or false, fn_args or false) 

end

function pcc_action_type_func_args_0(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    return asn_list({ f }, ', ', 'type_func_args') 

end

function pcc_action_type_func_args_1(self, auxil, capture, vars)
	 _ENV = setmetatable(_ENV, {__index=function(s,k) return rawget(vars, k) or rawget(capture, k) end })
    table.insert(self, n) 

end

