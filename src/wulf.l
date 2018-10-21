%{
// wulf lexer
#include <wulf.hh>
%}


%%

\(                        return TOK_LPAREN;
\)                        return TOK_RPAREN;
[a-zA-Z][_a-zA-Z0-9]*     return TOK_IDENTIFIER;
[1-9][0-9]*               return TOK_INTEGER;
[\d\+\-\*\/\.\-><]*       return TOK_OPERATOR;
[ \t\n]                   ; // ignore whitespace
.                         return TOK_UNKNOWN;

%%


int yywrap(void) {
	return 1;
}
