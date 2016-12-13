/**
 * Copyright Blake Loring <blake_l@parsed.uk> 2015
 */
"use strict";

import Z3 from './Z3';

console.log('Done import');

var ctx = new Z3.Context();
var solver = new Z3.Solver(ctx);

console.log('Compiling RegEx');
let testRegex = Z3.Regex(ctx, /a/);
console.log('Test Regex: ' + testRegex);

let symbol = ctx.mkStringSymbol('HI');
let symbolic = ctx.mkConst(symbol, ctx.mkStringSort());

let stringToBeTest = symbolic;
let seqInRe = ctx.mkSeqInRe(stringToBeTest, testRegex);

solver.assert(seqInRe);

let mdl = solver.getModel();

if (mdl) {
	console.log('Sequence In Re? ' + mdl.eval(seqInRe).asConstant());
	console.log('String To Be Test: ' + mdl.eval(stringToBeTest).asConstant());
} else {
	console.log('Unsat');
}