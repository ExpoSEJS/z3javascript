/**
 * Copyright Blake Loring <blake_l@parsed.uk> 2015
 * Approximate JavaScript regular expression to Z3 regex parser
 */

 function CullOuterRegex(regex) {
	let firstSlash = regex.indexOf('/');
	let lastSlash = regex.lastIndexOf('/');
	return regex.substr(firstSlash + 1, lastSlash - 1);
}

function RegularExpression(ctx, regex, start, culled) {
	let idx = start || 0;
	let c_part = '';
	let portions = [];

	function MkRe(str) {
		return ctx.mkSeqToRe(ctx.mkString(str));
	}

	while (idx < regex.length && regex[idx] != ')') {
		switch (regex[idx]) {
			case '\\':
				c_part += regex[idx + 1];
				idx += 2;
				break;
			case '*':
				portions.push(ctx.mkReStar(MkRe(c_part)));
				c_part = '';
				idx += 1;
				break;
			case '+':
				portions.push(ctx.mkRePlus(MkRe(c_part)));
				c_part = '';
				idx += 1;
				break;
			case '(':
				let recursionResult = RegularExpression(ctx, regex, idx + 1, true);
				portions.push(recursionResult[0]);
				idx = recursionResult[1] + 1;
				break;
			default:
				c_part += regex[idx];
				idx++;
				break;
		}
	}

	portions.push(MkRe(c_part));

	let concatenated = undefined;

	for (let i = 0; i < portions.length; i++) {
		console.log('Concat ' + portions[i])
		if (!concatenated) {
			concatenated = portions[i];
		} else {
			concatenated = ctx.mkReConcat(concatenated, portions[i]);
		}
	}

	return [concatenated, idx];
}

function RegexOuter(ctx, regex) {
	return RegularExpression(ctx, CullOuterRegex('' + regex), 0, false)[0];
}

export default RegexOuter;