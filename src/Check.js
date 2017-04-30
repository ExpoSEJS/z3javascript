export default function(check_predicate, altgen) {
	return function(model) {

		//Returns the Query check structure with either a list of alternative queries or nothing
		let failed = check_predicate(model);

		console.log("isSAT ? " + failed);

		return {
			isSAT: failed,
			alternatives: failed ? altgen(model) : []
		};
	}
}