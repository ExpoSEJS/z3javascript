"use strict";

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports["default"] = void 0;

var _Z3Loader = _interopRequireDefault(require("./Z3Loader"));

var _Z3Utils = _interopRequireDefault(require("./Z3Utils"));

var _Expr = _interopRequireDefault(require("./Expr"));

function _interopRequireDefault(obj) { return obj && obj.__esModule ? obj : { "default": obj }; }

function _classCallCheck(instance, Constructor) { if (!(instance instanceof Constructor)) { throw new TypeError("Cannot call a class as a function"); } }

function _defineProperties(target, props) { for (var i = 0; i < props.length; i++) { var descriptor = props[i]; descriptor.enumerable = descriptor.enumerable || false; descriptor.configurable = true; if ("value" in descriptor) descriptor.writable = true; Object.defineProperty(target, descriptor.key, descriptor); } }

function _createClass(Constructor, protoProps, staticProps) { if (protoProps) _defineProperties(Constructor.prototype, protoProps); if (staticProps) _defineProperties(Constructor, staticProps); return Constructor; }

var Context =
/*#__PURE__*/
function () {
  function Context() {
    _classCallCheck(this, Context);

    var config = _Z3Loader["default"].Z3_mk_config();

    _Z3Loader["default"].Z3_set_param_value(config, "model", "true");

    this.ctx = _Z3Loader["default"].Z3_mk_context_rc(config);

    _Z3Loader["default"].Z3_del_config(config);
  }

  _createClass(Context, [{
    key: "_nullExpr",
    value: function _nullExpr() {
      return new _Expr["default"](this, null);
    }
  }, {
    key: "_appendList",
    value: function _appendList(list, l2) {
      return l2 ? list.concat(l2) : list;
    }
    /**
        * TODO: is not recursive on array
        */

  }, {
    key: "_buildChecks",
    value: function _buildChecks(args) {
      var _this = this;

      return args.filter(function (next) {
        return next.checks;
      }).reduce(function (last, next) {
        return _this._appendList(last, next.checks);
      }, []);
    }
  }, {
    key: "_build",
    value: function _build(func) {
      for (var _len = arguments.length, args = new Array(_len > 1 ? _len - 1 : 0), _key = 1; _key < _len; _key++) {
        args[_key - 1] = arguments[_key];
      }

      return this._buildConst.apply(this, [func, this._buildChecks(args, false)].concat(_Z3Utils["default"].astArray(args)));
    }
  }, {
    key: "_buildConst",
    value: function _buildConst(func, checks) {
      for (var _len2 = arguments.length, args = new Array(_len2 > 2 ? _len2 - 2 : 0), _key2 = 2; _key2 < _len2; _key2++) {
        args[_key2 - 2] = arguments[_key2];
      }

      var fnResult = func.apply(this, [this.ctx].concat(args));
      return new _Expr["default"](this, fnResult, checks);
    }
  }, {
    key: "_buildVar",
    value: function _buildVar(func) {
      for (var _len3 = arguments.length, args = new Array(_len3 > 1 ? _len3 - 1 : 0), _key3 = 1; _key3 < _len3; _key3++) {
        args[_key3 - 1] = arguments[_key3];
      }

      return this._buildVarNoArgs(func, args);
    }
  }, {
    key: "_buildVarNoArgs",
    value: function _buildVarNoArgs(func, args) {
      return new _Expr["default"](this, func(this.ctx, args.length, _Z3Utils["default"].astArray(args)), this._buildChecks(args, false));
    }
  }, {
    key: "destroy",
    value: function destroy() {
      return this._build(_Z3Loader["default"].Z3_del_context);
    }
  }, {
    key: "incRef",
    value: function incRef(e) {
      _Z3Loader["default"].Z3_inc_ref(this.ctx, e.ast);
    }
  }, {
    key: "decRef",
    value: function decRef(e) {
      _Z3Loader["default"].Z3_dec_ref(this.ctx, e.ast);
    }
  }, {
    key: "mkApp",
    value: function mkApp(func, args) {
      return this._build(_Z3Loader["default"].Z3_mk_app, func, args.length, args);
    }
  }, {
    key: "mkArray",
    value: function mkArray(name, baseSort) {
      var arraySort = this.mkArraySort(this.mkIntSort(), baseSort);
      var arrayInstance = this.mkVar(name, arraySort);
      var arrayLen = this.mkIntVar(name + "_Array_Length");
      return arrayInstance.setLength(arrayLen);
    }
  }, {
    key: "mkObject",
    value: function mkObject(name, baseSort) {
      var objectSort = this.mkArraySort(this.mkStringSort(), baseSort);
      var objectInstance = this.mkVar(name, objectSort);
      return objectInstance;
    }
  }, {
    key: "mkGetSort",
    value: function mkGetSort(e) {
      return _Z3Loader["default"].Z3_get_sort(this.ctx, e.ast);
    }
  }, {
    key: "mkSortName",
    value: function mkSortName(sort) {
      return _Z3Loader["default"].Z3_get_sort_name(this.ctx, sort);
    }
  }, {
    key: "mkSymbolString",
    value: function mkSymbolString(s) {
      return _Z3Loader["default"].Z3_get_symbol_string(this.ctx, s);
    }
  }, {
    key: "mkVar",
    value: function mkVar(name, sort) {
      return this._build(_Z3Loader["default"].Z3_mk_const, this.mkStringSymbol(name), sort);
    }
  }, {
    key: "mkIntVar",
    value: function mkIntVar(name) {
      return this.mkVar(name, this.mkIntSort());
    }
  }, {
    key: "mkRealVar",
    value: function mkRealVar(name) {
      return this.mkVar(name, this.mkRealSort());
    }
  }, {
    key: "mkBoolVar",
    value: function mkBoolVar(name) {
      return this.mkVar(name, this.mkBoolSort());
    }
  }, {
    key: "mkString",
    value: function mkString(val) {
      return this._buildConst(_Z3Loader["default"].Z3_mk_string, [], val);
    }
  }, {
    key: "mkStringVar",
    value: function mkStringVar(name) {
      return this.mkVar(name, this.mkStringSort());
    }
  }, {
    key: "mkIntVal",
    value: function mkIntVal(val) {
      return this.mkInt(val, this.mkIntSort());
    }
  }, {
    key: "mkUnsignedIntVal",
    value: function mkUnsignedIntVal(val) {
      return this.mkUnsignedInt(val, this.mkIntSort());
    }
  }, {
    key: "mkSeqLength",
    value: function mkSeqLength(val) {
      return this._build(_Z3Loader["default"].Z3_mk_seq_length, val);
    }
  }, {
    key: "mkSeqAt",
    value: function mkSeqAt(val, off) {
      return this._build(_Z3Loader["default"].Z3_mk_seq_at, val, off);
    }
  }, {
    key: "mkSeqContains",
    value: function mkSeqContains(val1, val2) {
      return this._build(_Z3Loader["default"].Z3_mk_seq_contains, val1, val2);
    }
  }, {
    key: "mkSeqConcat",
    value: function mkSeqConcat(strings) {
      return this._buildVarNoArgs(_Z3Loader["default"].Z3_mk_seq_concat, strings);
    }
  }, {
    key: "mkSeqSubstr",
    value: function mkSeqSubstr(str, offset, length) {
      if (!length) {
        length = this._nullExpr();
      }

      return this._build(_Z3Loader["default"].Z3_mk_seq_extract, str, offset, length);
    }
  }, {
    key: "mkSeqIndexOf",
    value: function mkSeqIndexOf(str, str2, off) {
      return this._build(_Z3Loader["default"].Z3_mk_seq_index, str, str2, off);
    }
  }, {
    key: "mkStrToInt",
    value: function mkStrToInt(str) {
      return this._build(_Z3Loader["default"].Z3_mk_str_to_int, str);
    }
  }, {
    key: "mkIntToStr",
    value: function mkIntToStr(num) {
      return this._build(_Z3Loader["default"].Z3_mk_int_to_str, num);
    }
  }, {
    key: "mkSeqInRe",
    value: function mkSeqInRe(seq, re) {
      return this._build(_Z3Loader["default"].Z3_mk_seq_in_re, seq, re);
    }
  }, {
    key: "mkReConcat",
    value: function mkReConcat(re1, re2) {
      return this._buildVar(_Z3Loader["default"].Z3_mk_re_concat, re1, re2);
    }
  }, {
    key: "mkReEmpty",
    value: function mkReEmpty() {
      return this._build(_Z3Loader["default"].Z3_mk_re_empty, this.mkReSort(this.mkStringSort()));
    }
  }, {
    key: "mkReFull",
    value: function mkReFull() {
      return this._build(_Z3Loader["default"].Z3_mk_re_full, this.mkStringSort());
    }
  }, {
    key: "mkReOption",
    value: function mkReOption(re) {
      return this._build(_Z3Loader["default"].Z3_mk_re_option, re);
    }
  }, {
    key: "mkReStar",
    value: function mkReStar(re) {
      return this._build(_Z3Loader["default"].Z3_mk_re_star, re);
    }
  }, {
    key: "mkReUnion",
    value: function mkReUnion(re1, re2) {
      return this._buildVar(_Z3Loader["default"].Z3_mk_re_union, re1, re2);
    }
  }, {
    key: "mkReIntersect",
    value: function mkReIntersect(re1, re2) {
      return this._buildVar(_Z3Loader["default"].Z3_mk_re_intersect, re1, re2);
    }
  }, {
    key: "mkReComplement",
    value: function mkReComplement(re) {
      return this._build(_Z3Loader["default"].Z3_mk_re_complement, re);
    }
  }, {
    key: "mkRePlus",
    value: function mkRePlus(re) {
      return this._build(_Z3Loader["default"].Z3_mk_re_plus, re);
    }
  }, {
    key: "mkReRange",
    value: function mkReRange(ch1, ch2) {
      return this._build(_Z3Loader["default"].Z3_mk_re_range, ch1, ch2);
    }
  }, {
    key: "mkReLoop",
    value: function mkReLoop(re, lo, hi) {
      return this._build(_Z3Loader["default"].Z3_mk_re_loop, re, lo, hi);
    }
  }, {
    key: "mkSeqToRe",
    value: function mkSeqToRe(seq) {
      return this._build(_Z3Loader["default"].Z3_mk_seq_to_re, seq);
    }
  }, {
    key: "isString",
    value: function isString(ast) {
      return this.build(_Z3Loader["default"].Z3_is_string, ast) === _Z3Loader["default"].TRUE;
    }
  }, {
    key: "mkBoolSort",
    value: function mkBoolSort() {
      return _Z3Loader["default"].Z3_mk_bool_sort(this.ctx);
    }
  }, {
    key: "mkStringSort",
    value: function mkStringSort() {
      return _Z3Loader["default"].Z3_mk_string_sort(this.ctx);
    }
  }, {
    key: "mkIntSort",
    value: function mkIntSort() {
      return _Z3Loader["default"].Z3_mk_int_sort(this.ctx);
    }
  }, {
    key: "mkSeqSort",
    value: function mkSeqSort(sort) {
      return _Z3Loader["default"].Z3_mk_seq_sort(this.ctx, sort);
    }
  }, {
    key: "mkReSort",
    value: function mkReSort(sort) {
      return _Z3Loader["default"].Z3_mk_re_sort(this.ctx, sort);
    }
  }, {
    key: "mkIntSymbol",
    value: function mkIntSymbol(val) {
      return _Z3Loader["default"].Z3_mk_int_symbol(this.ctx, val);
    }
  }, {
    key: "mkStringSymbol",
    value: function mkStringSymbol(val) {
      return _Z3Loader["default"].Z3_mk_string_symbol(this.ctx, val);
    }
  }, {
    key: "mkConst",
    value: function mkConst(symb, sort) {
      return this._build(_Z3Loader["default"].Z3_mk_const, symb, sort);
    }
  }, {
    key: "mkFunc",
    value: function mkFunc(name, args, sort) {
      return this._build(_Z3Loader["default"].Z3_mk_func_decl, name, args.length, args, sort);
    }
  }, {
    key: "mkRecFunc",
    value: function mkRecFunc(name, args, sort) {
      return this._build(_Z3Loader["default"].Z3_mk_rec_func_decl, name, args.length, args, sort);
    }
  }, {
    key: "mkRecFuncDef",
    value: function mkRecFuncDef(fn, args, body) {
      return this._build(_Z3Loader["default"].Z3_add_rec_func_decl, fn, args.length, args, body);
    }
    /**
        * Propositional logic and equality
        */

  }, {
    key: "mkTrue",
    value: function mkTrue() {
      return this._build(_Z3Loader["default"].Z3_mk_true);
    }
  }, {
    key: "mkFalse",
    value: function mkFalse() {
      return this._build(_Z3Loader["default"].Z3_mk_false);
    }
  }, {
    key: "mkEq",
    value: function mkEq(left, right) {
      return this._build(_Z3Loader["default"].Z3_mk_eq, left, right);
    } //missing: distinct

  }, {
    key: "mkNot",
    value: function mkNot(arg) {
      return this._build(_Z3Loader["default"].Z3_mk_not, arg);
    }
  }, {
    key: "mkIte",
    value: function mkIte(ifarg, thenarg, elsearg) {
      return this._build(_Z3Loader["default"].Z3_mk_ite, ifarg, thenarg, elsearg);
    }
  }, {
    key: "mkIff",
    value: function mkIff(left, right) {
      return this._build(_Z3Loader["default"].Z3_mk_iff, left, right);
    }
  }, {
    key: "mkImplies",
    value: function mkImplies(left, right) {
      return this._build(_Z3Loader["default"].Z3_mk_implies, left, right);
    }
  }, {
    key: "mkXOr",
    value: function mkXOr(left, right) {
      return this._build(_Z3Loader["default"].Z3_mk_xor, left, right);
    }
  }, {
    key: "mkAnd",
    value: function mkAnd(left, right) {
      return this._buildVar(_Z3Loader["default"].Z3_mk_and, left, right);
    }
  }, {
    key: "mkAndList",
    value: function mkAndList(conditions) {
      return this._buildVarNoArgs(_Z3Loader["default"].Z3_mk_and, conditions);
    }
  }, {
    key: "mkOr",
    value: function mkOr(left, right) {
      return this._buildVar(_Z3Loader["default"].Z3_mk_or, left, right);
    }
    /**
        * Arithmetic: Integers and Reals
        */

  }, {
    key: "mkRealSort",
    value: function mkRealSort() {
      return _Z3Loader["default"].Z3_mk_real_sort(this.ctx);
    }
  }, {
    key: "mkDoubleSort",
    value: function mkDoubleSort() {
      return _Z3Loader["default"].Z3_mk_fpa_sort_64(this.ctx);
    }
  }, {
    key: "mkAdd",
    value: function mkAdd(left, right) {
      return this._buildVar(_Z3Loader["default"].Z3_mk_add, left, right);
    }
  }, {
    key: "mkMul",
    value: function mkMul(left, right) {
      return this._buildVar(_Z3Loader["default"].Z3_mk_mul, left, right);
    }
  }, {
    key: "mkSub",
    value: function mkSub(left, right) {
      return this._buildVar(_Z3Loader["default"].Z3_mk_sub, left, right);
    }
  }, {
    key: "mkUnaryMinus",
    value: function mkUnaryMinus(arg) {
      return this._build(_Z3Loader["default"].Z3_mk_unary_minus, arg);
    }
  }, {
    key: "mkDiv",
    value: function mkDiv(arg1, arg2) {
      return this._build(_Z3Loader["default"].Z3_mk_div, arg1, arg2);
    }
  }, {
    key: "mkBitwiseShiftLeft",
    value: function mkBitwiseShiftLeft(arg1, arg2) {
      return this._build(_Z3Loader["default"].Z3_mk_bvshl, arg1, arg2);
    }
  }, {
    key: "mkBitwiseShiftRight",
    value: function mkBitwiseShiftRight(arg1, arg2) {
      return this._build(_Z3Loader["default"].Z3_mk_bvlshr, arg1, arg2);
    }
  }, {
    key: "mkMod",
    value: function mkMod(arg1, arg2) {
      return this._build(_Z3Loader["default"].Z3_mk_mod, arg1, arg2);
    }
  }, {
    key: "mkRem",
    value: function mkRem(arg1, arg2) {
      return this._build(_Z3Loader["default"].Z3_mk_rem, arg1, arg2);
    }
  }, {
    key: "mkPower",
    value: function mkPower(arg1, arg2) {
      return this._build(_Z3Loader["default"].Z3_mk_power, arg1, arg2);
    }
  }, {
    key: "mkLt",
    value: function mkLt(left, right) {
      return this._build(_Z3Loader["default"].Z3_mk_lt, left, right);
    }
  }, {
    key: "mkLe",
    value: function mkLe(left, right) {
      return this._build(_Z3Loader["default"].Z3_mk_le, left, right);
    }
  }, {
    key: "mkGt",
    value: function mkGt(left, right) {
      return this._build(_Z3Loader["default"].Z3_mk_gt, left, right);
    }
  }, {
    key: "mkGe",
    value: function mkGe(left, right) {
      return this._build(_Z3Loader["default"].Z3_mk_ge, left, right);
    }
  }, {
    key: "mkRealToInt",
    value: function mkRealToInt(real) {
      return this._build(_Z3Loader["default"].Z3_mk_real2int, real);
    }
  }, {
    key: "mkIntToReal",
    value: function mkIntToReal(ival) {
      return this._build(_Z3Loader["default"].Z3_mk_int2real, ival);
    }
  }, {
    key: "mkIntToBv",
    value: function mkIntToBv(ival) {
      return this._build(_Z3Loader["default"].Z3_mk_int2bv, 32, ival);
    }
  }, {
    key: "mkBvToInt",
    value: function mkBvToInt(bval) {
      return this._build(_Z3Loader["default"].Z3_mk_bv2int, bval, true);
    }
  }, {
    key: "mkIsInt",
    value: function mkIsInt(arg) {
      return this._build(_Z3Loader["default"].Z3_mk_is_int, arg);
    }
    /**
        * Numerals
        */

  }, {
    key: "mkNumeral",
    value: function mkNumeral(numeral, sort) {
      return this._build(_Z3Loader["default"].Z3_mk_numeral, numeral, sort);
    }
  }, {
    key: "mkReal",
    value: function mkReal(num, den) {
      return this._build(_Z3Loader["default"].Z3_mk_real, num, den);
    }
  }, {
    key: "mkInt",
    value: function mkInt(v, sort) {
      return this._build(_Z3Loader["default"].Z3_mk_int, v, sort);
    }
  }, {
    key: "mkUnsignedInt",
    value: function mkUnsignedInt(v, sort) {
      return this._build(_Z3Loader["default"].Z3_mk_unsigned_int, v, sort);
    }
  }, {
    key: "mkInt64",
    value: function mkInt64(v, sort) {
      return this._build(_Z3Loader["default"].Z3_mk_int64, v, sort);
    }
  }, {
    key: "mkUnsignedInt64",
    value: function mkUnsignedInt64(v, sort) {
      return this._build(_Z3Loader["default"].Z3_mk_unsigned_int64, v, sort);
    }
  }, {
    key: "mkToString",
    value: function mkToString(e) {
      return _Z3Loader["default"].Z3_ast_to_string(this.ctx, e.ast || e);
    }
    /**
        * Arrays
        */

  }, {
    key: "mkArraySort",
    value: function mkArraySort(indexSort, elemSort) {
      return this._build(_Z3Loader["default"].Z3_mk_array_sort, indexSort, elemSort);
    }
  }, {
    key: "mkSelect",
    value: function mkSelect(array, index) {
      return this._build(_Z3Loader["default"].Z3_mk_select, array, index);
    }
  }, {
    key: "mkStore",
    value: function mkStore(array, index, v) {
      return this._build(_Z3Loader["default"].Z3_mk_store, array, index, v).setLength(array.getLength());
    }
  }, {
    key: "mkConstArray",
    value: function mkConstArray(sort, v) {
      return this._build(_Z3Loader["default"].Z3_mk_const_array, sort, v).setLength(this.mkIntVal(v.length));
    }
    /**
        * Quantifiers
        * SEE: https://stackoverflow.com/questions/9777381/c-api-for-quantifiers
        */
    /// https://z3prover.github.io/api/html/group__capi.html#gaa80db40fee2eb0124922726e1db97b43

  }, {
    key: "mkBound",
    value: function mkBound(index, sort) {
      return this._build(_Z3Loader["default"].Z3_mk_bound, index, sort);
    } /// Weight, and patterns are optional. Bound should be an array of consts.

  }, {
    key: "mkForAllConst",
    value: function mkForAllConst(bound, body) {
      var patterns = arguments.length > 2 && arguments[2] !== undefined ? arguments[2] : [];
      var weight = arguments.length > 3 && arguments[3] !== undefined ? arguments[3] : 0;
      return this._build(_Z3Loader["default"].mkForAllConst, weight, bound.length, bound, patterns.length, patterns, body);
    }
  }, {
    key: "mkForAll",
    value: function mkForAll(decl_names, sorts, body) {
      var patterns = arguments.length > 3 && arguments[3] !== undefined ? arguments[3] : [];
      var weight = arguments.length > 4 && arguments[4] !== undefined ? arguments[4] : 0;
      return this._build(_Z3Loader["default"].Z3_mk_forall, weight, patterns.length, patterns, decl_names.length, [sorts], decl_names, body);
    }
  }, {
    key: "mkExists",
    value: function mkExists(decl_names, sorts, body) {
      var patterns = arguments.length > 3 && arguments[3] !== undefined ? arguments[3] : [];
      var weight = arguments.length > 4 && arguments[4] !== undefined ? arguments[4] : 0;
      return this._build(_Z3Loader["default"].Z3_mk_exists, weight, patterns.length, patterns, decl_names.length, [sorts], decl_names, body);
    } /// Weight, and patterns are optional. Bound should be an array of consts.

  }, {
    key: "mkExistsConst",
    value: function mkExistsConst(bound, body) {
      var patterns = arguments.length > 2 && arguments[2] !== undefined ? arguments[2] : [];
      var weight = arguments.length > 3 && arguments[3] !== undefined ? arguments[3] : 0;
      return this._build(_Z3Loader["default"].Z3_mk_exists_const, weight, bound.length, bound, patterns.length, patterns, body);
    }
  }, {
    key: "mkPattern",
    value: function mkPattern(terms) {
      return this._build(_Z3Loader["default"].Z3_mk_pattern, terms.length, terms);
    }
  }]);

  return Context;
}();

var _default = Context;
exports["default"] = _default;