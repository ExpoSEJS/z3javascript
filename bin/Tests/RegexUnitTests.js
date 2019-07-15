/**
 * Copyright Blake Loring <blake_l@parsed.uk> 2015
 */
"use strict";

var _Z3Test = _interopRequireDefault(require("./Z3Test.js"));

var _Z = _interopRequireDefault(require("../Z3.js"));

function _interopRequireDefault(obj) { return obj && obj.__esModule ? obj : { "default": obj }; }

var errorCount = 0;

function Assert(regex, given, expectedCaptures, not) {
  console.log('Process' + regex);
  var before = _Z["default"].Query.TOTAL;
  var res = (0, _Z3Test["default"])(regex);

  if (res === 'GOOD') {
    console.log("Generated good model for ".concat(regex, " with ").concat(_Z["default"].Query.TOTAL - before, " queries"));
  } else {
    console.log("Error in ".concat(regex, " ").concat(res));
    process.exit(1);
    errorCount++;
  }
}

Assert(/[0-9]{3}/);
Assert(/[0-9]{undefined}/);
Assert(/xxx/);
Assert(/(xxx)/);
Assert(/(x)(y)(z)/);
Assert(/(x)((yz)(z))/); //Tests on klene*

Assert(/^(a)*$/, '', ['', '']);
Assert(/^(a)*$/);
Assert(/^(a?)*$/);
Assert(/^(a?)*$/);
Assert(/^(([a-z]+)a)*$/);
Assert(/^((([a-z]+)d)ef)*$/); //Tests on klene+

Assert(/^([a-z]+)$/);
Assert(/^([a-zA-Z]+)([a-z]{3})$/);
Assert(/^([a-zA-Z]+)(([a-z]){3})$/);
Assert(/^([a-zA-Z]+)+(([a-z]){3})$/); //Tests on strings with preceeding matches

Assert(/xyz/);
Assert(/a+/);
Assert(/[a-z]+[a-z]{5}/);
Assert(/^([a-zA-Z])+ef$/);
Assert(/^([a-zA-Z]){5}ef$/); //Loop tests on captures

Assert(/^([a-zA-Z]?){5}$/); //Real world ones

Assert(/^abc$|^$/);
Assert(/^((?:<|>)?=?)\s*([v=\s]*([0-9]+)\.([0-9]+)\.([0-9]+))$/);
Assert(/^((?:<|>)?=?)\s*([v=\s]*([0-9]+)\.([0-9]+)\.([0-9]+)(?:-?((?:[0-9]+|\d*[a-zA-Z-][a-zA-Z0-9-]*)(?:\.(?:[0-9]+|\d*[a-zA-Z-][a-zA-Z0-9-]*))*))?(?:\+([0-9A-Za-z-]+(?:\.[0-9A-Za-z-]+)*))?)$/);
console.log("Exit with ".concat(errorCount, " errors"));
process.exit(errorCount);