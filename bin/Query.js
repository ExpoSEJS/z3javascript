"use strict";

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports["default"] = void 0;

function _classCallCheck(instance, Constructor) { if (!(instance instanceof Constructor)) { throw new TypeError("Cannot call a class as a function"); } }

function _defineProperties(target, props) { for (var i = 0; i < props.length; i++) { var descriptor = props[i]; descriptor.enumerable = descriptor.enumerable || false; descriptor.configurable = true; if ("value" in descriptor) descriptor.writable = true; Object.defineProperty(target, descriptor.key, descriptor); } }

function _createClass(Constructor, protoProps, staticProps) { if (protoProps) _defineProperties(Constructor.prototype, protoProps); if (staticProps) _defineProperties(Constructor, staticProps); return Constructor; }

/**
 * Copyright Blake Loring <blake_l@parsed.uk>
 */
var Query =
/*#__PURE__*/
function () {
  function Query(exprs, checks) {
    _classCallCheck(this, Query);

    this.exprs = exprs;
    this.checks = checks;
  }

  _createClass(Query, [{
    key: "getModel",
    value: function getModel(solver) {
      return Query.process(solver, [this]);
    }
  }]);

  return Query;
}();

Query.MAX_REFINEMENTS = -1;
Query.TOTAL = 0;
Query.LAST_ATTEMPTS = 0;

Query.canAttempt = function (currentAttempts) {
  return Query.MAX_REFINEMENTS == -1 || currentAttempts < Query.MAX_REFINEMENTS;
};

Query.process = function (solver, alternatives) {
  var attempts = 0;
  var model = null;

  var _loop = function _loop() {
    attempts++;
    Query.TOTAL++;
    var next = alternatives.splice(Math.floor(Math.random() * alternatives.length), 1)[0];
    solver.push();
    next.exprs.forEach(function (clause) {
      return solver.assert(clause);
    });
    model = solver.getModel();
    solver.pop();

    if (model) {
      //Run all the checks and concat any alternatives
      var all_checks = next.checks.map(function (check) {
        return check(next, model);
      }).filter(function (x) {
        return !!x;
      });
      alternatives = all_checks.reduce(function (alt, next) {
        return alt.concat(next.alternatives);
      }, alternatives); //Find any failing check

      var failed_check = all_checks.find(function (check) {
        return !check.isSAT;
      }); //If we have found a satisfying model return it otherwise add alternatives from check

      if (failed_check) {
        model.destroy();
        model = null;
      } else {
        return "break";
      }
    }
  };

  while (Query.canAttempt(attempts) && alternatives.length) {
    var _ret = _loop();

    if (_ret === "break") break;
  }

  Query.LAST_ATTEMPTS = attempts;
  return model;
};

var _default = Query;
exports["default"] = _default;