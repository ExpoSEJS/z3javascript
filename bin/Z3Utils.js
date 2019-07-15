"use strict";

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports["default"] = void 0;

/**
 * Copyright Blake Loring <blake_l@parsed.uk> 2015
 */

/**
 * Recursively reduce Expr to AST
 */
function astArray(args) {
  return args.map(function (a) {
    if (a instanceof Array) {
      return astArray(a);
    }

    return a.ast || a;
  });
}

var _default = {
  wrap: function wrap(ctx, ret) {
    return ret;
  },
  astArray: astArray
};
exports["default"] = _default;