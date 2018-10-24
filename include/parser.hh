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


#ifndef __PARSER_HH
#define __PARSER_HH


#include <scanner.hh>
#include <value.hh>

class Parser {
	private:
		std::vector<Token> tokens;
		Token tok;
		int tok_index;
	public:
		/*
		 * constructors for a Parser
		 */
		Parser(std::vector<Token>);

		/*
		 * peek and move are helper functions used by next and back
		 */
		Token peek(int);
		Token move(int);

		/*
		 * next and back tokens mutate the state of the class,
		 * and return the new token.
		 * next() will return the next token
		 * back() will return the previous token
		 */
		Token next();
		Token back();
		Token reset();

		/*
		 * requires throws an error if the expected token type
		 * wasn't found on the current token
		 */
		bool requires(int);

		/*
		 * parse the top level nodes of a set of tokens
		 */
		std::vector<value::Value*> parse_top_level();
		value::List* parse_list();
		value::List* parse_quote();
		value::Ident* parse_ident();
		value::Value* parse_expr();
		value::Number* parse_number();
};


#endif
