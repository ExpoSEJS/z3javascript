/**
 * Copyright Blake Loring <blake_l@parsed.uk> 2015
 */
"use strict";

import Z3 from './Z3Loader';
import Z3Utils from './Z3Utils';
import Expr from './Expr';
import Model from './Model';
import Context from './Context';

class Solver {

    constructor(context, timeout) {
        this.context = context;

        let config = Z3.Z3_mk_params(this.context.ctx);
        
        Z3.Z3_params_inc_ref(this.context.ctx, config);

        Z3.Z3_params_set_uint(this.context.ctx, config, Z3.Z3_mk_string_symbol(this.context.ctx, "smt.random_seed"), Math.floor(Math.random() * 10000));
        Z3.Z3_params_set_symbol(this.context.ctx, config, Z3.Z3_mk_string_symbol(this.context.ctx, "smt.string_solver"), Z3.Z3_mk_string_symbol(this.context.ctx, "z3str3"));

        if (timeout) {
            Z3.Z3_params_set_uint(this.context.ctx, config, Z3.Z3_mk_string_symbol(this.context.ctx, "timeout"), timeout);
        }

        this.slv = Z3.Z3_mk_simple_solver(this.context.ctx);
        
        Z3.Z3_solver_inc_ref(this.context.ctx, this.slv);
        Z3.Z3_solver_set_params(this.context.ctx, this.slv, config);
    }

    destroy() {
        Z3.Z3_solver_dec_ref(this.context.ctx, this.slv);
    }

    reset() {
        Z3.Z3_solver_reset(this.context.ctx, this.slv);
    }

    push() {
        Z3.Z3_solver_push(this.context.ctx, this.slv);
    }

    pop() {
        Z3.Z3_solver_pop(this.context.ctx, this.slv, 1);
    }

    check() {
        return Z3.Z3_solver_check(this.context.ctx, this.slv) === Z3.TRUE;
    }

    getModel() {
        if (this.check()) {
            return new Model(this.context, Z3.Z3_solver_get_model(this.context.ctx, this.slv));
        } else {
            return null;
        }
    }

    assert(expr) {
        return Z3.Z3_solver_assert(this.context.ctx, this.slv, expr.ast);
    }

    toString() {
        return "Solver {\n" + Z3.Z3_solver_to_string(this.context.ctx, this.slv) + "}";
    }
};

export default Solver;