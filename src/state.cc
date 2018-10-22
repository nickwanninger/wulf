#include <wulf.hh>
#include <ast.hh>
#include <color.hh>

#include <readline/readline.h>
#include <readline/history.h>


void State::eval(char* source) {

	std::vector<Token> toks = lex(source);

	Parser* parser = new Parser(toks);
	std::vector<ast::Node*> nodes;
	try {
		nodes = parser->parse_top_level();
	} catch (const char* msg) {
		std::cout << "Evaluation Failed: " << msg << "\n";
		return;
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
	while ((buf = repl_readline()) != nullptr) {
		if (strlen(buf) > 0) {
			add_history(buf);
		} else {
			free(buf);
			continue;
		}
		eval(buf);
		free(buf);
	}
}

char* State::repl_readline() {
	rl_bind_key('\t', rl_insert);
	char* buf;
	char prompt[40];
	std::ostringstream os;
	os << "[" << repl_index++ << "]: ";

	buf = readline(os.str().c_str());
	return buf;
}

