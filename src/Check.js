export default function(check_predicate, altgen) {
	return function(query, model) {

		//Returns the Query check structure with either a list of alternative queries or nothing
		let sat = check_predicate(model);

		return {
			isSAT: sat,
			alternatives: !sat ? altgen(query, model) : []
		};
	}
}