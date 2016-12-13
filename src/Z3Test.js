/**
 * Copyright Blake Loring <blake_l@parsed.uk> 2015
 */
"use strict";

import Z3 from './Z3';

console.log('Done import');

var ctx = new Z3.Context();
var solver = new Z3.Solver(ctx);

console.log('Compiling RegEx');

let testRegex = Z3.Regex(ctx, /.+The Date Is: ([1-9]|[1-3][0-9]):([1-9]|10|11|12):[1-9]{4}/);
//let testRegex = Z3.Regex(ctx, /ab$/);


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
	console.log('String To Be Test As Str: ' + mdl.eval(stringToBeTest).toString());
} else {
	console.log('Unsat');
}