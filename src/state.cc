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
#include <wulf.hh>
#include <state.hh>
#include <value.hh>
#include <color.hh>
#include <fstream>
#include <parser.hh>
#include <linenoise.h>

#define BIT(num, n) (((num) >> (n)) & 1)
#define BITAD(b) ((b) == 0 ? 'a' : 'd')

State::State() {
	scope = new scope::Scope();
	scope->install_default_bindings();
	machine = new vm::Machine();
	machine->state = this;
	// bootstrap the load procedure
	eval("(def (load path) (syscall 8 path))");
	// define some basic functions
	eval("(load \"/usr/local/lib/wulf/runtime.wl\")");


	// this bit of monsterous code generates all the car and cdr combinations
	// anyone would ever reasonably need
	std::ostringstream carcdrs;
	for (int n = 2; n <= 4; n++) {
		for (int i = 0; i < 1 << n; i++) {
			std::ostringstream name;
			std::ostringstream body;
			for (int b = 0; b < n; b++) {
				char c = BITAD(BIT(i, b));
				name << c;
				body << "(c" << c << "r ";
			}
			carcdrs << "(def (c" << name.str() << "r x) " << body.str() << "x";
			for (int p = 0; p <= n; p++) carcdrs << ")";
			carcdrs << "\n";
		}
	}
	eval(carcdrs.str().c_str());
}


void State::eval(const char* source) {
	eval(ccharcopy(source));
}
void State::eval(char* source) {
	// lex the tokens from the source
	auto toks = lex(source);
	auto parser = new Parser(toks);

	std::vector<value::Object*> nodes;

	try {
		nodes = parser->parse_top_level();
	} catch (const char* msg) {
		std::cerr << "Parse Error: " << msg << "\n";
		return;
	}

	bool print_bytecode = false;
	// machine->debug = true;
	for (auto& node : nodes) {

		// create bytecode for this node.
		// each top level node gets it's own bytecode to evaluate in a "sandbox"
		// (in relation to the other things on the top level)
		// this is because each top level statement should be able to be evaluated
		// individually aside from assignments (which work in an enviroment like this)
		// because that's the only time you would use the returned value from a TLS
		vm::Bytecode bc;

		// reset the VM's stack. this is only done because it's top level and we don't care
		// about the previous nodes' effect on the stack
		machine->stack->index = 0;

		// attempt to compile the bytecode
		try {
			node->compile(machine, scope, &bc);
		} catch (const char* err) {
			std::cerr << "Compilation Failed: " << err << "\n";
			return;
		}


		if (print_bytecode) {
			std::cout << "; " << node->to_string() << "\n";
			for (int i = 0; i < bc.instructions.size(); i++) {
				printf("0x%04x %s\n", i, bc.instructions[i].to_string().c_str());
			}
			std::cout << "\n";
		}

		// attempt to evaluate the bytecode
		try {
			machine->eval(bc, scope);
		} catch (const char* err) {
			std::cerr << "Evaluation Failed: " << err << "\n";
			if (!repl) exit(1);
			return;
		} catch (std::string err) {
			std::cerr << "Evaluation Failed: " << err << "\n";
			if (!repl) exit(1);
			return;
		}
		if (machine->stack->index > 0) {
			auto top = machine->stack->pop();
			if (repl) {
				std::ostringstream name;
				name << "$";
				name << repl_index;
				scope->root->set(name.str(), top);
				repl_index++;
				if (top.type != value::nil)
					std::cout << "\x1B[90m" << name.str() << ": " << KGRN << top.to_string() << RST << "\n";
			}
		}
	}
}

std::vector<Token> State::lex(char* source) {
	Scanner *scanner = new Scanner(source);
	return scanner->run();
}

std::string get_file_ext(const std::string& s) {
   size_t i = s.rfind('.', s.length());
   if (i != std::string::npos) {
      return(s.substr(i+1, s.length() - i));
   }
   return("");
}

void State::eval_file(char* source) {
	std::string name(strdup(source));

	std::string ext = get_file_ext(name);

	if (ext.size() == 0) {
		name += ".wl";
	}

	try {
		char* contents = read_file_contents(ccharcopy(name.c_str()));
		eval(contents);
	} catch (const char* err) {
		throw err;
	}
}


void State::run_repl() {
	std::string line;
	repl = true;
	char* buf;

	while (true) {
		buf = linenoise("# ");

		std::cout << RST;
		if (buf == nullptr) break;

		if (strlen(buf) > 0) {
			linenoiseHistoryAdd(buf);
			eval(buf);
		}
		// free the buffer provided by linenoise
		free(buf);
	}
	repl = false;
}



