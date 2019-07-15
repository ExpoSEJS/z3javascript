"use strict";

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports["default"] = void 0;

var _package = _interopRequireDefault(require("./package"));

function _interopRequireDefault(obj) { return obj && obj.__esModule ? obj : { "default": obj }; }

/**
 * Copyright Blake Loring <blake_l@parsed.uk> 2015
 */
var Z3Path = process && process.env.Z3_PATH ? process.env.Z3_PATH : undefined;

var _default = (0, _package["default"])(Z3Path);

exports["default"] = _default;