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
            model = solver.getModel();
        }
        solver.pop();

        if (model) {
        	let CheckState = Check(model);
        	if (CheckState.isSAT) {
        		model.destroy();
        		alternatives = alternatives.concat(CheckState.alternatives);
        	} else {
        		return model;
        	}
        }
	}
}

export default Query;