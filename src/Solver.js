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

    constructor(context) {
        this.ctx = context.ctx;

        let config = Z3.Z3_mk_params(this.ctx);
        Z3.Z3_params_inc_ref(this.ctx, config);
        Z3.Z3_params_set_uint(this.ctx, config, Z3.Z3_mk_string_symbol(this.ctx, "smt.random_seed"), Math.floor(Math.random()) * 1000000);
        
        this.slv = Z3.Z3_mk_simple_solver(this.ctx);
        Z3.Z3_solver_set_params(this.ctx, this.slv, config);

        Z3.Z3_solver_inc_ref(this.ctx, this.slv);
    }

    destroy() {
        Z3.Z3_solver_dec_ref(this.ctx, this.slv);
    }

    reset() {
        Z3.Z3_solver_reset(this.ctx, this.slv);
    }

    push() {
        Z3.Z3_solver_push(this.ctx, this.slv);
    }

    pop() {
        Z3.Z3_solver_pop(this.ctx, this.slv, 1);
    }

    check() {
        return Z3.Z3_solver_check(this.ctx, this.slv) === Z3.TRUE;
    }

    getModel() {
        if (this.check()) {
            return new Model(this.ctx, Z3.Z3_solver_get_model(this.ctx, this.slv));
        } else {
            return null;
        }
    }

    assert(expr) {
        return Z3.Z3_solver_assert(this.ctx, this.slv, expr.ast);
    }

    toString() {
        return "Solver {\n" + Z3.Z3_solver_to_string(this.ctx, this.slv) + "}";
    }
};

export default Solver;