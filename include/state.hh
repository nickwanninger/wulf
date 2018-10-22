#ifndef __STATE__
#define __STATE__

#include <vector>
#include <scanner.hh>

class State {
	int repl_index = 0;
public:
	void eval(char*);
	void eval_file(char*);
	void run_repl();
	char* repl_readline();
	std::vector<Token> lex(char* source);
};


#endif
