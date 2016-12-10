/**
 * Copyright Blake Loring <blake_l@parsed.uk> 2015
 */
"use strict";

import Z3 from './Z3';

console.log('Done import');

var ctx = new Z3.Context();
var solver = new Z3.Solver(ctx);

let testRegex = Z3.Regex(ctx, /[^abcdefg]/);

console.log('Test Regex: ' + testRegex);

let symbol = ctx.mkStringSymbol('HI');
let symbolic = ctx.mkConst(symbol, ctx.mkStringSort());

let stringToBeTest = symbolic;
let seqInRe = ctx.mkSeqInRe(stringToBeTest, testRegex);

solver.assert(seqInRe);

let mdl = solver.getModel();

if (mdl) {
	console.log(mdl.eval(stringToBeTest).toString());
	console.log(mdl.eval(seqInRe).toString());
} else {
	console.log('Unsat');
}