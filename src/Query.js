/**
 * Copyright Blake Loring <blake_l@parsed.uk>
 */
class Query {
	constructor(exprs, checks) {
		this.exprs = exprs;
		this.checks = checks;
	}

	getModel(solver) {
		return Query.process(solver, [this]);
	}
}

Query.TOTAL = 0;

Query.process = function(solver, alternatives) {
	while (alternatives.length) {
        Query.TOTAL++;
		let next = alternatives.shift();

		let model;

		solver.push();
        {
        	next.exprs.forEach(clause => solver.assert(clause));
            //console.log(`${solver.toString()}`);
            model = solver.getModel();
        }
        solver.pop();

        if (model) {

            //console.log(`${model.toString()}`);

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
}

export default Query;
