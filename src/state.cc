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
#include <autonum.hh>
#include <stdlib.h>
#include <dlfcn.h>
#include <signal.h>
#include <path.hh>

#define BIT(num, n) (((num) >> (n)) & 1)
#define BITAD(b) ((b) == 0 ? 'a' : 'd')



value::obj proc_binding_binding(int argc, value::obj *argv, State* state, scope::Scope* sc) {
	if (argc != 2) {
		return value::newobj("must have two arguments");
	}

	void *handle;
	value::bind_func_t func;

	if (argv[0]->type != value::string)
		return value::newobj("first argument is not a string");
	if (argv[1]->type != value::string)
		return value::newobj("second argument is not a string");

	handle = dlopen(argv[0]->string, RTLD_LAZY);


	const char *err = dlerror();
	if (err)
		std::cerr << err << "\n";

	if (!handle) throw "dlopen failed to find a shared object file";

	func = (value::bind_func_t)dlsym(handle, argv[1]->string);
	if (!func)
		throw "not found";

	value::obj proc = value::newobj(value::procedure);
	proc->code = new vm::Bytecode();
	proc->code->type = vm::bc_binding;
	proc->code->binding = func;
	proc->code->name = argv[1]->string;



	return proc;
}

value::obj load_binding(int argc, value::obj *argv, State* state, scope::Scope* sc) {
	if (argc != 1) {
		throw "'load' requires one argument";
	}

	if (argv[0]->type != value::string) throw "'load' requires a string as an argument";

	apathy::Path oldpath = state->current_path;
	apathy::Path newpath;
	if (oldpath.is_file())
		oldpath = oldpath.up();
	newpath = oldpath.relative(argv[0]->string);
	bool repls = state->repl;
	state->repl = false;
	char* path = ccharcopy(newpath.string().c_str());
	state->eval_file(path);
	state->repl = repls;
	return value::newobj(value::nil);
}

State::State() {
	scope = new scope::Scope();
	scope->install_default_bindings();
	machine = new vm::Machine();
	machine->state = this;

	current_path = apathy::Path::cwd();

	bind("proc-binding", proc_binding_binding);
	// bootstrap the load procedure
	bind("load", load_binding);
	// load up the runtime
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

	std::vector<value::obj> nodes;

	try {
		nodes = parser->parse_top_level();
	} catch (const char* msg) {
		std::cerr << "Parse Error: " << msg << "\n";
		return;
	}

	for (value::obj node : nodes) {
		//std::cout << "parsed: " << node->to_string() << std::endl;
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
			value::obj top = machine->stack->pop();
			if (repl) {
				std::ostringstream name;
				name << "$";
				name << repl_index;
				scope->root->set(name.str(), top);
				repl_index++;
				// if (top->type != value::nil)
				std::cout << "\x1B[90m" << name.str() << ": " << RST;
				std::cout << repl_highlight((char*)top->to_string(false).c_str());
				std::cout << "\n";
			}
		}
	}
}


value::obj State::bind(const char *name, value::bind_func_t binding) {
	value::obj proc = value::newobj(value::procedure);
	proc->code = new vm::Bytecode();
	proc->code->type = vm::bc_binding;
	proc->code->name = name;
	proc->code->binding = binding;
	scope->set(name, proc);
	return proc;
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
   return "";
}

void State::eval_file(char* source) {
	apathy::Path oldpath = current_path;
	current_path = source;
	std::string name(strdup(source));

	std::string ext = get_file_ext(name);

	if (ext.size() == 0) {
		name += ".wl";
	}

	try {
		char* contents = read_file_contents(ccharcopy(name.c_str()));
		eval(contents);
		current_path = oldpath;
	} catch (const char* err) {
		throw err;
	}
}




void sigint(int a) {
	printf("\n^C caught\n");
}

void State::run_repl() {
	repl = true;
	char* buf;
	// signal(SIGINT, sigint);

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
	/// signal(SIGINT, SIG_DFL);
	// signal(SIGINT, SIG_DFL);
	repl = false;
}

