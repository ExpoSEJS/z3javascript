"use strict";

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports["default"] = void 0;

/**
 * Copyright Blake Loring <blake_l@parsed.uk> 2015
 */
var Z3Loader = require("./package");

var Z3Path = process && process.env.Z3_PATH ? process.env.Z3_PATH : undefined;

var _default = Z3Loader(Z3Path);

exports["default"] = _default;