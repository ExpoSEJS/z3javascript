/**
 * Copyright Blake Loring <blake_l@parsed.uk> 2015
 */
"use strict";

import Z3 from './Z3Loader';
import Z3Utils from './Z3Utils';

class Expr {

    constructor(context, ast, checks) {
        this.context = context;

        this.ast = ast;
        Z3.Z3_inc_ref(this.context.ctx, this.ast);

        this._fields = [];

        this.checks = checks || {
            trueCheck: [],
            falseCheck: []
        }
    }

    /**
     * Singleton simplify params, just allocate once per execution and
     * leave it rather than inc_ref and dec_refing each time
     */
    _simplifyParams() {
        if (!Expr._simpleParams) {
            let config = Z3.Z3_mk_params(this.context.ctx);
            Z3.Z3_params_inc_ref(this.context.ctx, config);
            Z3.Z3_params_set_bool(this.context.ctx, config, Z3.Z3_mk_string_symbol(this.context.ctx, "rewriter.elim_to_real"), true);
            Expr._simpleParams = config;
        }
        return Expr._simpleParams;
    }

    destroy() {
        Z3.Z3_dec_ref(this.context.ctx, this.ast);
        this.ast = null;
    }

    toString() {
        return Z3.Z3_ast_to_string(this.context.ctx, this.ast);
    }

    isString() {
        return Z3.Z3_is_string(this.context.ctx, this.ast);
    }
    
    toPrettyString() {
        let output = Z3.Z3_ast_to_string(this.context.ctx, this.ast);
        output = output.replace(/\(not (\S)\)/g, "¬$1");
        output = output.replace("or", "∨");
        return output;
    }

    getBoolValue() {
        return Z3.Z3_get_bool_value(this.context.ctx, this.ast) == Z3.TRUE;
    }

    getRealValue() {
        let num_dec_string = Z3.Z3_get_numeral_decimal_string(this.context.ctx, this.ast, 30);
        return Number(num_dec_string);
    }

    escapeString(str) {
            function replacer(match, p1) {
                var chars = str[p1 + 2] + str[p1 + 3];
                return String.fromCharCode(parseInt(chars, 16));
            }

            function unicodeReplacer(match, p1) {
                var chars = str[p1 + 2] + str[p1 + 3] + str[p1 + 4] + str[p1 + 5];
                return String.fromCharCode(parseInt(chars));
            }

            return str.replace(/\\x[0-9a-fA-F]{2}/g, replacer).replace(/\\u\d{4}/g, unicodeReplacer).replace(/\\a/g, '\a').replace(/\\b/g, '\b').replace(/\\r/g, '\r').replace(/\\v/g, '\v').replace(/\\f/g, '\f').replace(/\\n/g, '\n').replace(/\\t/g, '\t');  
    }

    getLength() {
        let sort = Z3.Z3_get_sort(this.context.ctx, this.ast);
        if (Z3.Z3_is_string_sort(this.context.ctx, sort)) {
            return this.context.mkSeqLength(this);
        } else {
            return this._length;
        }
    }

    getField(indexSymbol) {
        const sort = Z3.Z3_get_sort(this.context.ctx, this.ast);
        const is_string = Z3.Z3_is_string_sort(this.context.ctx, sort);
        return is_string ? this.context.mkSeqAt(this, indexSymbol) : this.context.mkSelect(this, indexSymbol);
    }

    setField(indexSymbol, valueSymbol) {
        return this.context.mkStore(this, indexSymbol, valueSymbol);
    }

    setLength(l) {
        this._length = l;
        return this;
    }

    getFields() {
        return this._fields;
    }

    setFields(f) {
        this._fields = f;
        return this;
    }

    addField(expr) {
        this._fields.push(expr);
        return this;
    }

    asConstant(mdl) {
        const sort = Z3.Z3_get_symbol_string(this.context.ctx, Z3.Z3_get_sort_name(this.context.ctx, Z3.Z3_get_sort(this.context.ctx, this.ast)));

        if (sort === "Real") {
            return this.getRealValue();
        } else if (sort === "Bool") {
            return this.getBoolValue();
        } else if (sort === "String") {
            return this.escapeString(Z3.Z3_get_string(this.context.ctx, this.ast));
        } else if (this.getLength()) { //Array

            //TODO: Propogating array lengths like this is horrible, find a better way
            let len = mdl.eval(this.getLength()).asConstant(mdl);

            let built = [];

            for (let i = 0; i < len; i++) {
                built.push(mdl.eval(this.context.mkSelect(this, this.context.mkIntVal(i))).asConstant(mdl));
            }

            return built;
        } else {
            let obj = {};

            this._fields.forEach(field => {
                obj[mdl.eval(field).asConstant(mdl)] = mdl.eval(this.context.mkSelect(this, field)).asConstant(mdl);
            });

            return obj;
        }

        throw `Can't get constant of unknown AST kind type: ${sort}`;
    }

    simplify() {
        let newAst = Z3.Z3_simplify_ex(this.context.ctx, this.ast, this._simplifyParams());
        Z3.Z3_inc_ref(this.context.ctx, newAst);
        this.destroy();
        this.ast = newAst;
        return this;
    }
}

export default Expr;
