#!/bin/bash -e
entry="$(cat ./templates/pre_bindings.js)"
binding="$(cat ./compiled/z3_bindings_stripped.js)"
exit="$(cat ./templates/post_bindings.js)"

echo "$entry \n\n$binding \n\n$exit" > ./compiled/z3_bindings_built.js