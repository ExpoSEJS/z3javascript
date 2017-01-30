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
	solver.assert(ctx.mkEq(compiled.implier, ctx.mkString(given)).simplify());

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
					console.log(mdl.eval(symbolic).asConstant() + ' error capture ' + idx + ' MDL.eval(' + mdl.eval(compiled.captures[idx]).asConstant() + ') != ' + x + ' expected');
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

function Assert(regex, given, expectedCaptures, not) {
	let test = TestCaptures(regex, given, expectedCaptures || []);
	
	if (not) {
		test = !test;
	}

	if (!test) {
		console.log('Solver State\n'+solver.toString());
		throw ('Regex ' + regex + ' failed test given ' + given + ' ExpectedCap ' + JSON.stringify(expectedCaptures));
	} else {
		console.log('Regex ' + regex + ' passed test given ' + given + ' ExpectedCap ' + JSON.stringify(expectedCaptures));
	}
}

Assert(/xxx/, 'xxx');
Assert(/(xxx)/, 'xxx', ['xxx']);

Assert(/(x)(y)(z)/, 'xyz', ['xyz', 'x', 'y', 'z']);
Assert(/(x)((yz)(z))/, 'xyzz', ['xyzz', 'x', 'yzz', 'yz', 'z']);

//Tests on klene*
Assert(/^(a)*$/, '', ['', '']);
Assert(/^(a)*$/, 'aaa', ['aaa', 'a']);
Assert(/^(a?)*$/, '', ['', '']);
Assert(/^(a?)*$/, 'aaa', ['aaa', 'a']);

//Tests on klene+
Assert(/^([a-z]+)$/, 'aba', ['aba', 'aba']);
Assert(/^([a-zA-Z]+)([a-z]{3})$/, 'abaaa', ['abaaa', 'ab', 'aaa']);
Assert(/^([a-zA-Z]+)(([a-z]){3})$/, 'abaab', ['abaab', 'ab', 'aab', 'b']);

Assert(/^([a-zA-Z])+ef$/, 'def', ['def', 'd']);
Assert(/^([a-zA-Z]){5}ef$/, 'abzCDef', ['abzCDef', 'D']);

//Loop tests on captures
Assert(/^([a-zA-Z]?){5}$/, 'abc', ['abc', '']);
Assert(/^([a-zA-Z]?){5}$/, 'abcde', ['abcde', 'e']);
Assert(/^([a-zA-Z]?){5}$/, 'abcde', ['abcde', ''], true);


