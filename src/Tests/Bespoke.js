"use strict";

import Test from './Z3Test.js';
import Z3 from '../Z3.js';

Test(/^((?:<|>)?=?)\s*([v=\s]*([0-9]+)\.([0-9]+)\.([0-9]+))$/);

console.log(Z3.Query.TOTAL);