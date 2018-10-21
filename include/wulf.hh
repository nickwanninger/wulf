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

#define _GNU_SOURCE
#ifndef __WULF__
#define __WULF__

#include <iostream>
#include <stdio.h>
#include <fstream>
#include <mutex>
#include <vector>
#include <strings.h>



// -- scanner nonsense --
extern std::mutex scanner_lock;
extern int yylex();
extern int yylineno;
extern char* yytext;
extern FILE* yyin;


/*
 * the token class represents a single token emitted by the tokenizer
 */
class Token {
public:
	int type;
	char* value;
	Token(int, char*);
	~Token();
};

// declare the "shift into stream" operator for a token
std::ostream& operator<<(std::ostream& os, const Token& m);

/*
 * the scanner class implements wrappers around the flex scanner interface
 */
class Scanner {
private:
	FILE* fp = NULL;
public:
	Scanner(FILE*);
	std::vector<Token> *run();
};

#define TOK_UNKNOWN     1
#define TOK_INTEGER     2
#define TOK_IDENTIFIER  3
#define TOK_LPAREN      4
#define TOK_RPAREN      5
#define TOK_OPERATOR    6
// ~~ scanner nonsense ~~

#endif
