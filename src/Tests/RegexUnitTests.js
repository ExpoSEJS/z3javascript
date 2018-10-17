/**
 * Copyright Blake Loring <blake_l@parsed.uk> 2015
 */
"use strict";

import Test from './Z3Test.js';
import Z3 from '../Z3.js';

let errorCount = 0;

function Assert(regex, given, expectedCaptures, not) {
    console.log('Process' + regex);
	let before = Z3.Query.TOTAL;
	let res = Test(regex);
	if (res === 'GOOD') {
		console.log(`Generated good model for ${regex} with ${Z3.Query.TOTAL - before} queries`);
	} else {
		console.log(`Error in ${regex} ${res}`);
		process.exit(1);
		errorCount++;
	}
}

Assert(/[0-9]{3}/);
Assert(/[0-9]{undefined}/);

Assert(/xxx/);
Assert(/(xxx)/);

Assert(/(x)(y)(z)/);
Assert(/(x)((yz)(z))/);

//Tests on klene*
Assert(/^(a)*$/, '', ['', '']);
Assert(/^(a)*$/);
Assert(/^(a?)*$/);
Assert(/^(a?)*$/);
Assert(/^(([a-z]+)a)*$/);
Assert(/^((([a-z]+)d)ef)*$/);

//Tests on klene+
Assert(/^([a-z]+)$/);
Assert(/^([a-zA-Z]+)([a-z]{3})$/);
Assert(/^([a-zA-Z]+)(([a-z]){3})$/);
Assert(/^([a-zA-Z]+)+(([a-z]){3})$/);

//Tests on strings with preceeding matches
Assert(/xyz/);
Assert(/a+/);
Assert(/[a-z]+[a-z]{5}/);

Assert(/^([a-zA-Z])+ef$/);
Assert(/^([a-zA-Z]){5}ef$/);

//Loop tests on captures
Assert(/^([a-zA-Z]?){5}$/);

//Real world ones
Assert(/^abc$|^$/);
Assert(/^((?:<|>)?=?)\s*([v=\s]*([0-9]+)\.([0-9]+)\.([0-9]+))$/);
Assert(/^((?:<|>)?=?)\s*([v=\s]*([0-9]+)\.([0-9]+)\.([0-9]+)(?:-?((?:[0-9]+|\d*[a-zA-Z-][a-zA-Z0-9-]*)(?:\.(?:[0-9]+|\d*[a-zA-Z-][a-zA-Z0-9-]*))*))?(?:\+([0-9A-Za-z-]+(?:\.[0-9A-Za-z-]+)*))?)$/);


console.log(`Exit with ${errorCount} errors`);

process.exit(errorCount);
