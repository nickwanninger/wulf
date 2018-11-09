#include <wulf.hh>
#include <math.h>

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


std::string unescape(const std::string& s) {
	std::string res;
	std::string::const_iterator it = s.begin();
	while (it != s.end()) {
		char c = *it++;
		if (c == '\\' && it != s.end()) {
			switch (*it++) {
				case '\\': c = '\\'; break;
				case 'n': c = '\n'; break;
				case 't': c = '\t'; break;
				case 'r': c = '\r'; break;
				case '"': c = '"';  break;
				case 'f': c = '\f'; break;
				case '\'': c = '\''; break;
					// all other escapes
				default:
					c = '?';
					break;
			}
		}
		res += c;
	}

	return res;
}


