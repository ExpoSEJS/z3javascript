class Query {
	constructor(exprs, check) {
		this.exprs = exprs;
		this.check = check;
	}

	getModel(solver) {
		return Query.process(solver, [this]);
	}
}

Query.process = function(solver, alternatives) {
	while (alternatives.length) {
		let next = alternatives.shift();

		let model;

		solver.push();
        {
        	next.exprs.forEach(clause => solver.assert(clause));
        	console.log('Solver ' + solver.toString());
            model = solver.getModel();
        }
        solver.pop();

        if (model) {
        	let CheckState = next.check(model);
        	
        	//If we have found a satisfying model return it
        	if (CheckState.isSAT) {
        		return model;
        	}

        	model.destroy();
        	alternatives = alternatives.concat(CheckState.alternatives);
        }
	}
}

export default Query;