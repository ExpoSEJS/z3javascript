/**
 * Copyright Blake Loring <blake_l@parsed.uk>
 */
class Query {
	constructor(exprs, checks) {
		this.exprs = exprs;
		this.checks = checks;
	}

	getModel(solver, incremental) {
		return Query.process(solver, [this], incremental);
	}
}

Query.MAX_REFINEMENTS = -1;
Query.TOTAL = 0;

Query.canAttempt = function(currentAttempts) {
    return Query.MAX_REFINEMENTS == -1 || (currentAttempts < Query.MAX_REFINEMENTS);
}

Query.process = function(solver, alternatives, incremental) {
    let attempts = 0;

	while (Query.canAttempt(attempts) && alternatives.length) {
        
        attempts++;
        Query.TOTAL++;

		let next = alternatives.shift();

		let model;

        if (incremental) {
		    solver.push();
        } else {
            solver.reset();
        }

        next.exprs.forEach(clause => solver.assert(clause));
        model = solver.getModel();

        if (incremental) {
            solver.pop();
        }

        if (model) {
            //Run all the checks and concat any alternatives
            let Checks = next.checks.map(check => check(next, model));
            alternatives = Checks.reduce((alt, next) => alt.concat(next.alternatives), alternatives);

            //Find any failing check
        	let Failed = Checks.find(check => !check.isSAT);
        	
        	//If we have found a satisfying model return it otherwise add alternatives from check
        	if (Failed) {
        		model.destroy();
        	} else {
                return model;
            }
        } //Else unsat
	}

    return null;
}

export default Query;
