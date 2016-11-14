"use strict";

import Z3 from './Z3Loader';
import Z3Utils from './Z3Utils';
import Expr from './Expr';
import Model from './Model';
import Context from './Context';

class Solver {

    constructor(context) {
        this.ctx = context.ctx;
        this.slv = Z3.Z3_mk_solver(this.ctx);
        Z3.Z3_solver_inc_ref(this.ctx, this.slv);
        this.push();
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