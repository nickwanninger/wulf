/*
 * A compiler for the wulf language
 * Copyright (C) 2018  Nick Wanninger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#include <wulf.hh>
#include <math.h>

char* read_file_contents(char* filename) {
	FILE* f = fopen(filename, "r");
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




char* ccharcopy(const char* cc) {
	auto len = strlen(cc);
	char *buf = new char[len+1];
	for (int i = 0; i < len; i++) buf[i] = cc[i];
	buf[len] = 0;
	return buf;
}
