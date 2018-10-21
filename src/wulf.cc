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

#define EXIT_FILE_ERROR 1


int main(int argc, char** argv) {
	int ntoken, vtoken;


	if (argc <= 1) {
		std::cout << "Usage: wulfc <file>\n";
		exit(1);
	}

	char* filepath = argv[1];


	FILE* source = fopen(filepath, "r");
	if (source == NULL) {
		std::cout << "File \"" << filepath << "\" could not be found!\n";
		exit(EXIT_FILE_ERROR);
	}


	Scanner *scanner = new Scanner(source);
	std::vector<Token> *toks = scanner->run();
	for (Token& tok : *toks) {
		std::cout << tok << "\n";
	}
	delete toks;
	return 0;
}
