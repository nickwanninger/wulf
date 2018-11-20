#include <bindings.hh>


wulf_binding(cons) {
	if (argc != 2) throw "cons requires 2 arguments";

	value::Object *result = new value::Object(value::list);
	result->car = argv[0];
	result->cdr = argv[1];
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
	return lst->car;
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
		if (lst->cdr == NULL) {
			return new value::Object();
		} else {
			if (lst->cdr->type == value::list && lst->cdr->length() == 0) {
				return new value::Object();
			} else {
				return lst->cdr;
			}
		}
	}
}


wulf_binding(setf) {
	if (argc != 2) throw "setf requires 2 arguments";
	*argv[0] = *argv[1];
	return argv[0];
}
