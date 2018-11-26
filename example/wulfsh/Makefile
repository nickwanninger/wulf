src/bindings.so: bindings.cc
	@clang++ -shared -undefined dynamic_lookup -o src/bindings.so bindings.cc

default: src/bindings.so

clean:
	rm -rf src/bindings.so
