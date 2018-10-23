#include <wulf.hh>


char* read_file_contents(char* filename) {
	FILE* f = fopen(filename, "r");
	return read_file_contents(f);
}


char* read_file_contents(FILE* f) {
	if (f == NULL) {
		throw "file not found";
	}
	// Determine file size
	fseek(f, 0, SEEK_END);
	size_t size = ftell(f);
	// allocate a buffer
	char* buf = new char[size];
	// read the file into the buffer
	rewind(f);
	fread(buf, sizeof(char), size, f);
	return buf;
}
