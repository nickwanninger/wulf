#include <wulf.hh>

#define EXIT_FILE_ERROR 1


int main(int argc, char** argv) {
	int ntoken, vtoken;


	if (argc <= 1) {
		std::cout << "Usage: wulfc <file>\n";
		exit(1);
	}

	char* filepath = argv[1];


	FILE* source = fopen(filepath, "r");

	Scanner *scanner = new Scanner(source);
	std::vector<Token> *toks = scanner->run();

	std::cout << toks->size() << "\n";

	delete toks;
	return 0;
}
