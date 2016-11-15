/**
 * Copyright Blake Loring <blake_l@parsed.uk> 2015
 */
"use strict";

import Z3 from './Z3Loader';
import Z3Utils from './Z3Utils';
import Expr from './Expr';

class Model {
    constructor(context, model) {
        this.ctx = context;
        this.mdl = model;
        Z3.Z3_model_inc_ref(this.ctx, this.mdl);
    }

    toString() {
        return Z3.Z3_model_to_string(this.ctx, this.mdl);
    }

    eval(expr) {
        let res = Z3.bindings_model_eval(this.ctx, this.mdl, expr.ast);
        return res ? new Expr(this.ctx, res) : null;
    }

    destroy() {
        Z3.Z3_model_dec_ref(this.ctx, this.mdl);
    }
}

export default Model;