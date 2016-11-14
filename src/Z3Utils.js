export default {
	wrap: function(ctx, ret) {
		return ret;
	},
	astArray: function(args) {
		return args.map(a => a.ast);
	}
};