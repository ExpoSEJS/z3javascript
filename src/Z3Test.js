/**
 * Copyright Blake Loring <blake_l@parsed.uk> 2015
 */
"use strict";

import Z3 from './Z3';

var ctx = new Z3.Context();
var solver = new Z3.Solver(ctx);

console.log('Compiling RegEx');

let regExToTest = [/^([a-zA-Z])+ef$/];
let testRegexs = regExToTest.map(r => Z3.Regex(ctx, r));

let symbol = ctx.mkStringSymbol('HI');
let symbolic = ctx.mkConst(symbol, ctx.mkStringSort());


solver.assert(ctx.mkEq(testRegexs[0].captures[1], ctx.mkString('f')).simplify());

console.log(testRegexs[0].captures[0].toString());

testRegexs.forEach(regex => {
	solver.assert(ctx.mkSeqInRe(symbolic, regex.ast));
	solver.assert(ctx.mkImplies(ctx.mkSeqInRe(symbolic, regex.ast), ctx.mkEq(symbolic, regex.implier)));
	regex.assertions.forEach(assert => {
		solver.assert(assert.simplify());
	});
});

console.log('Solver ' + solver.toString());

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