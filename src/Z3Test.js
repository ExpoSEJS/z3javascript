"use strict";

import Z3 from './Z3';

console.log('Done import');

var ctx = new Z3.Context();
var solver = new Z3.Solver(ctx);

let rSort = ctx.mkRealSort();

let q75 = ctx.mkNumeral('75', rSort);
let q150 = ctx.mkNumeral('150', rSort);
let qrGy = ctx.mkRealVar('Y');
let qrGt = ctx.mkRealVar('X');

console.log('Here here');

ctx.mkSub(qrGt, q75);

console.log('Here');

let cA = ctx.mkNot(ctx.mkLe(q150, qrGt));
let cB = ctx.mkNot(ctx.mkLe(qrGt, q75));

console.log('Made stuff');

for (var i = 0; i < 1000; i++) {
	solver.push();
	solver.assert(cA);
	solver.assert(cB);

	console.log(solver.toString());

	let model = solver.getModel();

	console.log('Solvable: ' + !!model);

	if (model) {
		console.log('QRGTEV: ' + solver.getModel().eval(qrGt).asConstant());
		console.log('QRGT: ' + qrGt.toString());
		console.log('QRGT: ' + qrGt.asConstant());
	}

	model.destroy();
	solver.pop();
}