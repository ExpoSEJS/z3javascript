"use strict";

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports["default"] = void 0;

var _Z3Loader = _interopRequireDefault(require("./Z3Loader"));

var _Model = _interopRequireDefault(require("./Model"));

function _interopRequireDefault(obj) { return obj && obj.__esModule ? obj : { "default": obj }; }

function _classCallCheck(instance, Constructor) { if (!(instance instanceof Constructor)) { throw new TypeError("Cannot call a class as a function"); } }

function _defineProperties(target, props) { for (var i = 0; i < props.length; i++) { var descriptor = props[i]; descriptor.enumerable = descriptor.enumerable || false; descriptor.configurable = true; if ("value" in descriptor) descriptor.writable = true; Object.defineProperty(target, descriptor.key, descriptor); } }

function _createClass(Constructor, protoProps, staticProps) { if (protoProps) _defineProperties(Constructor.prototype, protoProps); if (staticProps) _defineProperties(Constructor, staticProps); return Constructor; }

var Solver =
/*#__PURE__*/
function () {
  function Solver(context, incremental, options) {
    var _this = this;

    _classCallCheck(this, Solver);

    options = options || [];
    this.context = context;

    var config = _Z3Loader["default"].Z3_mk_params(this.context.ctx);

    _Z3Loader["default"].Z3_params_inc_ref(this.context.ctx, config);

    options.forEach(function (option) {
      if (typeof option.value === "number") {
        _Z3Loader["default"].Z3_params_set_uint(_this.context.ctx, config, _Z3Loader["default"].Z3_mk_string_symbol(_this.context.ctx, option.name), option.value);
      } else if (typeof option.value === "string") {
        _Z3Loader["default"].Z3_params_set_symbol(_this.context.ctx, config, _Z3Loader["default"].Z3_mk_string_symbol(_this.context.ctx, option.name), _Z3Loader["default"].Z3_mk_string_symbol(_this.context.ctx, option.value));
      }
    });

    if (incremental) {
      this.slv = _Z3Loader["default"].Z3_mk_simple_solver(this.context.ctx);
    } else {
      var defaultTactic = _Z3Loader["default"].Z3_mk_tactic(this.context.ctx, "default");

      _Z3Loader["default"].Z3_tactic_inc_ref(this.context.ctx, defaultTactic);

      this.slv = _Z3Loader["default"].Z3_mk_solver_from_tactic(this.context.ctx, defaultTactic);
    }

    _Z3Loader["default"].Z3_solver_inc_ref(this.context.ctx, this.slv);

    _Z3Loader["default"].Z3_solver_set_params(this.context.ctx, this.slv, config);
  }

  _createClass(Solver, [{
    key: "destroy",
    value: function destroy() {
      _Z3Loader["default"].Z3_solver_dec_ref(this.context.ctx, this.slv);
    }
  }, {
    key: "reset",
    value: function reset() {
      _Z3Loader["default"].Z3_solver_reset(this.context.ctx, this.slv);
    }
  }, {
    key: "push",
    value: function push() {
      _Z3Loader["default"].Z3_solver_push(this.context.ctx, this.slv);
    }
  }, {
    key: "pop",
    value: function pop() {
      _Z3Loader["default"].Z3_solver_pop(this.context.ctx, this.slv, 1);
    }
  }, {
    key: "check",
    value: function check() {
      return _Z3Loader["default"].Z3_solver_check(this.context.ctx, this.slv) === _Z3Loader["default"].TRUE;
    }
    /**
        * Process an SMT2Lib string and assert it on slv 
        */

  }, {
    key: "fromString",
    value: function fromString(str) {
      _Z3Loader["default"].Z3_solver_from_string(this.context.ctx, this.slv, str);
    }
  }, {
    key: "getModel",
    value: function getModel() {
      if (this.check()) {
        return new _Model["default"](this.context, _Z3Loader["default"].Z3_solver_get_model(this.context.ctx, this.slv));
      } else {
        return null;
      }
    }
  }, {
    key: "assert",
    value: function assert(expr) {
      return _Z3Loader["default"].Z3_solver_assert(this.context.ctx, this.slv, expr.ast);
    }
  }, {
    key: "toString",
    value: function toString() {
      return "Solver {\n" + _Z3Loader["default"].Z3_solver_to_string(this.context.ctx, this.slv) + "}";
    }
  }]);

  return Solver;
}();

var _default = Solver;
exports["default"] = _default;