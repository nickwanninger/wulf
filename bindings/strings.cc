#include <bindings.hh>




wulf_binding(wulf_str) {
	if (argc != 1) throw "str requires one argument";
	std::string res = argv[0]->to_string(true);
	return value::newobj(ccharcopy(res.c_str()));
}


wulf_binding(wulf_strlen) {
	if (argc != 1) throw "strlen requires one argument";
	if (argv[0]->type != value::string) throw "strlen requires a string";
	return value::newobj(strlen(argv[0]->string));
}

wulf_binding(wulf_strcat) {
	if (argc < 2) throw "strcat requires at least 2 arguments";
	std::string s = argv[0]->to_string(true);
	for (int i = 1; i < argc; i++) {
		s += argv[i]->to_string(true);
	}
	return value::newobj(ccharcopy(s.c_str()));
}

wulf_binding(wulf_strseq) {
	if (argc != 3)
		throw "strseq requires 3 arguments";
	if (argv[0]->type != value::string)
		throw "strseq requires a string as the first argument";

	if (argv[1]->type != value::number || argv[2]->type != value::number)
		throw "strseq requires numbers as 2nd and 3rd arguments";
	std::string target = argv[0]->string;
	int64_t a = argv[1]->number;
	int64_t b = argv[2]->number;
	if (a < 0 || a > target.size()) throw "lower bound of strseq is out of bounds";
	if (a > b) throw "lower bound of strseq is above the upper bound";
	if (b > target.size()) throw "upper bound of strseq is out of bounds";
	target = target.substr(a, b-a);
	return value::newobj(ccharcopy(target.c_str()));
}
