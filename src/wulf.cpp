#include <wulf.hpp>

int main(int argc, char** argv) {

	if (argc <= 1) {
		std::cout << "Usage: wulfc <file>\n";
		exit(1);
	}



	char* filepath = argv[1];

	char* contents = read_file_contents(filepath);
	std::cout << contents << "\n";

	// free up the contents
	delete[] contents;


	return 0;
}
