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

