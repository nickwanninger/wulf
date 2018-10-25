#include <specialforms.hh>
#include <value.hh>
#include <math.h>
#include <functional>

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

#define NUMOPLAMBDA(op) \
	numberop(st, sc, args, [](double a, double b) -> double { return a op b; })

#define FORM(name) \
	value::Value* specialform::name(State* st, scope::Scope* sc, valuelist args)

FORM(add) {
	return NUMOPLAMBDA(+);
}

FORM(sub) {
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
