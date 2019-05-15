emcc \
	-std=c++17 \
	-s WASM=1 \
	-s LEGALIZE_JS_FFI=0 \
	-s RESERVED_FUNCTION_POINTERS=64 \
    -s ALLOW_MEMORY_GROWTH=1 \
	-s BINARYEN=1 \
	-s SIDE_MODULE=1 \
	-o CalcManager.wasm \
	-s EXPORT_ALL=1 \
	CEngine/*.cpp RatPack/*.cpp *.cpp -I.
