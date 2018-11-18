#include <bindings.hh>
#include <vm.hh>
#include <math.h>

wulf_binding(add) {
	if (argc == 0) {
		return new value::Object(0.0);
	}
	double total = 0.0;
	for (int i = 0; i < argc; i++) {
		if (argv[i]->type != value::number) throw "addition requires only numbers";
		total += argv[i]->number;
	}
	delete argv;
	return new value::Object(total);
}


wulf_binding(sub) {
	if (argc == 0) {
		return new value::Object(0.0);
	}

	for (int i = 0; i < argc; i++)
		if (argv[i]->type != value::number) throw "subtraction requires only numbers as arguments";

	if (argc == 1) {
		return new value::Object(-argv[0]->number);
	}
	double total = argv[0]->number;
	for (int i = 1; i < argc; i++) total -= argv[i]->number;

	return new value::Object(total);
}

wulf_binding(mul) {
	if (argc == 0) {
		return new value::Object(0.0);
	}
	double total = 1;
	for (int i = 0; i < argc; i++) {
		if (argv[i]->type != value::number) throw "multiplication requires only numbers";
		total *= argv[i]->number;
	}
	delete argv;
	return new value::Object(total);
}


wulf_binding(divide) {
	if (argc == 0) {
		return new value::Object(0.0);
	}

	for (int i = 0; i < argc; i++)
		if (argv[i]->type != value::number)
			throw "division requires only numbers";

	if (argc == 1) {
		return new value::Object(1 / argv[0]->number);
	}
	double total = argv[0]->number;
	for (int i = 1; i < argc; i++) total /= argv[i]->number;

	return new value::Object(total);
}

wulf_binding(mod) {
	if (argc != 2) throw "modulus requires 2 arguments";
	for (int i = 0; i < argc; i++)
		if (argv[i]->type != value::number)
			throw "modulus requires only numbers";
	return new value::Object(fmod(argv[0]->number, argv[1]->number));
}




wulf_binding(wulf_pow) {
	if (argc != 2) throw "power requires 2 arguments";
	for (int i = 0; i < argc; i++)
		if (argv[i]->type != value::number)
			throw "pow requires only numbers";
	return new value::Object(pow(argv[0]->number, argv[1]->number));
}


#define single_math_binding(wulf_name, fn_name) \
	wulf_binding(wulf_name) { \
		if (argc != 1) throw #wulf_name " requires one argument"; \
		if (argv[0]->type != value::number) throw #wulf_name "requires a number"; \
		return new value::Object(fn_name(argv[0]->number)); }


single_math_binding(wulf_cos, cos);
single_math_binding(wulf_sin, sin);
single_math_binding(wulf_tan, tan);
single_math_binding(wulf_sqrt, sqrt);


