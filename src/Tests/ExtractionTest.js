/**
 * Copyright Blake Loring <blake_l@parsed.uk> 2015
 */
"use strict";

import Test from './Z3Test.js';
import Z3 from '../Z3.js';

const ctx = new Z3.Context();
const solver = new Z3.Solver(ctx);

let arrayInstance = ctx.mkObject('Obj', ctx.mkStringSort());

let forceSelect = ctx.mkEq(ctx.mkSelect(arrayInstance, ctx.mkString('What')), ctx.mkString('Bob Jenkins'));
arrayInstance.addField(ctx.mkString('What'));

solver.assert(forceSelect);

let mdl = solver.getModel();
console.log(arrayInstance.asConstant(mdl));

process.exit(0);
