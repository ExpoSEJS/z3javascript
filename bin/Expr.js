"use strict";

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports["default"] = void 0;

var _Z3Loader = _interopRequireDefault(require("./Z3Loader"));

function _interopRequireDefault(obj) { return obj && obj.__esModule ? obj : { "default": obj }; }

function _classCallCheck(instance, Constructor) { if (!(instance instanceof Constructor)) { throw new TypeError("Cannot call a class as a function"); } }

function _defineProperties(target, props) { for (var i = 0; i < props.length; i++) { var descriptor = props[i]; descriptor.enumerable = descriptor.enumerable || false; descriptor.configurable = true; if ("value" in descriptor) descriptor.writable = true; Object.defineProperty(target, descriptor.key, descriptor); } }

function _createClass(Constructor, protoProps, staticProps) { if (protoProps) _defineProperties(Constructor.prototype, protoProps); if (staticProps) _defineProperties(Constructor, staticProps); return Constructor; }

var Expr =
/*#__PURE__*/
function () {
  function Expr(context, ast, checks) {
    _classCallCheck(this, Expr);

    this.context = context;
    this.ast = ast;
    this.context.incRef(this);
    this._fields = [];
    this.checks = checks || [];
  }
  /**
      * Singleton simplify params, just allocate once per execution and
      * leave it rather than inc_ref and dec_refing each time
      */


  _createClass(Expr, [{
    key: "_simplifyParams",
    value: function _simplifyParams() {
      if (!Expr._simpleParams) {
        var config = _Z3Loader["default"].Z3_mk_params(this.context.ctx);

        _Z3Loader["default"].Z3_params_inc_ref(this.context.ctx, config);

        _Z3Loader["default"].Z3_params_set_bool(this.context.ctx, config, _Z3Loader["default"].Z3_mk_string_symbol(this.context.ctx, "rewriter.elim_to_real"), true);

        Expr._simpleParams = config;
      }

      return Expr._simpleParams;
    }
  }, {
    key: "destroy",
    value: function destroy() {
      this.context.decRef(this);
      this.ast = null;
    }
  }, {
    key: "toString",
    value: function toString() {
      return this.context.mkToString(this);
    }
  }, {
    key: "_sortName",
    value: function _sortName() {
      return this.context.mkSymbolString(this.context.mkSortName(this.context.mkGetSort(this)));
    }
  }, {
    key: "isString",
    value: function isString() {
      return this._sortName() == "String";
    }
  }, {
    key: "toPrettyString",
    value: function toPrettyString() {
      var output = this.context.mkToString(this);
      output = output.replace(/\(not (\S)\)/g, "¬$1");
      output = output.replace("or", "∨");
      return output;
    }
  }, {
    key: "getBoolValue",
    value: function getBoolValue() {
      return _Z3Loader["default"].Z3_get_bool_value(this.context.ctx, this.ast) == _Z3Loader["default"].TRUE;
    }
  }, {
    key: "getRealValue",
    value: function getRealValue() {
      var num_dec_string = _Z3Loader["default"].Z3_get_numeral_decimal_string(this.context.ctx, this.ast, 30);

      return Number(num_dec_string);
    }
  }, {
    key: "escapeString",
    value: function escapeString(str) {
      function replacer(match, p1) {
        var chars = str[p1 + 2] + str[p1 + 3];
        return String.fromCharCode(parseInt(chars, 16));
      }

      function unicodeReplacer(match, p1) {
        var chars = str[p1 + 2] + str[p1 + 3] + str[p1 + 4] + str[p1 + 5];
        return String.fromCharCode(parseInt(chars));
      }

      return str.replace(/\\x[0-9a-fA-F]{2}/g, replacer).replace(/\\u\d{4}/g, unicodeReplacer).replace(/\\a/g, "\a").replace(/\\b/g, "\b").replace(/\\r/g, "\r").replace(/\\v/g, "\v").replace(/\\f/g, "\f").replace(/\\n/g, "\n").replace(/\\t/g, "\t");
    }
  }, {
    key: "getLength",
    value: function getLength() {
      if (this.isString()) {
        return this.context.mkSeqLength(this);
      } else {
        return this._length;
      }
    }
  }, {
    key: "getField",
    value: function getField(indexSymbol) {
      var sort = _Z3Loader["default"].Z3_get_sort(this.context.ctx, this.ast);

      var is_string = _Z3Loader["default"].Z3_is_string_sort(this.context.ctx, sort);

      return is_string ? this.context.mkSeqAt(this, indexSymbol) : this.context.mkSelect(this, indexSymbol);
    }
  }, {
    key: "setField",
    value: function setField(indexSymbol, valueSymbol) {
      return this.context.mkStore(this, indexSymbol, valueSymbol);
    }
  }, {
    key: "setLength",
    value: function setLength(l) {
      this._length = l;
      return this;
    }
  }, {
    key: "getFields",
    value: function getFields() {
      return this._fields;
    }
  }, {
    key: "setFields",
    value: function setFields(f) {
      this._fields = f;
      return this;
    }
  }, {
    key: "addField",
    value: function addField(expr) {
      this._fields.push(expr);

      return this;
    }
  }, {
    key: "asConstant",
    value: function asConstant(mdl) {
      var _this = this;

      var sort = this._sortName();

      if (sort === "Real" || sort === "Int") {
        return this.getRealValue();
      } else if (sort === "Bool") {
        return this.getBoolValue();
      } else if (sort === "String") {
        return this.escapeString(_Z3Loader["default"].Z3_get_string(this.context.ctx, this.ast));
      } else if (this.getLength()) {
        //Array
        //TODO: Propogating array lengths like this is horrible, find a better way
        var len = mdl.eval(this.getLength()).asConstant(mdl);
        var built = [];

        for (var i = 0; i < len; i++) {
          built.push(mdl.eval(this.context.mkSelect(this, this.context.mkIntVal(i))).asConstant(mdl));
        }

        return built;
      } else {
        var obj = {};

        this._fields.forEach(function (field) {
          obj[mdl.eval(field).asConstant(mdl)] = mdl.eval(_this.context.mkSelect(_this, field)).asConstant(mdl);
        });

        return obj;
      }
    }
  }, {
    key: "simplify",
    value: function simplify() {
      var newAst = _Z3Loader["default"].Z3_simplify_ex(this.context.ctx, this.ast, this._simplifyParams());

      _Z3Loader["default"].Z3_inc_ref(this.context.ctx, newAst);

      this.destroy();
      this.ast = newAst;
      return this;
    }
  }]);

  return Expr;
}();

var _default = Expr;
exports["default"] = _default;