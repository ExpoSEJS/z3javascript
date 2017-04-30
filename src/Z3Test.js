/**
 * Copyright Blake Loring <blake_l@parsed.uk> 2015
 */
"use strict";

import Z3 from './Z3';

const ctx = new Z3.Context();
const solver = new Z3.Solver(ctx);

console.log('Compiling RegEx');

let Origin = /^a+(a)?$/;
let TestRegex = Z3.Regex(ctx, Origin);
let symbolic = ctx.mkConst(ctx.mkStringSymbol('TestC0'), ctx.mkStringSort());

solver.assert(ctx.mkEq(TestRegex.captures[1], ctx.mkString('')));
solver.assert(ctx.mkSeqInRe(symbolic, TestRegex.ast).simplify());
solver.assert(ctx.mkImplies(ctx.mkSeqInRe(symbolic, TestRegex.ast), ctx.mkEq(symbolic, TestRegex.implier)));

TestRegex.assertions.forEach(assert => {
	solver.assert(assert.simplify());
});

function Exists(array1, array2, pred) {
	for (let i = 0; i < array1.length; i++) {
		if (pred(array1[i], array2[i])) {
			return true;
		}
	}

	return false;
}

let query = new Z3.Query([], Z3.Check(model => {
	let real_match = Origin.exec(model.eval(symbolic).asConstant());
	let sym_match = TestRegex.captures.map(cap => model.eval(cap).asConstant()).map(cap => cap == '' ? undefined : cap);
	console.log('Real match ' + JSON.stringify(real_match));
	console.log('Sym Match: ' + JSON.stringify(sym_match));
	if (real_match) {
		return !Exists(real_match, sym_match, (l, r) => l !== r);
	} else {
		return false;
	}
}, model => {
	let real_match = Origin.exec(model.eval(symbolic).asConstant()).map(match => match || '');
	let query_list = TestRegex.captures.map((cap, idx) => ctx.mkEq(ctx.mkString(real_match[idx]), cap));
	return [new Z3.Query(query_list, Z3.Check(m => true, q => []))];
}));

let mdl = query.getModel(solver);

if (mdl) {
		let match = Origin.exec(mdl.eval(symbolic).asConstant());
		console.log('Modelled Match: ' + mdl.eval(symbolic).asConstant());
		
		if (match) {
			console.log('Model: ' + mdl.eval(symbolic).asConstant());
			console.log('Match Length: ' + match.length + ' CapturesLength: ' + TestRegex.captures.length);
			
			for (let i = 0; i < match.length; i++) {
				let modeled_i = mdl.eval(TestRegex.captures[i]).asConstant();
				
				if (modeled_i == '') {
					modeled_i = undefined;
				}

				console.log('Capture: ' + i + ' Match: ' + match[i] + ' Model: ' + modeled_i);
				if (match[i] != modeled_i) {
					console.log('Bad Capture');
				}
			}

		} else {
			console.log('BAD MATCH')
		}
} else {
	console.log('Unsat');
}