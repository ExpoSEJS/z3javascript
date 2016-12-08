/**
 * Copyright Blake Loring <blake_l@parsed.uk> 2015
 */
"use strict";

import Z3 from './Z3';

console.log('Done import');

var ctx = new Z3.Context();
var solver = new Z3.Solver(ctx);

let testRegex = Z3.Regex(ctx, /a(bc)*d(a|b)/);

console.log('Test Regex: ' + testRegex);

let stringToBeTest = ctx.mkString('abcbcda');
let seqInRe = ctx.mkSeqInRe(stringToBeTest, testRegex);

solver.assert(seqInRe);

let mdl = solver.getModel();

if (mdl) {
	console.log(mdl.eval(stringToBeTest).toPrettyString());
	console.log(mdl.eval(seqInRe).toPrettyString());
} else {
	console.log('Unsat');
}