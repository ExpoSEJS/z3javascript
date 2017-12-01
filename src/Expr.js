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
        let inner = Z3.Z3_ast_to_string(this.context.ctx, this.ast);
        return "Expr {" + inner + "}";
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

    toNumber() {
        let numeralDecString = Z3.Z3_get_numeral_decimal_string(this.context.ctx, this.ast, 30);
        return Number(numeralDecString);
    }

    // TODO (AF) Replace this, this is horrid
    parseNumber() {
        let numString = this.toString();
        if (numString.includes('seq')) {
            const regex = /Expr {\(seq.unit ([\d\.]+)\)}/g; 
            const matches = regex.exec(numString)
            // console.log('Match is ' + matches[1])
            if (matches.length > 1){
                return Number(matches[1])
            }
            throw new Exception(`Unable to parse number from ${numString}`)
        }
    }

    getAstSortKind() {
        return Z3.Z3_get_sort_kind(this.context.ctx, Z3.Z3_get_sort(this.context.ctx, this.ast));
    }

    getAstSortName() {
        const sortSymbol = Z3.Z3_get_sort_name(this.context.ctx, Z3.Z3_get_sort(this.context.ctx, this.ast));
        return Z3.Z3_get_symbol_string(this.context.ctx, sortSymbol);
    }

    // Use this to get the value at the index of an array 
    selectFromIndex(index) {
        let result = this.context.mkSelect(this, index);
        return result;
    }

    asConstant(model) {
        console.log('Entering As Constant')
        switch (this.getAstSortKind()) {
            
            case Z3.INT_SORT:
            case Z3.REAL_SORT: {
                return this.toNumber();
            }

            case Z3.BOOL_SORT: {
                return this.getBoolValue();
            }

            case Z3.SEQ_SORT: {
                if (this.isString()) {
                    return this.escapeString(Z3.Z3_get_string(this.context.ctx, this.ast));
                } else {
                    throw 'Sequence sort that is not a string'
                }
            }

            case Z3.ARRAY_SORT: {
                console.log(this);
                console.log(`This length ${this.length}`)
                const arrayLength = model.eval(this.length).asConstant(model);                
                let array = [];
                
                for (let i = 0; i < arrayLength; i++) {
                    const targetIndex = this.context.mkIntVal(i);
                    const targetAt = this.selectFromIndex(targetIndex);
                    array.push(model.eval(targetAt).asConstant(model));
                }
                return array;
            }


            default: {
                throw `Can't get constant of unknown AST sort type: ${this.getAstSortName()}`;
                return undefined;
            }
        }
        console.log('Exiting As Constant')
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
