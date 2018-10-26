#include <specialforms.hh>
#include <value.hh>
#include <math.h>
#include <functional>
#include <gc/gc.h>
using namespace specialform;


value::Value* numberop(State* st, scope::Scope* sc, valuelist args, std::function<double(double, double)> fn) {
	if (args.size() != 2) {
		throw std::string("call to binary op requires two arguments");
	}
	auto* val1 = static_cast<value::Number*>(args[0]->eval(st, sc));
	auto* val2 = static_cast<value::Number*>(args[1]->eval(st, sc));
	if (!val1 || !val2) {
		return new value::Nil();
	}
	if (val1->type == val2->type && val1->type == value::number) {
		return new value::Number(fn(val1->value, val2->value));
	}
	return new value::Nil();
}


#define BOOLVAL(val) ((val) ? (value::Value*)(new value::Ident("t")) : (value::Value*)(new value::Nil()))

#define NUMOPLAMBDA(op) \
	numberop(st, sc, args, [](double a, double b) -> double { return a op b; })

#define FORM(name) \
	value::Value* specialform::name(State* st, scope::Scope* sc, valuelist args)

FORM(add) {
	return NUMOPLAMBDA(+);
}

FORM(sub) {
	// sub is a *little* special.
	// if there is only one argument, we just negate it.
	// otherwise we do normal binary ops
	if (args.size() == 1) {
		auto val = args[0]->eval(st, sc);
		auto num = dynamic_cast<value::Number*>(val);
		if (num == NULL) {
			throw "attempt to negate non-number";
		}
		return new value::Number(num->value * -1);
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
		auto val = arg->eval(st, sc);
		os << val->to_string();
		if (i < args.size()-1)
			os << " ";
	}
	std::cout << os.str() << std::endl;
	return new value::Nil();
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
	return args[0]->eval(st, sc)->eval(st, sc);
}

FORM(load) {
	return new value::Nil();
}


FORM(lambda) {
	if (args.size() != 2) {
		throw std::string("invalid syntax in lambda: must have two arguments. ex: (lambda (..) ..)");
	}
	auto* argnames = static_cast<value::List*>(args[0]);
	if (argnames == NULL)
		throw std::string("argument 1 to lambda definition is not a list of names");

	std::vector<std::string> arglist;
	// for (auto a : argnames->List::z)
	for (auto a : argnames->args) {
		auto* ident = static_cast<value::Ident*>(a);
		if (ident == NULL)
			throw std::string("argument name to lambda is not an identifier");
		arglist.push_back(ident->to_string());
	}
	return new value::Procedure(arglist, args[1]);
}

FORM(defun) {
	if (args.size() != 3) {
		throw std::string("invalid syntax in defun: must have two arguments. ex: (defun name (args..) (body..))");
	}

	auto nameval = dynamic_cast<value::Ident*>(args[0]);
	if (nameval == NULL)
		throw std::string("first argument to defun is not an identifier");
	std::string name = nameval->to_string();

	auto* argnames = dynamic_cast<value::List*>(args[1]);
	if (args[1]->type != value::list && argnames == NULL)
		throw std::string("argument 2 to defun is not a list of names");
	std::vector<std::string> arglist;
	for (auto a : argnames->args) {
		auto* ident = dynamic_cast<value::Ident*>(a);
		if (ident == NULL)
			throw std::string("argument name to defun is not an identifier");
		arglist.push_back(ident->to_string());
	}
	auto* func = new value::Procedure(arglist, args[2]);
	sc->root->set(name, func);
	return func;
}

FORM(set) {
	if (args.size() != 2) {
		throw std::string("invalid syntax in set: must have two arguments. ex: (set 'foo bar)");
	}
	auto name = args[0]->eval(st, sc);
	value::Ident* n = static_cast<value::Ident*>(name);
	if (n->type != value::ident)
		throw std::string("attempt to assign to non-identifier ") + n->to_string();
	auto val = args[1]->eval(st, sc);
	sc->root->set(name->to_string(), val);
	return val;
}

FORM(setq) {
	if (args.size() != 2) {
		throw std::string("invalid syntax in setq: must have two arguments. ex: (setq foo bar)");
	}
	auto name = args[0];
	value::Ident* n = static_cast<value::Ident*>(name);
	if (n->type != value::ident)
		throw std::string("attempt to assign to non-identifier ") + n->to_string();
	auto val = args[1]->eval(st, sc);
	sc->root->set(name->to_string(), val);
	return val;
}

FORM(gc_collect) {
	GC_gcollect();
	return new value::Nil();
}

FORM(if_stmt) {
	// if there isn't enough for an else
	if (args.size() < 3) {
		// and there isn't enough for only a case and a then
		if (args.size() != 2) {
			throw "invalid syntax in if conditional (incorrect number of arguments)";
		}
		// there wasn't a provided "else" value, so push a nil
		args.push_back(new value::Nil());
	}

	auto predicate = args[0]->eval(st, sc);
	auto then_expr = args[1];
	auto else_expr = args[2];

	if (value::is_true(predicate)) {
		return then_expr->eval(st, sc);
	}

	return else_expr->eval(st, sc);
	std::cout << args.size() << "\n";
	return new value::Nil();
}

FORM(repl) {
	st->run_repl();
	return new value::Nil();
}

FORM(equals) {
	if (args.size() != 2) throw "incorrect arg count provided to =";


	auto a = args[0]->eval(st, sc);
	auto b = args[1]->eval(st, sc);

	if (a->type != b->type) return new value::Nil();

	return BOOLVAL(a->to_string() == b->to_string());
}



FORM(greater) {
	if (args.size() != 2) throw "incorrect arg count provided to >";


	auto a = args[0]->eval(st, sc);
	auto b = args[1]->eval(st, sc);

	if (a->type != value::number || a->type != b->type) throw "comparison to a non number failed";

	auto numa = dynamic_cast<value::Number*>(a);
	auto numb = dynamic_cast<value::Number*>(b);
	return BOOLVAL(numa->value > numb->value);
}

FORM(nand) {
	if (args.size() != 2) throw "incorrect arg count provided to nand";
	auto a = args[0]->eval(st, sc);
	auto b = args[1]->eval(st, sc);
	bool at = value::is_true(a);
	bool bt = value::is_true(b);
	return BOOLVAL(!(at && bt));
}




