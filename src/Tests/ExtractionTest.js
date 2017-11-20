/**
 * Copyright Blake Loring <blake_l@parsed.uk> 2015
 */
"use strict";

import Test from './Z3Test.js';
import Z3 from '../Z3.js';

const ctx = new Z3.Context();
const solver = new Z3.Solver(ctx);

let arraySort = ctx.mkArraySort(ctx.mkIntSort(), ctx.mkRealSort());
let arrayInstance = ctx.mkVar('Arr', arraySort);
let arrayLen = ctx.mkIntVar('Arr_length');

let forceSelect = ctx.mkEq(ctx.mkSelect(arrayInstance, ctx.mkIntVal(5)), ctx.mkIntVal(5));
let forceSelectL = ctx.mkGt(arrayLen, ctx.mkIntVal(5));

let forceSelect2 = ctx.mkEq(ctx.mkSelect(arrayInstance, ctx.mkIntVal(3)), ctx.mkIntVal(0));
let forceSelect2L = ctx.mkGt(arrayLen, ctx.mkIntVal(3));

solver.assert(forceSelect);
solver.assert(forceSelectL);
solver.assert(forceSelect2);
solver.assert(forceSelect2L);

console.log(solver.toString());
let lmdl = solver.getModel();

let arrayLReal = lmdl.eval(arrayLen).asConstant();
let built = [];

for (let i = 0; i < arrayLReal; i++) {
	built.push(lmdl.eval(ctx.mkSelect(arrayInstance, ctx.mkIntVal(i))).asConstant());
}

console.log(JSON.stringify(built));

process.exit(0);
