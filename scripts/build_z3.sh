./emsdk_portable/emsdk activate latest
./emsdk/emsdk_env.sh

cd z3
if [ -e build ]; then
    mv build build-$(stat -c "%y" build | sed 's/[ :]/_/g' - | sed 's/\..*/ /g')
fi

alias c++=em++
alias g++=em++
alias ar=emar
alias cc=emcc
alias gcc=emcc
alias cmake=emcmake
alias configure=emconfigure
alias ranlib=emranlib
export CC=emcc
export CXX=em++
python scripts/mk_make.py --x86 --githash=$(git rev-parse HEAD) --staticlib
cd build
sed -i 's/EXE_EXT=/EXE_EXT=.emscripten.js/g' config.mk
sed -i 's/^\(LINK_EXTRA_FLAGS=.*\)/\1 -O3/g' config.mk
emmake make
cp z3.emscripten.js* ../../compiled/
