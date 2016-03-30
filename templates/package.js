export default function(mode, path) {
	if (mode === 'native') {
		throw 'Unfinished';
	}

	return require('./z3_bindings_em_es5.js').default;
};