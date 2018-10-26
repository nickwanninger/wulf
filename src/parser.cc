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
#include <scanner.hh>
#include <parser.hh>


Parser::Parser(std::vector<Token> toks) {
	tokens = toks;
	tok = toks[0];
}


Token Parser::peek(int o) {
	int target = tok_index + o;
	if (target < 0 || target > tokens.size()-1) {
		Token t;
		t.type = TOK_EOF;
		t.value = strdup("EOF");
		return t;
	}
	return tokens[target];
}

Token Parser::move(int o) {
	tok_index += o;
	tok = peek(0);
	return tok;
}

Token Parser::next() {
	return move(1);
}

Token Parser::back() {
	return move(-1);
}

Token Parser::reset() {
	tok_index = 0;
	tok = peek(0);
	return tok;
}


std::vector<value::Value*> Parser::parse_top_level() {
	std::vector<value::Value*> nodes;
	reset();
	while (tok.type != TOK_EOF) {
		value::Value *node = parse_expr();
		if (node == NULL) {
			break;
		}
		nodes.push_back(node);
	}

	return nodes;
}


bool Parser::requires(int type) {
	if (tok.type != type) {
		throw "invalid token type found";
	}
	return true;
}

value::List* Parser::parse_list() {
	value::List *list = new value::List();
	requires(TOK_LPAREN);
	// step along to the next token
	next();
	while (tok.type != TOK_RPAREN && tok.type) {
		if (tok.type == TOK_EOF) {
			throw "unexpected EOF";
		}
		value::Value *node = parse_expr();
		if (node == NULL) {
			std::cout << "null thing\n";
			break;
		}
		list->push(node);
	}

	if (tok.type != TOK_RPAREN) {
		throw "missing closing paren!";
	}
	// step forward after the last right paren in the list
	next();
	return list;
}


value::List* Parser::parse_quote() {
	requires(TOK_QUOTE);
	value::List *list = new value::List();
	list->push(new value::Ident("quote"));
	next();
	list->push(parse_expr());
	return list;
}

value::Ident* Parser::parse_ident() {
	requires(TOK_IDENTIFIER);
	value::Ident *ident = new value::Ident(tok.value);
	next();
	return ident;
}

value::Number* Parser::parse_number() {
	requires(TOK_NUMBER);
	value::Number *num = new value::Number(tok.value);
	next();
	return num;
}


value::Value* Parser::parse_expr() {
	switch (tok.type) {
		case TOK_EOF:
			throw "unexpected EOF";
		case TOK_RPAREN:
			throw "unexpected right paren";
		case TOK_LPAREN:
			return parse_list();
		case TOK_NUMBER:
			return parse_number();
		case TOK_IDENTIFIER:
			return parse_ident();
		case TOK_QUOTE:
			return parse_quote();
		case TOK_UNKNOWN:
		case TOK_SYMBOL:
			std::cout << "warning: token " << tok << " unimplemented. Converted to nil\n";
		case TOK_STRING:
			auto s = new value::String(tok.value);
			next();
			return s;
	}
	next();
	return new value::Ident("nil");
}








