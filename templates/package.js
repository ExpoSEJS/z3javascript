export default function(mode) {
	if (mode === 'native') {
		throw 'Unfinished';
	}

	import Z3 from './z3_bindings_em_es5.js';
	return Z3;
};