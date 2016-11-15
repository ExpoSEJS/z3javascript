#Z3Javascript
A project to try and get Z3 running as a Javascript library. The project functions either in DLL mode, opening up as a `ref`, or as a `emscripten` binding.
The project also includes a convenience wrapper to wrap ASTs.

DLL support is fairly stable - though Expr memory management has to be explicit and no good solution has been found.

NOTE: `Z3_PATH` environment variable should point to the Z3 .so .dll or .dylib

Emscripten support is new, buggy and not recommended. if `Z3_WEB` env variable is set Z3 will build in emscripten and the wrapper will automatically use the emscripten bindings but support is new. To use the emscripten build you will need the mem created by `Z3_WEB=true` npm install in the current working directory of the application running Z3.

#Usage
Used as an npm dependency.

`npm install git+ssh://github.com/jawline/z3javascript.git`

Z3 will be automatically downloaded and built using either emscripten or clang. Uses a forked version of Z3 to maintain compatability with wrapper and build bindings automatically.

From then, simply point the `Z3_PATH` environment variable to the generated dll, so, etc using something like `Z3_PATH=./node_modules/z3javascript/bin/libz3.dylib node myapplication.js`

#Dependencies
Node JS (We test on version 7), python, build-essential,  git, clang++ or g++ will work as a cpp compiler for Z3.
On Ubuntu gcc, g++ multilib may also be needed (gcc-4.8-multilib, g++-4.8-multilib).

#Setup
To initially setup run `npm install`

#Rebuilding
`./setup` will automatically pull and build Z3.
To specifically recompile the emscripten binding run `./scripts/build_z3.sh`.
To specifically recompile the native library run `./scripts/build_z3_std.sh`.