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
#include <color.hh>
#include <strings.h>
std::mutex scanner_lock;

/*
 * constructor for scanner
 */
Scanner::Scanner(FILE* source) {
	fp = source;
}

/*
 * constructor for a scanner that can
 * take a string instead
 */
Scanner::Scanner(char* source) {
	FILE* memf = fmemopen(source, strlen(source), "r");
	fp = memf;
}

/*
 * run method for scanner will rewind the file and scan the tokens from it
 * using flex under a mutex lock
 */
std::vector<Token> Scanner::run() {

	// aquire the lock for the scanner c bindings
	scanner_lock.lock();

	std::vector<Token> toks;
	// rewind the file pointer to the start (jic)
	rewind(fp);
	// set the global yyin file pointer
	yyin = fp;
	int ntok;

	// scan over all the tokens
	// adding them to the vector
	while ((ntok = yylex())) {
		Token tok(ntok, strdup(yytext));
		toks.push_back(tok);
	}

	// release the scanner
	scanner_lock.unlock();
	return toks;
}


// Token implementation section
Token::Token(int type, char* value) {

	if (type == TOK_STRING) {
		value = value + 1;
		value[strlen(value)-1] = '\0';
	}
	this->type = type;
	this->value = value;
}

Token::~Token() {
}

std::ostream& operator<<(std::ostream& os, const Token& m) {
	os << "(" << KBLU << m.type << RST << ", " << KYEL << "'" << m.value << "'" << RST << ")";
	return os;
}
