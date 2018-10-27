#include <wulf.hh>
#include <value.hh>
#include <color.hh>
#include <fstream>
#include <linenoise.h>
#include <parser.hh>



State::State() {
	scope = new scope::Scope();
	scope->install_default_bindings();
}

void State::eval(char* source) {
	// lex the tokens from the source
	auto toks = lex(source);
	auto parser = new Parser(toks);

	std::vector<value::Object> nodes;

	try {
		nodes = parser->parse_top_level();
	} catch (const char* msg) {
		std::cout << "Parse Error: " << msg << "\n";
		return;
	}

	try {
		for (auto& node : nodes) {
			auto res = node.eval(this, scope);
			if (repl && res.type != value::nil) {
				std::cout << res.to_string() << "\n";
			}
		}
	} catch (std::string msg) {
		std::cout << "Exception: " << msg << "\n";
	} catch (const char* msg) {
		std::cout << "Exception: " << msg << "\n";
	}

	delete parser;
}

std::vector<Token> State::lex(char* source) {
	Scanner *scanner = new Scanner(source);
	return scanner->run();
}


void State::eval_file(char* source) {
	try {
		char* contents = read_file_contents(source);
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
		buf = linenoise("wulf> ");
		if (buf == nullptr) break;

		if (strlen(buf) > 0) {
			linenoiseHistoryAdd(buf);
			eval(buf);
		}
		// free the buffer provided by linenoise
		free(buf);
	}
}



