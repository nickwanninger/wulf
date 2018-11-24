/*
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

#ifndef __WULF_HH
#define __WULF_HH



#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <mutex>
#include <vector>
#include <strings.h>
#include <sstream>
#include <unistd.h>
#include <string>

#define NSCLASS(ns, name) namespace ns { class name; }

//#include <value.hh>
//#include <parser.hh>
//#include <state.hh>
//#include <scanner.hh>

extern std::mutex scanner_lock;
extern int yylex();
extern int yylineno;
extern char* yytext;
extern FILE* yyin;

char* read_file_contents(char*);
char* read_file_contents(FILE* f);
std::string unescape(const std::string& s);

char* ccharcopy(const char*);


#define TOK_EOF           0
#define TOK_UNKNOWN       1
#define TOK_FLOAT         2
#define TOK_INT           3
#define TOK_IDENTIFIER    4
#define TOK_LPAREN        5
#define TOK_RPAREN        6
#define TOK_OPERATOR      7
#define TOK_KEYWORD       8
#define TOK_STRING        9
#define TOK_QUOTE         10
#define TOK_QUASIQUOTE    11
#define TOK_UNQUOTE       12
#define TOK_UNQUOTE_SPLICING 13
#define TOK_WHITESPACE    14
#endif
