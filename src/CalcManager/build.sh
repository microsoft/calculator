mkdir -p bin/wasm
echo Generating WASM File
emcc \
	-std=c++17 \
	-s WASM=1 \
	-s LEGALIZE_JS_FFI=0 \
	-s RESERVED_FUNCTION_POINTERS=64 \
    -s ALLOW_MEMORY_GROWTH=1 \
	-s BINARYEN=1 \
	-s SIDE_MODULE=1 \
	-s DISABLE_EXCEPTION_CATCHING=0 \
	-o bin/wasm/CalcManager.wasm \
	-s EXPORT_ALL=1 \
	CEngine/*.cpp Ratpack/*.cpp *.cpp -I.

echo Generating LLVM Bitcode files
emcc \
	-std=c++17 \
	-s WASM=1 \
	-s LEGALIZE_JS_FFI=0 \
	-s RESERVED_FUNCTION_POINTERS=64 \
    -s ALLOW_MEMORY_GROWTH=1 \
	-s BINARYEN=1 \
	-s DISABLE_EXCEPTION_CATCHING=0 \
	-o bin/wasm/CalcManager.bc \
	CEngine/*.cpp Ratpack/*.cpp *.cpp -I.
