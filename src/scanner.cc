#include <wulf.hh>

std::mutex scanner_lock;

/*
 * constructor for scanner
 */
Scanner::Scanner(FILE* source) {
	fp = source;
}

/*
 * run method for scanner will rewind the file and scan the tokens from it
 * using flex under a mutex lock
 */
std::vector<Token> *Scanner::run() {

	std::vector<Token> *toks = new std::vector<Token>();
	// aquire the lock for the scanner c bindings
	scanner_lock.lock();

	rewind(fp);


	yyin = fp;


	int ntok;


	// scan over all the tokens
	while ((ntok = yylex())) {
		Token tok(ntok, strdup(yytext));
		std::cout << yytext << "\n";
		toks->push_back(tok);
	}

	// release the scanner
	scanner_lock.unlock();
	return toks;
}



Token::Token(int type, char* value) {
	this->type = type;
	this->value = value;
}
