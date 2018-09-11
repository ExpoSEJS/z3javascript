[![Build Status](https://travis-ci.org/ExpoSEJS/z3javascript.svg?branch=master)](https://travis-ci.org/ExpoSEJS/z3javascript)

## Z3Javascript

A project to try and get Z3 running as a Javascript library. The project exercises a custom Z3 build through the library `ref`.

NOTE: `Z3_PATH` environment variable should point to the Z3 .so .dll or .dylib. This dll or dylib file will be automatically created and moved to bin when the library is `npm install`'d

## Usage

Used as an npm dependency.

`npm install git+ssh://github.com/ExpoSEJS/z3javascript.git`

Z3 will be automatically downloaded and built. We require clang to build Z3,

From then, simply point the `Z3_PATH` environment variable to the generated dll, so, etc using something like `Z3_PATH=./node_modules/z3javascript/bin/libz3.dylib node myapplication.js` to use your program. View tests for an example.

## Dependencies

build-essential, clang (5.0 onward) and Node.js and NPM (Use the latest LTS)
