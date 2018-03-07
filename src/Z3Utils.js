/**
 * Copyright Blake Loring <blake_l@parsed.uk> 2015
 */
"use strict";

/**
 * Recursively reduce Expr to AST
 */
function astArray(args) {
	return args.map(a => a instanceof Array ? astArray(a) : (a.ast ? a.ast : a));
}

export default {
	wrap: function(ctx, ret) {
		return ret;
	},
	astArray: astArray
};