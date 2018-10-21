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
};

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
