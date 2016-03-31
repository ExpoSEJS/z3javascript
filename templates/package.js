export default function(mode, path) {

	if (mode === 'native') {
		return {
			default: require('./z3_bindings_ref_es5.js').default(path)
		};
	}

	return require('./z3_bindings_em_es5.js').default;
};