/**
 * Copyright Blake Loring <blake_l@parsed.uk> 2015
 */
"use strict";

import Z3 from './Z3';

var ctx = new Z3.Context();
var solver = new Z3.Solver(ctx);

console.log('Compiling RegEx');

let regExToTest = [/((s|\-|\ |\.)+(atr%C3%A1s|em)*(s|\-|\ |\.)*(\d+|zero|e meio|um|dois|tr%C3%AAs|poucos|alguns|quatro|cinco|seis|sete|oito|nove|dez|onze|doze|treze|catorze|quinze|dezesseis|dezessete|dezoito|dezenove|vinte|trinta|quarenta|cinquenta|sessenta|setenta|oitenta|noventa|cem|mil|milh%C3%A3o)+(s|\-|\ |\.)*((milissegundo|segundo|minuto|hora|dia|semana|m%C3%AAs|trimestre|ano|d%C3%A9cada)(s|\-|\ |\.)*|s|\-|\ |\.)*(atr%C3%A1s|em)*(s|\-|\ |\.)+)/];
let testRegexs = regExToTest.map(r => Z3.Regex(ctx, r));

let symbol = ctx.mkStringSymbol('HI');
let symbolic = ctx.mkConst(symbol, ctx.mkStringSort());

console.log(testRegexs[0].captures[0].toString());

testRegexs.forEach(regex => {
	solver.assert(ctx.mkSeqInRe(symbolic, regex.ast).simplify());
	/**solver.assert(ctx.mkImplies(ctx.mkSeqInRe(symbolic, regex.ast), ctx.mkEq(symbolic, regex.implier)));
	regex.assertions.forEach(assert => {
		solver.assert(assert.simplify());
	});*/
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