#ifndef __SCANNER__
#define __SCANNER__


#include <iostream>
#include <vector>

/*
 * the token class represents a single token emitted by the tokenizer
 */
class Token {
public:
	int type;
	char* value;
	Token();
	Token(int, const char*);
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
	Scanner(char*);
	std::vector<Token> run();
};

#endif
