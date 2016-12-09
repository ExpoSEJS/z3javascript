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

	function Any() {
		return ctx.mkReRange(ctx.mkString('\x00'), ctx.mkString('\xff'));
	}

	/**
	 * BNF:
	 * RangeInner: char-char [RangeInner]
	 * Range: [RangeInner]
	 * Atom1: char | '(' Atoms ')'
	 * Atom2: Atom1 ['*' | '+' | "|"]
	 * Atoms: [Atom [Atoms]]
	 */

	function ParseRangeInner() {

		let range = mk('');

		while (more() && current() != ']') {
			let c1 = ctx.mkString(next());

			if (current() == '-') {
				next();
				let c2 = ctx.mkString(next());
				range = ctx.mkReUnion(range, ctx.mkReRange(c1, c2));
			} else {
				range = ctx.mkReUnion(range, ctx.mkSeqToRe(c1));
			}
		}

		return range;
	}

	function ParseRange() {
		next();
		let r = ParseRangeInner();
		if (next() == ']') {
			return r;
		} else {
			return null;
		}
	}

	let Specials = {
		'.': Any
	}

	function ParseAtom1() {
		if (current() == '(') {
			next();
			let atoms = ParseAtoms();
			if (next() != ')') {
				console.log('Some error has occured parsing regex');
			}
			return atoms;
		} else if (current() == '[') {
			return ParseRange();
		} else {
			if (Specials[current()]) {
				return Specials[current()]();
			} else {
				return mk(next());
			}
		}
	}

	function ParseNumber() {
		
		function digit() {
			return current() >= '0' && current() <= '9';
		}

		let numStr = '';

		if (!digit()) {
			console.log(current());
			return null;
		}

		while (digit()) {
			numStr += next();
		}

		return parseInt(numStr);
	}

	function ParseLoopCount() {
		let n1 = ParseNumber();

		if (n1 === null) {
			console.log('Faile n1')
			return [null, null];
		}

		if (current() == ',') {
			next();
			let n2 = ParseNumber();
			return [n1, n2];
		} else {
			return [n1, n1];
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
		} else if (current() == '?') {
			next();
			return ctx.mkReOption(atom);
		} else if (current() == '|') {
			next();
			let atom2 = ParseAtom2();
			return ctx.mkReUnion(atom, atom2);
		} else if (current() == '{') {
			next();
			let [lo, hi] = ParseLoopCount();

			if (lo === null || hi === null) {
				return null;
			}
			
			if (!next() == '}') {
				return null;
			}

			return ctx.mkReLoop(atom, lo, hi);
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