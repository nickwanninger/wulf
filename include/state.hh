#ifndef __STATE_HH
#define __STATE_HH

#include <wulf.hh>
#include <vector>
#include <scanner.hh>
#include <scope.hh>
#include <vm.hh>




NSCLASS(scope, Scope);
NSCLASS(value, Object);

class State {
public:
	int repl_index = 0;
	bool repl = false;
	vm::Machine* machine = NULL;
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
	void eval(const char*);
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

