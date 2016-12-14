/**
 * Copyright Blake Loring <blake_l@parsed.uk> 2015
 */
"use strict";

export default {
	wrap: function(ctx, ret) {
		return ret;
	},
	astArray: function(args) {
		return args.map(a => a.ast);
	},
	tagStr: function(args) {
		return args.reduce((a, b) => { if (b.getTag()) { a = a + b.getTag(); } return a; }, '');
	}
};