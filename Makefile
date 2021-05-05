# Just run make to build everything, including tests


.PHONY: all tests clean

.SECONDARY: bytecode/%.bc

CXXFLAGS=$(shell llvm-config --cxxflags) -g -O3
CXX= clang++


all: shared/pointer.so tests andersen_tests


test_pointer: tests shared/pointer.so | output
	opt -load ./shared/pointer.so -S -pointer ./bytecode/hash.bc -o output/out

shared/pointer.so: build/pointer.o | shared
	$(CXX) -O3 -rdynamic  -dylib  -shared $^ -o $@

.SECONDEXPANSION:
build/%.o: $$(shell find . -name $$*.cpp -o -name $$*.hpp) | build
	$(CXX) -fPIC -c $(CXXFLAGS) $(shell find . -name "$*.cpp") -o $@

.SECONDEXPANSION:
tests/%.bc: $$(shell find . -name $$*.c -o -name $$*.cpp)
	clang++ -Xclang -disable-O0-optnone -fno-discard-value-names -O0 -emit-llvm -c $^  \
	-o $@
.SECONDEXPANSION:
andersen_tests/%.bc: $$(shell find . -name $$*.c -o -name $$*.cpp)
	clang++ -Xclang -disable-O0-optnone -fno-discard-value-names -O0 -emit-llvm -c $^  \
	-o $@
	
	
#; opt -mem2reg $@ -o $@; opt -loop-simplify $@ -o $@


andersen_tests: bytecode/store.bc bytecode/load.bc bytecode/call.bc bytecode/andersen_retval.bc \
				bytecode/gep.bc bytecode/phi.bc
tests: bytecode/constfold.bc bytecode/hash.bc

bytecode/%.bc: tests/%.bc | bytecode
	cp $^ ./bytecode


build:
	mkdir build
shared:
	mkdir shared
bytecode:
	mkdir bytecode
output:
	mkdir output

clean:
	rm -f output/* bytecode/* build/* shared/* tests/*.bc
