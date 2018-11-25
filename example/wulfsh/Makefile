bindings.so: bindings.cc
	@clang++ -shared -undefined dynamic_lookup -o bindings.so bindings.cc

default: bindings.so

clean:
	rm -rf bindings.so
