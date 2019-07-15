/**
 * Copyright Blake Loring <blake_l@parsed.uk> 2015
 */
"use strict";

var _Z3Test = _interopRequireDefault(require("./Z3Test.js"));

var _Z = _interopRequireDefault(require("../Z3.js"));

function _interopRequireDefault(obj) { return obj && obj.__esModule ? obj : { "default": obj }; }

var ctx = new _Z["default"].Context();
var solver = new _Z["default"].Solver(ctx);
var arrayInstance = ctx.mkObject('Obj', ctx.mkStringSort());
var forceSelect = ctx.mkEq(ctx.mkSelect(arrayInstance, ctx.mkString('What')), ctx.mkString('Bob Jenkins'));
arrayInstance.addField(ctx.mkString('What'));
solver.assert(forceSelect);
var mdl = solver.getModel();
console.log(arrayInstance.asConstant(mdl));
process.exit(0);