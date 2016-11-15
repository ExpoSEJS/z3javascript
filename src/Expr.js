/**
 * Copyright Blake Loring <blake_l@parsed.uk> 2015
 */
"use strict";

import Z3 from './Z3Loader';
import Z3Utils from './Z3Utils';

class Expr {

    constructor(context, ast) {
        this.ctx = context;
        this.ast = ast;
        Z3.Z3_inc_ref(this.ctx, this.ast);
    }

    destroy() {
        Z3.Z3_dec_ref(this.ctx, this.ast);
        this.ast = null;
    }

    toString() {
        return "Expr {" + Z3.Z3_ast_to_string(this.ctx, this.ast) + "}";
    }

    isString() {
        return Z3.Z3_is_string(this.ctx, this.ast);
    }

    toPrettyString() {
        let output = Z3.Z3_ast_to_string(this.ctx, this.ast);
        output = output.replace(/\(not (\S)\)/g, "¬$1");
        output = output.replace("or", "∨");
        return output;
    }

    getBoolValue() {
        return Z3.Z3_get_bool_value(this.ctx, this.ast) == Z3.TRUE;
    }

    asConstant() {
        let kind = Z3.Z3_get_ast_kind(this.ctx, this.ast);

        switch (kind) {

            case Z3.NUMERAL_AST: {
                let num_dec_string = Z3.Z3_get_numeral_decimal_string(this.ctx, this.ast, 30);
                return Number(num_dec_string);
            }

            case Z3.APP_AST: {
                if (this.isString()) {
                    return Z3.Z3_get_string(this.ctx, this.ast);
                } else {
                    return this.getBoolValue();
                }
            }

            default: {
                console.log("Can't get constant of unknown AST kind type:", kind);
                return undefined;
            }
        }
    }

    simplify() {
        let newAst = Z3.Z3_simplify(this.ctx, this.ast);
        Z3.Z3_inc_ref(this.ctx, newAst);
        this.destroy();
        this.ast = newAst;
        return this;
    }
}

export default Expr;
