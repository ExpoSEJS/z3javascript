/**
 * Copyright Blake Loring <blake_l@parsed.uk> 2015
 */
"use strict";

import Z3 from './Z3';

var ctx = new Z3.Context();
var solver = new Z3.Solver(ctx);

console.log('Compiling RegEx');

let regExToTest = [/^([a-z]{10})|(b{4})$/];
let testRegexs = regExToTest.map(r => Z3.Regex(ctx, r));

console.log('Test Regex: ' + JSON.stringify(testRegexs));

let symbol = ctx.mkStringSymbol('HI');
let symbolic = ctx.mkConst(symbol, ctx.mkStringSort());

solver.assert(ctx.mkEq(symbolic, ctx.mkString('bbbb')));

testRegexs.forEach(regex => {
	solver.assert(ctx.mkSeqInRe(symbolic, regex.ast));
	solver.assert(ctx.mkImplies(ctx.mkSeqInRe(symbolic, regex.ast), ctx.mkEq(symbolic, regex.implier)));
	regex.assertions.forEach(assert => {
		solver.assert(assert);
	});
});

let mdl = solver.getModel();

if (mdl) {

	for (let i = 0; i < regExToTest.length; i++) {
		if (!regExToTest[i].test(mdl.eval(symbolic).asConstant())) {
			console.log('FAILED');
		} else {
			console.log(mdl.eval(symbolic).asConstant());
			console.log('Checked' + regExToTest[i]);

			testRegexs.forEach(regex => {
				regex.captures.forEach((capture, idx) => {
					console.log('Capture ' + idx + ' ' + mdl.eval(capture).asConstant());
				});
			});
		}
	}
} else {
	console.log('Unsat');
}