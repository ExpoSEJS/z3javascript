/**
 * Copyright Blake Loring <blake_l@parsed.uk> 2015
 */
"use strict";

import Test from './Z3Test.js';
import Z3 from '../Z3.js';

const ctx = new Z3.Context();
const solver = new Z3.Solver(ctx);

let arrayInstance = ctx.mkArray('Arr', ctx.mkStringSort());

let forceSelect = ctx.mkEq(ctx.mkSelect(arrayInstance, ctx.mkIntVal(5)), ctx.mkString('Bob Jenkins'));
let forceSelectL = ctx.mkGt(arrayInstance.getLength(), ctx.mkIntVal(5));

let forceSelect2 = ctx.mkEq(ctx.mkSelect(arrayInstance, ctx.mkIntVal(3)), ctx.mkString('Boop'));
let forceSelect2L = ctx.mkGt(arrayInstance.getLength(), ctx.mkIntVal(3));

solver.assert(forceSelect);
solver.assert(forceSelectL);
solver.assert(forceSelect2);
solver.assert(forceSelect2L);

console.log(solver.toString());
let lmdl = solver.getModel();

console.log(JSON.stringify(lmdl.eval(arrayInstance).asConstant(lmdl)));

process.exit(0);
