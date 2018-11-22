#include <bindings.hh>


wulf_binding(cons) {
	if (argc != 2) throw "cons requires 2 arguments";

	value::obj result = value::newobj(value::list);
	result->car = argv[0];
	result->cdr = argv[1];
	return result;
}

wulf_binding(car) {
	if (argc != 1) throw "car requires one argument";
	value::obj lst = argv[0];
	if (lst->type != value::list) {
		if (lst->type == value::nil) {
			return value::newobj();
		} else {
			throw "attempt to car non-list";
		}
	}
	return lst->car;
}


wulf_binding(cdr) {
	if (argc != 1) throw "cdr requires one argument";
	value::obj lst = argv[0];
	if (lst->type != value::list) {
		if (lst->type == value::nil) {
			return value::newobj();
		} else {
			throw "attempt to cdr non-list";
		}
	} else {
		if (lst->cdr == NULL) {
			return value::newobj();
		} else {
			if (lst->cdr->type == value::list && lst->cdr->length() == 0) {
				return value::newobj();
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
