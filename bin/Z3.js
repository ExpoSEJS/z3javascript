"use strict";

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports["default"] = void 0;

var _Expr = _interopRequireDefault(require("./Expr"));

var _Model = _interopRequireDefault(require("./Model"));

var _Context = _interopRequireDefault(require("./Context.js"));

var _Solver = _interopRequireDefault(require("./Solver"));

var _Regex = _interopRequireDefault(require("./Regex"));

var _Query = _interopRequireDefault(require("./Query"));

var _Check = _interopRequireDefault(require("./Check"));

function _interopRequireDefault(obj) { return obj && obj.__esModule ? obj : { "default": obj }; }

/**
 * Copyright Blake Loring <blake_l@parsed.uk> 2015
 */

/**
 * A node.js API for Z3. Currently, all objects only increment ref counts but never decrement.
 * Ideally, a garbage collector would call a finalizer on the objects that then decrements the
 * ref counts.
 */
var API = {};
API.Solver = _Solver["default"];
API.Context = _Context["default"];
API.Model = _Model["default"];
API.Expr = _Expr["default"];
API.Regex = _Regex["default"];
API.Query = _Query["default"];
API.Check = _Check["default"];
var _default = API;
exports["default"] = _default;