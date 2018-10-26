#include <specialforms.hh>
#include <value.hh>
#include <math.h>
#include <functional>
#include <gc/gc.h>
using namespace specialform;


value::Object numberop(State* st, scope::Scope* sc, valuelist args, std::function<double(double, double)> fn) {
	if (args.size() != 2) {
		throw std::string("call to binary op requires two arguments");
	}
	auto val1 = args[0].eval(st, sc);
	auto val2 = args[1].eval(st, sc);

	// std::cout << "binop: " << val1.to_string() << ", " << val2.to_string() << "\n";
	if (val1.type == val2.type && val1.type == value::number) {
		auto obj = value::Object();
		obj.type = value::number;
		obj.number = fn(val1.number, val2.number);
		return obj;
	}
	return value::Object();
}


#define BOOLVAL(val) ((val) ? (value::Object("t")) : (value::Object()))

#define NUMOPLAMBDA(op) \
	numberop(st, sc, args, [](double a, double b) -> double { return a op b; })

#define FORM(name) \
	value::Object specialform::name(State* st, scope::Scope* sc, valuelist args)

FORM(add) {
	return NUMOPLAMBDA(+);
}

FORM(sub) {
	// sub is a *little* special.
	// if there is only one argument, we just negate it.
	// otherwise we do normal binary ops
	if (args.size() == 1) {
		auto val = args[0].eval(st, sc);
		if (val.type != value::number) {
			throw "attempt to negate non-number";
		}
		return value::Object(val.number * -1);
	}
	return NUMOPLAMBDA(-);
}

FORM(mul) {
	return NUMOPLAMBDA(*);
}

FORM(div) {
	return NUMOPLAMBDA(/);
}

FORM(pow) {
	return numberop(st, sc, args, [](double a, double b) -> double { return powf(a,b); });
}

FORM(print) {
	std::ostringstream os;
	for (int i = 0; i < args.size(); i++) {
		auto& arg = args[i];
		auto val = arg.eval(st, sc);
		os << val.to_string();
		if (i < args.size()-1)
			os << " ";
	}
	std::cout << os.str() << std::endl;
	return value::Object();
}

FORM(quote) {
	if (args.size() != 1) {
		throw std::string("invalid syntax in quote: must have one argument");
	}
	return args[0];
}

FORM(eval) {
	if (args.size() != 1) {
		throw std::string("invalid syntax in eval: must have one argument");
	}
	return args[0].eval(st, sc).eval(st, sc);
}

FORM(load) {
	return value::Object();
}


FORM(lambda) {
	if (args.size() != 2) {
		throw std::string("invalid syntax in lambda: must have two arguments. ex: (lambda (..) ..)");
	}
	auto argnames = args[0];
	if (argnames.type != value::list)
		throw std::string("argument 1 to lambda definition is not a list of names");

	std::vector<std::string> arglist;
	// for (auto a : argnames->List::z)
	for (auto a : argnames.list) {
		if (a.type != value::ident)
			throw std::string("argument name to lambda is not an identifier");
		arglist.push_back(a.to_string());
	}
	// allocate the body in dynamic memory, then copy the old body into it.
	auto* body = new value::Object();
	*body = args[1];
	return value::Object(arglist, body);
}

FORM(defun) {
	if (args.size() != 3) {
		throw std::string("invalid syntax in defun: must have two arguments. ex: (defun name (args..) (body..))");
	}

	auto nameval = args[0];
	if (nameval.type != value::ident)
		throw std::string("first argument to defun is not an identifier");
	std::string name = nameval.ident;

	auto argnames = args[1];
	if (argnames.type != value::list)
		throw std::string("argument 2 to defun is not a list of names");

	std::vector<std::string> arglist;
	for (auto ident : argnames.list) {
		if (ident.type != value::ident)
			throw std::string("argument name to defun is not an identifier");
		arglist.push_back(ident.ident);
	}
	auto* body = new value::Object();
	*body = args[2];
	auto func = value::Object(arglist, body);
	sc->root->set(name, func);
	return func;
}

FORM(set) {
	if (args.size() != 2) {
		throw std::string("invalid syntax in set: must have two arguments. ex: (set 'foo bar)");
	}
	auto name = args[0].eval(st, sc);
	if (name.type != value::ident)
		throw std::string("attempt to assign to non-identifier ") + name.to_string();
	auto val = args[1].eval(st, sc);
	sc->root->set(name.to_string(), val);
	return val;
}

FORM(setq) {
	if (args.size() != 2) {
		throw std::string("invalid syntax in setq: must have two arguments. ex: (setq foo bar)");
	}
	auto name = args[0];
	if (name.type != value::ident)
		throw std::string("attempt to assign to non-identifier ") + name.to_string();
	auto val = args[1].eval(st, sc);
	sc->root->set(name.to_string(), val);
	return val;
}

FORM(gc_collect) {
	GC_gcollect();
	return value::Object();
}

FORM(if_stmt) {
	// if there isn't enough for an else
	if (args.size() < 3) {
		// and there isn't enough for only a case and a then
		if (args.size() != 2) {
			throw "invalid syntax in if conditional (incorrect number of arguments)";
		}
		// there wasn't a provided "else" value, so push a nil
		args.push_back(value::Object());
	}

	auto predicate = args[0].eval(st, sc);
	auto then_expr = args[1];
	auto else_expr = args[2];

	if (predicate.type != value::nil) {
		return then_expr.eval(st, sc);
	}
	return else_expr.eval(st, sc);
}

FORM(repl) {
	st->run_repl();
	return value::Object();
}

FORM(equals) {
	if (args.size() != 2) throw "incorrect arg count provided to =";


	auto a = args[0].eval(st, sc);
	auto b = args[1].eval(st, sc);

	if (a.type != b.type) return value::Object();
	// check if the string representation is the same...
	// we can do this because the string representation generator
	// is fully featured, and can serialize any object.
	// (as long as both types are the same)
	return BOOLVAL(a.to_string() == b.to_string());
}



FORM(greater) {
	if (args.size() != 2) throw "incorrect arg count provided to >";
	auto a = args[0].eval(st, sc);
	auto b = args[1].eval(st, sc);
	if (a.type != value::number || a.type != b.type) throw "comparison to a non number failed";
	return BOOLVAL(a.number > b.number);
}
FORM(less) {
	if (args.size() != 2) throw "incorrect arg count provided to >";
	auto a = args[0].eval(st, sc);
	auto b = args[1].eval(st, sc);
	if (a.type != value::number || a.type != b.type) throw "comparison to a non number failed";
	return BOOLVAL(a.number < b.number);
}


FORM(nand) {
	if (args.size() != 2) throw "incorrect arg count provided to nand";
	auto a = args[0].eval(st, sc);
	auto b = args[1].eval(st, sc);
	return BOOLVAL(!(a.is_true() && b.is_true()));
}




