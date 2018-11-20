#include <bindings.hh>


wulf_binding(cons) {
	if (argc != 2) throw "cons requires 2 arguments";

	value::Object *result = new value::Object(value::list);
	result->first = new value::Object(*argv[0]);
	result->last = new value::Object(*argv[1]);
	return result;
}

wulf_binding(car) {
	if (argc != 1) throw "car requires one argument";
	value::Object *lst = argv[0];
	if (lst->type != value::list) {
		if (lst->type == value::nil) {
			return new value::Object();
		} else {
			throw "attempt to car non-list";
		}
	}
	return new value::Object(*lst->first);
}


wulf_binding(cdr) {
	if (argc != 1) throw "cdr requires one argument";
	value::Object *lst = argv[0];
	if (lst->type != value::list) {
		if (lst->type == value::nil) {
			return new value::Object();
		} else {
			throw "attempt to cdr non-list";
		}
	} else {
		if (lst->last == NULL) {
			return new value::Object();
		} else {
			if (lst->last->type == value::list && lst->last->length() == 0) {
				return new value::Object();
			} else {
				return new value::Object(*lst->last);
			}
		}
	}
}
