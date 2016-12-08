/**
 * Copyright Blake Loring <blake_l@parsed.uk> 2015
 * Approximate JavaScript regular expression to Z3 regex parser
 */

 function CullOuterRegex(regex) {
	let firstSlash = regex.indexOf('/');
	let lastSlash = regex.lastIndexOf('/');
	return regex.substr(firstSlash + 1, lastSlash - 1);
}

function RegexRecursive(ctx, regex, idx) {

	function more() {
		return idx < regex.length && current() != ')';
	}

	function mk(v) {
		return ctx.mkSeqToRe(ctx.mkString(v));
	}

	function current() {
		return regex[idx];
	}

	function next() {
		return regex[idx++];
	}

	function peek() {
		return regex[idx + 1];
	}

	function special(x) {
		return x == "*" || x == "+";
	}

	/**
	 * BNF:
	 * Atom1: char | '(' Atoms ')'
	 * Atom2: Atom1 ['*' | '+' | "|"]
	 * Atoms: [Atom [Atoms]]
	 */

	function ParseAtom1() {
		if (current() == '(') {
			next();
			let atoms = ParseAtoms();
			if (next() != ')') {
				console.log('Some error has occured parsing regex');
			}
			return atoms;
		} else {
			return mk(next());
		}
	}

	function ParseAtom2() {
		let atom = ParseAtom1();
		if (current() == '*') {
			next();
			return ctx.mkReStar(atom);
		} else if (current() == '+') {
			next();
			return ctx.mkRePlus(atom);
		} else if (current() == '|') {
			next();
			let atom2 = ParseAtom2();
			return ctx.mkReUnion(atom, atom2);
		} else {
			return atom;
		}
	}

	function ParseAtoms() {
	
		let rollup = mk('');

		while (more()) {
			rollup = ctx.mkReConcat(rollup, ParseAtom2());
		}

		return rollup.simplify();
	}

	return ParseAtoms();
}

function RegexOuter(ctx, regex) {
	return RegexRecursive(ctx, CullOuterRegex('' + regex), 0, false);
}

export default RegexOuter;