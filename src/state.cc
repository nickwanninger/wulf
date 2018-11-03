#include <wulf.hh>
#include <state.hh>
#include <value.hh>
#include <color.hh>
#include <fstream>
#include <linenoise.h>
#include <parser.hh>


State::State() {
	scope = new scope::Scope();
	scope->install_default_bindings();
	machine = new vm::Machine();
	machine->state = this;
}


void State::eval(char* source) {
	// lex the tokens from the source
	auto toks = lex(source);
	auto parser = new Parser(toks);

	std::vector<value::Object*> nodes;

	try {
		nodes = parser->parse_top_level();
	} catch (const char* msg) {
		std::cout << "Parse Error: " << msg << "\n";
		return;
	}

	vm::Bytecode bc;
	bool print_bytecode = false;
	// machine->debug = true;
	for (auto& node : nodes) {
		// empty the instructions for this top level node
		bc.instructions.empty();
		machine->stack->index = 0;

		// attempt to compile the bytecode
		try {
			node->compile(machine, &bc);
		} catch (const char* err) {
			std::cout << "Compilation Failed: " << err << "\n";
			return;
		}


		if (print_bytecode)
			for (auto& in : bc.instructions) {
				std::cout << ">> " << in.to_string() << "\n";
			}

		// attempt to evaluate the bytecode
		try {
			machine->eval(bc, scope);
		} catch (const char* err) {
			std::cout << "Evaluation Failed: " << err << "\n";
			machine->stack->dump();
			if (!repl) exit(1);
			return;
		} catch (std::string err) {
			std::cout << "Evaluation Failed: " << err << "\n";
			machine->stack->dump();
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
				std::cout << name.str() << " = " << top.to_string() << "\n";
			}
		}
	}
	delete parser;
	return;
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
	std::string name = source;

	std::string ext = get_file_ext(name);

	if (ext.size() == 0) {
		name += ".wl";
	}

	try {
		char* contents = read_file_contents((char*)name.c_str());
		eval(contents);
	} catch (const char* err) {
		throw err;
	}
}


void State::run_repl() {
	std::string line;
	linenoiseSetMultiLine(1);
	repl = true;
	char* buf;
	while (true) {
		auto prompt = scope->find("*wulf/repl-prompt*");
		buf = linenoise(prompt.to_string().c_str());
		if (buf == nullptr) break;

		if (strlen(buf) > 0) {
			linenoiseHistoryAdd(buf);
			eval(buf);
		}
		// free the buffer provided by linenoise
		free(buf);
	}
}



