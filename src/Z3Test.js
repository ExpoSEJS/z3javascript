/**
 * Copyright Blake Loring <blake_l@parsed.uk> 2015
 */
"use strict";

import Z3 from './Z3';

var ctx = new Z3.Context();
var solver = new Z3.Solver(ctx);

console.log('Compiling RegEx');

let Origin = /^a+(a)?$/;
let TestRegex = Z3.Regex(ctx, Origin);
let symbolic = ctx.mkConst(ctx.mkStringSymbol('TestC0'), ctx.mkStringSort());

solver.assert(ctx.mkEq(TestRegex.captures[1], ctx.mkString('a')));
solver.assert(ctx.mkSeqInRe(symbolic, TestRegex.ast).simplify());
solver.assert(ctx.mkImplies(ctx.mkSeqInRe(symbolic, TestRegex.ast), ctx.mkEq(symbolic, TestRegex.implier)));
TestRegex.assertions.forEach(assert => {
	solver.assert(assert.simplify());
});

console.log('Solver ' + solver.toString());

let mdl = solver.getModel();

if (mdl) {
		let match = Origin.exec(mdl.eval(symbolic).asConstant());
		console.log('Modelled Match: ' + mdl.eval(symbolic).asConstant());
		
		if (match) {
			console.log('Model: ' + mdl.eval(symbolic).asConstant());
			console.log('Match Length: ' + match.length + ' CapturesLength: ' + TestRegex.captures.length);
			
			for (let i = 0; i < match.length; i++) {
				let modeled_i = mdl.eval(TestRegex.captures[i]).asConstant();
				console.log('Capture: ' + i + ' Match: ' + match[i] + ' Model: ' + modeled_i);
				if (match[i] !== modeled_i) {
					console.log('Bad Capture');
				}
			}

		} else {
			console.log('BAD MATCH')
		}
} else {
	console.log('Unsat');
}