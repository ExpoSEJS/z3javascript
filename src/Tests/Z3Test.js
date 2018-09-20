/**
 * Copyright Blake Loring <blake_l@parsed.uk> 2015
 */
"use strict";

import Z3 from '../Z3';

const ctx = new Z3.Context();
const solver = new Z3.Solver(ctx, false, []);

function Test(Origin) {
	solver.reset();

	let TestRegex = Z3.Regex(ctx, Origin);
	let symbolic = ctx.mkConst(ctx.mkStringSymbol('TestC0'), ctx.mkStringSort());

	//Assert to make capture correct all the time solver.assert(ctx.mkEq(TestRegex.captures[1], ctx.mkString('')));
	//solver.assert(ctx.mkEq(TestRegex.captures[1], ctx.mkString('a')));
	solver.assert(ctx.mkSeqInRe(symbolic, TestRegex.ast));
	solver.assert(ctx.mkImplies(ctx.mkSeqInRe(symbolic, TestRegex.ast), ctx.mkEq(symbolic, TestRegex.implier)));

	TestRegex.assertions.forEach(assert => {
		solver.assert(assert);
	});

	function Exists(array1, array2, pred) {

		for (let i = 0; i < array1.length; i++) {
			if (pred(array1[i], array2[i])) {
				return true;
			}
		}

		return false;
	}

	function DoesntMatch(l, r) {
		if (l === undefined) {
			return r !== '';
		} else {
			return l !== r;
		}
	}

	function CheckCorrect(model) {
		let real_match = Origin.exec(model.eval(symbolic).asConstant());
		let sym_match = TestRegex.captures.map(cap => model.eval(cap).asConstant());
		return real_match && !Exists(real_match, sym_match, DoesntMatch);
	}

	let NotMatch = Z3.Check(CheckCorrect, (query, model) => {
		console.log(model.eval(symbolic).asConstant());
		let query_list = query.exprs.concat([ctx.mkNot(ctx.mkEq(symbolic, ctx.mkString(model.eval(symbolic).asConstant())))]);
		return new Z3.Query(query_list, query.checks);
	});

	let CheckFixed = Z3.Check(CheckCorrect, (query, model) => {
		let real_match = Origin.exec(model.eval(symbolic).asConstant());

		if (!real_match) {
			return [];
		} else {

			real_match = Origin.exec(model.eval(symbolic).asConstant()).map(match => match || '');
			console.log(`Here ${real_match.length} in ${TestRegex.captures.length}`);

			TestRegex.captures.forEach((x, idx) => {
				console.log(`${x} => ${real_match[idx]}`);
			});

			let query_list = TestRegex.captures.map((cap, idx) => ctx.mkEq(ctx.mkString(real_match[idx]), cap));
			return [new Z3.Query(query.exprs.concat(query_list), [Z3.Check(CheckCorrect, (query, model) => [])])];
		}

	});

	let query = new Z3.Query([], [CheckFixed, NotMatch]);
	let mdl = query.getModel(solver);

	if (mdl) {
			let match = Origin.exec(mdl.eval(symbolic).asConstant());
			console.log('Modelled Match: ' + mdl.eval(symbolic).asConstant());
			
			if (match) {
				console.log(`Model: ${mdl.eval(symbolic).asConstant()} Captures: ${JSON.stringify(match)}`);
				console.log('Match Length: ' + match.length + ' CapturesLength: ' + TestRegex.captures.length);
				return CheckCorrect(mdl) ? 'GOOD' : 'BAD CAPTURE';
			} else {
				return 'BAD MATCH';
			}
	} else {
		return 'UNSAT';
	}
}

Test(/hello/);
Test(/\bGiggle/);
Test(/\bGiggle\b/);
Test(/^((?!chrome|android).)*safari/i);
Test(/^\bGiggle\b$/);

module.exports = Test;
