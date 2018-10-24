#include <wulf.hh>
#include <value.hh>
#include <color.hh>
#include <fstream>
#include <linenoise.h>
#include <parser.hh>


void State::eval(char* source) {

	std::vector<Token> toks = lex(source);

	Parser* parser = new Parser(toks);

	std::vector<value::Value*> nodes;
	try {
		nodes = parser->parse_top_level();
	} catch (const char* msg) {
		std::cout << "Evaluation Failed: " << msg << "\n";
		return;
	}

	for (value::Value* node : nodes) {
		std::cout << node->to_string() << "\n";
	}
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

	char* buf;
	while (true) {
		buf = linenoise("> ");
		if (buf == nullptr) break;

		if (strlen(buf) > 0) {
			linenoiseHistoryAdd(buf);
			eval(buf);
		}
		// free the buffer provided by linenoise
		free(buf);
	}
}



