"use strict";

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports["default"] = void 0;

var _Z3Loader = _interopRequireDefault(require("./Z3Loader"));

var _Expr = _interopRequireDefault(require("./Expr"));

function _interopRequireDefault(obj) { return obj && obj.__esModule ? obj : { "default": obj }; }

function _classCallCheck(instance, Constructor) { if (!(instance instanceof Constructor)) { throw new TypeError("Cannot call a class as a function"); } }

function _defineProperties(target, props) { for (var i = 0; i < props.length; i++) { var descriptor = props[i]; descriptor.enumerable = descriptor.enumerable || false; descriptor.configurable = true; if ("value" in descriptor) descriptor.writable = true; Object.defineProperty(target, descriptor.key, descriptor); } }

function _createClass(Constructor, protoProps, staticProps) { if (protoProps) _defineProperties(Constructor.prototype, protoProps); if (staticProps) _defineProperties(Constructor, staticProps); return Constructor; }

var Model =
/*#__PURE__*/
function () {
  function Model(context, model) {
    _classCallCheck(this, Model);

    this.context = context;
    this.mdl = model;

    _Z3Loader["default"].Z3_model_inc_ref(this.context.ctx, this.mdl);
  }

  _createClass(Model, [{
    key: "toString",
    value: function toString() {
      return _Z3Loader["default"].Z3_model_to_string(this.context.ctx, this.mdl);
    }
  }, {
    key: "eval",
    value: function _eval(expr) {
      var res = _Z3Loader["default"].bindings_model_eval(this.context.ctx, this.mdl, expr.ast); //TODO: Propogating array lengths like this is horrible, find a better way


      return res ? new _Expr["default"](this.context, res).setLength(expr.getLength()).setFields(expr.getFields()) : null;
    }
  }, {
    key: "destroy",
    value: function destroy() {
      _Z3Loader["default"].Z3_model_dec_ref(this.context.ctx, this.mdl);
    }
  }]);

  return Model;
}();

var _default = Model;
exports["default"] = _default;