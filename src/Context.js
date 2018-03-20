/**
 * Copyright Blake Loring <blake_l@parsed.uk> 2015
 */
"use strict";

import Z3 from './Z3Loader';
import Z3Utils from './Z3Utils';
import Expr from './Expr';
import Model from './Model';

class Context {

    constructor(timeout) {
          let config = Z3.Z3_mk_config();

          Z3.Z3_set_param_value(config, "model", "true");

          this.ctx = Z3.Z3_mk_context_rc(config);
          Z3.Z3_del_config(config);
    }
    
    _nullExpr() {
        return new Expr(this, null);
    }

    _appendList(list, l2) {
        return l2 ? list.concat(l2) : list;
    }

    /**
     * TODO: is not recursive on array
     */
    _buildChecks(args, not) {

        let checks = {
            trueCheck: args.filter(next => next.checks).reduce((last, next) => this._appendList(last, next.checks.trueCheck), []),
            falseCheck: args.filter(next => next.checks).reduce((last, next) => this._appendList(last, next.checks.falseCheck), [])
        };

        if (not) {
            let tmp = checks.trueCheck;
            checks.trueCheck = checks.falseCheck;
            checks.falseCheck = checks.trueCheck;
        }

        return checks;
    }

    _flipChecks(expr) {

        let tmp = expr.checks.trueCheck;
        expr.checks.trueCheck = expr.checks.falseCheck;
        expr.checks.falseCheck = tmp;

        return expr;
    }

    _build(func, ...args) {
        return this._buildConst.apply(this, [func, this._buildChecks(args, false)].concat(Z3Utils.astArray(args)));
    }

    _buildConst(func, checks, ...args) {
        let fnResult = func.apply(this, [this.ctx].concat(args));
        return new Expr(this, fnResult, checks);      
    }

    _buildVar(func, ...args) {
        return this._buildVarNoArgs(func, args);
    }
    
    _buildVarNoArgs(func, args) {
        return new Expr(this, func(this.ctx, args.length, Z3Utils.astArray(args)), this._buildChecks(args, false));
    }

    destroy() {
        Z3.Z3_del_context(this.ctx);
    }

    mkArray(name, baseSort) {
        let arraySort = this.mkArraySort(this.mkIntSort(), baseSort);
        let arrayInstance = this.mkVar(name, arraySort);
        let arrayLen = this.mkIntVar(name + '_Array_Length');
        return arrayInstance.setLength(arrayLen);
    }

    mkObject(name, baseSort) {
        let objectSort = this.mkArraySort(this.mkStringSort(), baseSort);
        let objectInstance = this.mkVar(name, objectSort);
        return objectInstance;
    }

    mkVar(name, sort) {
        return new Expr(this, Z3.Z3_mk_const(this.ctx, this.mkStringSymbol(name), sort));
    }

    mkIntVar(name) {
        return this.mkVar(name, this.mkIntSort());
    }

    mkRealVar(name) {
        return this.mkVar(name, this.mkRealSort());
    }

    mkBoolVar(name) {
        return this.mkVar(name, this.mkBoolSort());
    }

    mkString(val) {
        return this._buildConst(Z3.Z3_mk_string, [], val);
    }

    mkStringVar(name) {
        return this.mkVar(name, this.mkStringSort());
    }

    mkIntVal(val) {
        return this.mkInt(val, this.mkIntSort());
    }

    mkUnsignedIntVal(val) {
        return this.mkUnsignedInt(val, this.mkIntSort());
    }

    mkSeqLength(val) {
        return this._build(Z3.Z3_mk_seq_length, val);
    }
    
    mkSeqAt(val, off) {
        return this._build(Z3.Z3_mk_seq_at, val, off);
    }
   
    mkSeqContains(val1, val2) {
        return this._build(Z3.Z3_mk_seq_contains, val1, val2);
    }
    
    mkSeqConcat(strings) {
        return this._buildVarNoArgs(Z3.Z3_mk_seq_concat, strings);
    }
    
    mkSeqSubstr(str, offset, length) {
        
        if (!length) {
            length = this._nullExpr();
        }
        
        return this._build(Z3.Z3_mk_seq_extract, str, offset, length);
    }
    
    mkSeqIndexOf(str, str2, off) {
        return this._build(Z3.Z3_mk_seq_index, str, str2, off);
    }

    mkStrToInt(str) {
        return this._build(Z3.Z3_mk_str_to_int, str);
    }

    mkIntToStr(num) {
        return this._build(Z3.Z3_mk_int_to_str, num);
    }

    mkSeqInRe(seq, re) {
        return this._build(Z3.Z3_mk_seq_in_re, seq, re);
    }

    mkReConcat(re1, re2) {
        return this._buildVar(Z3.Z3_mk_re_concat, re1, re2);
    }

    mkReFull() {
        return new Expr(Z3.Z3_mk_re_full(this.ctx, this.mkStringSort()));
    }

    mkReOption(re) {
        return this._build(Z3.Z3_mk_re_option, re);
    }

    mkReStar(re) {
        return this._build(Z3.Z3_mk_re_star, re);
    }

    mkReUnion(re1, re2) {
        return this._buildVar(Z3.Z3_mk_re_union, re1, re2);
    }

    mkReIntersect(re1, re2) {
        return this._buildVar(Z3.Z3_mk_re_intersect, re1, re2);
    }

    mkReComplement(re) {
        return this._build(Z3.Z3_mk_re_complement, re);
    }

    mkRePlus(re) {
        return this._build(Z3.Z3_mk_re_plus, re);
    }

    mkReRange(ch1, ch2) {
        return this._build(Z3.Z3_mk_re_range, ch1, ch2);
    }

    mkReLoop(re, lo, hi) {
        let fnResult = Z3.Z3_mk_re_loop(this.ctx, re.ast, lo, hi);
        return new Expr(this, fnResult);
    }

    mkSeqToRe(seq) {
        return this._build(Z3.Z3_mk_seq_to_re, seq);
    }

    isString(ast) {
        return Z3.Z3_is_string(this.ctx, ast) === Z3.TRUE;
    }

    mkBoolSort() {
        return Z3.Z3_mk_bool_sort(this.ctx);
    }

    mkStringSort() {
        return Z3.Z3_mk_string_sort(this.ctx);
    }

    mkIntSort() {
        return Z3.Z3_mk_int_sort(this.ctx);
    }

    mkSeqSort(sort) {
        return Z3.Z3_mk_seq_sort(this.ctx, sort);
    }

    mkReSort(sort) {
        return Z3.Z3_mk_re_sort(this.ctx, sort);
    }

    mkIntSymbol(val) {
        return Z3.Z3_mk_int_symbol(this.ctx, val);
    }

    mkStringSymbol(val) {
        return Z3.Z3_mk_string_symbol(this.ctx, val);
    }

    mkConst(symb, sort) {
        return new Expr(this, Z3.Z3_mk_const(this.ctx, symb, sort));
    }

    /**
     * Propositional logic and equality
     */

    mkTrue() {
        return this._build(Z3.Z3_mk_true);
    }

    mkFalse() {
        return this._build(Z3.Z3_mk_false);
    }

    mkEq(left, right) {
        return this._build(Z3.Z3_mk_eq, left, right);
    } 

    //missing: distinct

    mkNot(arg) {
        return this._flipChecks(this._build(Z3.Z3_mk_not, arg));
    }

    mkIte(ifarg, thenarg, elsearg) {
        return this._build(Z3.Z3_mk_ite, ifarg, thenarg, elsearg);
    }

    mkIff(left, right) {
        return this._build(Z3.Z3_mk_iff, left, right);
    }

    mkImplies(left, right) {
        return this._build(Z3.Z3_mk_implies, left, right);
    }

    mkXOr(left, right) {
        return this._build(Z3.Z3_mk_xor, left, right);
    }

    mkAnd(left, right) {
        return this._buildVar(Z3.Z3_mk_and, left, right);
    }

    mkAndList(conditions) {
        return this._buildVarNoArgs(Z3.Z3_mk_and, conditions);
    }

    mkOr(left, right) {
        return this._buildVar(Z3.Z3_mk_or, left, right);
    }

    /**
     * Arithmetic: Integers and Reals
     */

    mkRealSort() {
        return Z3.Z3_mk_real_sort(this.ctx);
    }

    mkDoubleSort() {
        return Z3.Z3_mk_fpa_sort_64(this.ctx);
    }

    mkAdd(left, right) {
        return this._buildVar(Z3.Z3_mk_add, left, right);
    }

    mkMul(left, right) {
        return this._buildVar(Z3.Z3_mk_mul, left, right);
    }

    mkSub(left, right) {
        return this._buildVar(Z3.Z3_mk_sub, left, right);
    }

    mkUnaryMinus(arg) {
        return this._build(Z3.Z3_mk_unary_minus, arg);
    }

    mkDiv(arg1, arg2) {
        return this._build(Z3.Z3_mk_div, arg1, arg2);
    }

    mkBitwiseShiftLeft(arg1, arg2) {
        return this._build(Z3.Z3_mk_bvshl, arg1, arg2);
    }

    mkBitwiseShiftRight(arg1, arg2) {
        return this._build(Z3.Z3_mk_bvlshr, arg1, arg2);
    }

    mkMod(arg1, arg2) {
        return this._build(Z3.Z3_mk_mod, arg1, arg2);
    }

    mkRem(arg1, arg2) {
        return this._build(Z3.Z3_mk_rem, arg1, arg2);
    }

    mkPower(arg1, arg2) {
        return this._build(Z3.Z3_mk_power, arg1, arg2);
    }

    mkLt(left, right) {
        return this._build(Z3.Z3_mk_lt, left, right);
    }

    mkLe(left, right) {
        return this._build(Z3.Z3_mk_le, left, right);
    }

    mkGt(left, right) {
        return this._build(Z3.Z3_mk_gt, left, right);
    }

    mkGe(left, right) {
        return this._build(Z3.Z3_mk_ge, left, right);
    }

    mkRealToInt(real) {
        return this._build(Z3.Z3_mk_real2int, real);
    }

    mkIntToReal(ival) {
        return this._build(Z3.Z3_mk_int2real, ival);
    }

    mkIntToBv(ival) {
        return this._build(Z3.Z3_mk_int2bv, 32, ival);
    }

    mkBvToInt(bval) {
        return this._build(Z3.Z3_mk_bv2int, bval, true);
    }

    mkIsInt(arg) {
        return this._build(Z3.Z3_mk_is_int, arg);
    }

    /**
     * Numerals
     */

    mkNumeral(numeral, sort) {
        return new Expr(this, Z3.Z3_mk_numeral(this.ctx, numeral, sort));
    }

    mkReal(num, den) {
        return new Expr(this, Z3.Z3_mk_real(this.ctx, num, den));
    }

    mkInt(v, sort) {
        return new Expr(this, Z3.Z3_mk_int(this.ctx, v, sort));
    }

    mkUnsignedInt(v, sort) {
        return new Expr(this, Z3.Z3_mk_unsigned_int(this.ctx, v, sort));
    }

    mkInt64(v, sort) {
        return new Expr(this, Z3.Z3_mk_int64(this.ctx, v, sort));
    }

    mkUnsignedInt64(v, sort) {
        return new Expr(this, Z3.Z3_mk_unsigned_int64(this.ctx, v, sort));
    }

    toString(ast) {
        return Z3.Z3_ast_to_string(this.ctx, this.ast);
    }

    /**
     * Arrays
     */

    mkArraySort(indexSort, elemSort) {
        return Z3.Z3_mk_array_sort(this.ctx, indexSort, elemSort);
    }

    mkSelect(array, index) {
        return this._build(Z3.Z3_mk_select, array, index);
    }

    mkStore(array, index, v) {
        return this._build(Z3.Z3_mk_store, array, index, v)
                   .setLength(array.getLength());
    }

    mkConstArray(sort, v) {
        return this._build(Z3.Z3_mk_const_array, sort, v)
                   .setLength(this.mkIntVal(v.length));
    }

    /**
     * Quantifiers
     * SEE: https://stackoverflow.com/questions/9777381/c-api-for-quantifiers
     */

    /// https://z3prover.github.io/api/html/group__capi.html#gaa80db40fee2eb0124922726e1db97b43
    mkBound(index, sort) {
        return this._build(Z3.Z3_mk_bound, index, sort);
    }

    /// Weight, and patterns are optional. Bound should be an array of consts.
    mkForAllConst(bound, body, patterns = [], weight = 0) {
        return this._build(Z3.mkForAllConst, weight, bound.length, bound, patterns.length, patterns, body);
    }

    mkForAll(decl_names, sorts, body, patterns = [], weight = 0) {
        return this._build(Z3.Z3_mk_forall, weight, patterns.length, patterns, decl_names.length, [sorts], decl_names, body);
    }

    mkExists(decl_names, sorts, body, patterns = [], weight = 0) {
        return this._build(Z3.Z3_mk_exists, weight, patterns.length, patterns, decl_names.length, [sorts], decl_names, body);
    }

    /// Weight, and patterns are optional. Bound should be an array of consts.
    mkExistsConst(bound, body, patterns = [], weight = 0) {
        return this._build(Z3.Z3_mk_exists_const, weight, bound.length, bound, patterns.length, patterns, body);
    }

    mkPattern(terms) {
        return this._build(Z3.Z3_mk_pattern, terms.length, terms);
    }
}

export default Context;
