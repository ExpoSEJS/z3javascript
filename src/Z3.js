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

class API {}

API.Solver = Solver;
API.Context = Context;
API.Model = Model;
API.Expr = Expr;

export default API;