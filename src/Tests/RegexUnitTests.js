/**
 * Copyright Blake Loring <blake_l@parsed.uk> 2015
 */
"use strict";

import Test from './Z3Test.js';

let errorCount = 0;

function Assert(regex, given, expectedCaptures, not) {
	if (Test(regex) === 'GOOD') {
		console.log(`Generated good model for ${regex}`);
	} else {
		console.log(`Error in ${regex}`);
		errorCount++;
	}
}

Assert(/xxx/, 'xxx');
Assert(/(xxx)/, 'xxx', ['xxx']);

Assert(/(x)(y)(z)/, 'xyz', ['xyz', 'x', 'y', 'z']);
Assert(/(x)((yz)(z))/, 'xyzz', ['xyzz', 'x', 'yzz', 'yz', 'z']);

//Tests on klene*
Assert(/^(a)*$/, '', ['', '']);
Assert(/^(a)*$/, 'aaa', ['aaa', 'a']);
Assert(/^(a?)*$/, '', ['', '']);
Assert(/^(a?)*$/, 'aaa', ['aaa', 'a']);
Assert(/^(([a-z]+)a)*$/, 'abcadefa', ['abcadefa', 'defa', 'def']);
Assert(/^((([a-z]+)d)ef)*$/, 'aaadefbbbdef', ["aaadefbbbdef", "aaadefbbbdef", "aaadefbbbd", "aaadefbbb"]);

//Tests on klene+
Assert(/^([a-z]+)$/, 'aba', ['aba', 'aba']);
Assert(/^([a-zA-Z]+)([a-z]{3})$/, 'abaaa', ['abaaa', 'ab', 'aaa']);
Assert(/^([a-zA-Z]+)(([a-z]){3})$/, 'abaab', ['abaab', 'ab', 'aab', 'b']);

Assert(/^([a-zA-Z])+ef$/, 'def', ['def', 'd']);
Assert(/^([a-zA-Z]){5}ef$/, 'abzCDef', ['abzCDef', 'D']);

//Loop tests on captures
Assert(/^([a-zA-Z]?){5}$/, 'abc', ['abc', '']);
Assert(/^([a-zA-Z]?){5}$/, 'abcde', ['abcde', 'e']);
Assert(/^([a-zA-Z]?){5}$/, 'abcde', ['abcde', ''], true);

console.log(`Exit with ${errorCount} errors`);

process.exit(errorCount);
