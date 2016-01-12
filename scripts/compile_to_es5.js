#!/bin/bash -e
node ./node_modules/babel-cli/bin/babel --presets es2015,stage-0 ./compiled/z3_bindings_em.js > ./compiled/z3_bindings_em_es5.js
node ./node_modules/babel-cli/bin/babel --presets es2015,stage-0 ./compiled/z3_bindings_ref.js > ./compiled/z3_bindings_ref_es5.js