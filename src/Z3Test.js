/**
 * Copyright Blake Loring <blake_l@parsed.uk> 2015
 */
"use strict";

import Z3 from './Z3';

var ctx = new Z3.Context();
var solver = new Z3.Solver(ctx);

console.log('Compiling RegEx');

let regExToTest = [/^--.+=/, /^--no-.+/, /^--.+/, /^[-+]?(?:\d+(?:\.\d*)?|\.\d+)(e[-+]?\d+)?$/];
let testRegexs = regExToTest.map(r => Z3.Regex(ctx, r));

console.log('Test Regex: ' + JSON.stringify(testRegexs));

let symbol = ctx.mkStringSymbol('HI');
let symbolic = ctx.mkConst(symbol, ctx.mkStringSort());

console.log('Set up HI');

let seqInRes = testRegexs.map(x => ctx.mkSeqInRe(symbolic, x));

console.log('SeqInRes done');

seqInRes.forEach(x => solver.assert(x));

let mdl = solver.getModel();

if (mdl) {

	for (let i = 0; i < seqInRes.length; i++) {
		console.log('Seq In Re: ' + mdl.eval(seqInRes[i]).asConstant());
	}

	for (let i = 0; i < regExToTest.length; i++) {
		if (!regExToTest[i].test(mdl.eval(symbolic).asConstant())) {
			console.log('FAILED');
		} else {
			console.log(mdl.eval(symbolic).asConstant());
			console.log('Checked' + regExToTest[i]);
		}
	}
} else {
	console.log('Unsat');
}