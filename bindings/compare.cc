#include <bindings.hh>

wulf_binding(wulf_equal) {
	if (argc != 2) throw "'=' requires 2 arguments";
	value::obj a = argv[0];
	value::obj b = argv[1];



	if (a->type == b->type && a->type == value::number) {
		if (a->number == b->number) {
			return value::newident("t");
		}
		return value::newobj(value::nil);
	}

	if (a->type == value::procedure && b->type == value::procedure) {
		if (a->code == b->code) {
			return value::newident("t");
		}
		return value::newobj(value::nil);
	}
	// special case nil and list comparison
	// todo: wrap this all in an `==` overloaded operator
	if (a->type == value::list || b->type == value::list) {
		if (a->type == value::nil || b->type == value::nil) {
			auto the_list = a->type == value::list ? a : b;
			auto the_nil = a->type == value::nil ? a : b;
			if (the_list->length() != 0) {
				return value::newobj(value::nil);
			}
		}
	} else if (a->type == b->type && a->to_string() == b->to_string()) {
		return value::newident("t");
	}
	return value::newobj(value::nil);
}

int compare_helper(value::obj a, value::obj b) {
	if (a->type != value::number || b->type != value::number)
		throw "unable to compare two non-numbers";

	if (a->number == b->number) return 0;
	if (a->number < b->number) return -1;
	return 1;
}

wulf_binding(wulf_lessthan) {
	if (argc != 2) throw "'<' requires 2 arguments";
	if (compare_helper(argv[0], argv[1]) == -1) return value::newident("t");
	return value::newobj(value::nil);
}
wulf_binding(wulf_greaterthan) {
	if (argc != 2) throw "'>' requires 2 arguments";
	if (compare_helper(argv[0], argv[1]) == 1) return value::newident("t");
	return value::newobj(value::nil);
}
