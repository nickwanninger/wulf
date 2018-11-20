#include <bindings.hh>


wulf_binding(wulf_puts) {
	if (argc != 1) throw "puts requires one argument";
	std::cout << argv[0]->to_string(true);
	return new value::Object();
}

wulf_binding(print) {
	for (int i = 0; i < argc; i++) {
		std::cout << argv[i]->to_string(true) << " ";
	}
	std::cout << "\n";
	return new value::Object();
}




