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
#include <string.h>
#include <stdio.h>

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


std::vector<value::obj> Parser::parse_top_level() {

	std::vector<value::obj> nodes;
	reset();
	while (tok.type != TOK_EOF) {
		value::obj node = parse_expr();
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

value::obj Parser::parse_list() {
	value::obj obj = value::newobj(value::list);

	requires(TOK_LPAREN);

	std::vector<value::obj> items;
	// step along to the next token
	next();
	while (tok.type != TOK_RPAREN && tok.type) {
		if (tok.type == TOK_EOF) {
			throw "unexpected EOF";
		}
		value::obj node = parse_expr();
		items.push_back(node);
	}



	value::obj list = value::newobj(value::list);


	unsigned len = items.size();
	unsigned last_i = len - 1;
	value::obj curr = obj;
	for (unsigned i = 0; i < len; i++) {
		value::obj lst = value::newobj(value::list);
		curr->car = items[i];

		if (i+1 <= last_i && items[i+1]->type == value::ident && strcmp(items[i+1]->string,".") == 0) {
			if (i+1 != last_i-1) throw "illegal end of dotted list";
			curr->cdr = items[last_i];
			if (curr->cdr->type == value::nil) {
				curr->cdr->type = value::list;
				curr->cdr->car = NULL;
			}
			break;
		}

		curr->cdr = lst;
		curr = lst;
	}

	if (tok.type != TOK_RPAREN) {
		throw "missing closing paren!";
	}
	// step forward after the last right paren in the list
	next();
	return obj;
}


value::obj Parser::parse_quote_variant(const char* variant) {
	value::obj obj = value::newobj();
	obj->type = value::list;
	obj->car = value::newobj(variant);
	obj->car->type = value::ident;
	next();
	obj->cdr = value::newobj();
	obj->cdr->type = value::list;
	obj->cdr->car = parse_expr();
	obj->cdr->cdr = value::newobj(value::list);
	return obj;
}

value::obj Parser::parse_ident() {
	requires(TOK_IDENTIFIER);
	auto obj = value::newobj(tok.value);
	obj->type = value::ident;
	if (!strcmp(obj->string, "nil")) {
		obj->type = value::nil;
	}
	next();
	return obj;
}

value::obj Parser::parse_number() {
	requires(TOK_NUMBER);
	auto obj = value::newobj();
	obj->type = value::number;
	obj->number = std::atof(tok.value);
	next();
	return obj;
}


value::obj Parser::parse_expr() {
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
			return parse_quote_variant("quote");
		case TOK_QUASIQUOTE:
			return parse_quote_variant("quasiquote");
		case TOK_UNQUOTE:
			return parse_quote_variant("unquote");
		case TOK_UNQUOTE_SPLICING:
			return parse_quote_variant("unquote-splicing");

		case TOK_KEYWORD: {
				auto kw = value::newobj();
				kw->type = value::keyword;
				kw->string = tok.value;
				next();
				return kw;
			}
		case TOK_STRING: {
				auto s = value::newobj(tok.value);
				s->type = value::string;
				s->string = tok.value;
				next();
				return s;
			}
	}
	std::cout << "unexpected token " << tok.type << "'" << tok.value << "'\n";
	throw "oops";
	next();
	return value::newobj();
}

