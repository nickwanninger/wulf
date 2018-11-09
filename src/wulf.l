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

%{
// wulf lexer
#include <wulf.hh>
#include <stdio.h>
%}


%%

\( {
	return TOK_LPAREN;
}

\) {
	return TOK_RPAREN;
}


:[_a-zA-Z\-\/\.:\*\?$][_a-zA-Z0-9\-\/\.:\*\?$]* {
	return TOK_KEYWORD;
}

[_a-zA-Z\/\.:\*\?$][_a-zA-Z0-9\-\/\.:\*\?$\+\-\*\/\.\-><=\!]* {
	return TOK_IDENTIFIER;
}

 /* special case some other identifier regex */
[\d\+\-\*\/\.\-><=]* {
	return TOK_IDENTIFIER;
}

-?([0-9]+([.][0-9]*)?|[.][0-9]+) {
	return TOK_NUMBER;
}


\"(\\.|[^"\\])*\" {
	return TOK_STRING;
}

' {
	return TOK_QUOTE;
}

[ \t\n] ; // ignore whitespace
";".*   ; // ignore comments

"#!".*   ; // ignore crunch bang

. {
	return TOK_UNKNOWN;
}

%%

int yywrap(void) {
	return 1;
}
