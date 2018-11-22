#include <bindings.hh>


wulf_binding(wulf_puts) {
	if (argc != 1) throw "puts requires one argument";

	argv[0]->write_stream(std::cout, true);
	return value::newobj();
}

wulf_binding(print) {
	for (int i = 0; i < argc; i++) {
		std::cout << argv[i]->to_string(true) << " ";
	}
	std::cout << "\n";
	return value::newobj();
}




