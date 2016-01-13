#Z3 JS

A project to try and get Z3 running as a Javascript library through the use of emscripten

#Dependancies
nodejs, npm, cmake, build-essential, core utils (Lots of bash scripts), git

On Ubuntu gcc, g++ multilib may also be needed (gcc-4.8-multilib, g++-4.8-multilib)

#Setup
To initially setup run ./setup

#Rebuilding
To recompile the emscripten binding run ./scripts/build_z3.sh
To recompile the native library run ./scripts/build_z3_std.sh