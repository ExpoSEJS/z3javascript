/**
 * Copyright Blake Loring <blake_l@parsed.uk> 2015
 */

"use strict";

/**
 * A node.js API for Z3. Currently, all objects only increment ref counts but never decrement.
 * Ideally, a garbage collector would call a finalizer on the objects that then decrements the
 * ref counts.
 */

import Expr from './Expr';
import Model from './Model';
import Context from './Context.js';
import Solver from './Solver';
import Regex from './Regex';

let API = {};

API.Solver = Solver;
API.Context = Context;
API.Model = Model;
API.Expr = Expr;
API.Regex = Regex;

export default API;