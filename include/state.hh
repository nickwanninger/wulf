#ifndef __STATE_HH
#define __STATE_HH

#include <wulf.hh>
#include <vector>
#include <scanner.hh>
#include <scope.hh>


NSCLASS(scope, Scope)
NSCLASS(value, Value)

class State {
	bool repl = false;
	int repl_index = 0;
public:

	State();
	/*
	 * a state must have a root scope, which defaults
	 * to having no parent
	 */
	scope::Scope* scope = NULL;

	/*
	 * evalueate various source into the scope
	 */
	void eval(char*);
	void eval_file(char*);

	/*
	 * start the repl for the user to insert wulf code
	 */
	void run_repl();

	/*
	 * lex tokens out of a char*
	 */
	std::vector<Token> lex(char* source);
};

#endif

