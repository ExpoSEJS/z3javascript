/**
 * Copyright Blake Loring <blake_l@parsed.uk> 2015
 */
"use strict";

import Z3 from './Z3';

var ctx = new Z3.Context();
var solver = new Z3.Solver(ctx);

function TestCaptures(regex, given, expectedCaptures) {
	solver.push();
	let compiled = Z3.Regex(ctx, regex);
	let symbolic = ctx.mkConst(ctx.mkStringSymbol('capture'), ctx.mkStringSort());

	//Force capture[0] -> given
	if (given) {
		solver.assert(ctx.mkEq(compiled.implier, ctx.mkString(given)).simplify());
	}

	solver.assert(ctx.mkSeqInRe(symbolic, compiled.ast));
	solver.assert(ctx.mkImplies(ctx.mkSeqInRe(symbolic, compiled.ast), ctx.mkEq(symbolic, compiled.implier)));

	compiled.assertions.forEach(assert => {
		solver.assert(assert.simplify());
	});

	let mdl = solver.getModel();

	if (mdl) {
		if (!regex.test(mdl.eval(symbolic).asConstant())) {
			return false;
		} else {
			let failed = false;

			expectedCaptures.forEach((x, idx) => {
				if (mdl.eval(compiled.captures[idx]).asConstant() != x) {
					console.log('MDL.eval(' + mdl.eval(compiled.captures[idx]).asConstant() + ') != ' + x + ' expected');
					failed = true;
				}
			});

			if (failed) {
				return false;
			}
		}
	} else {
		console.log('Unsat');
		return false;
	}

	solver.pop();

	return true;
}

function Assert(regex, given, expectedCaptures) {
	if (!TestCaptures(regex, given, expectedCaptures || [])) {
		console.log('Solver State\n'+solver.toString());
		throw ('Regex ' + regex + ' failed test given ' + given + ' ExpectedCap ' + expectedCaptures);
	} else {
		console.log('Regex ' + regex + ' passed test given ' + given + ' ExpectedCap ' + expectedCaptures);
	}
}

Assert(/xxx/, 'xxx');
Assert(/(xxx)/, 'xxx', ['xxx']);
Assert(/^([a-zA-Z])+ef$/, 'def', ['def', 'd']);
Assert(/^([a-zA-Z]){5}ef$/, 'abzCDef', ['abzCDef', 'D']);

