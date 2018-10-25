#include <specialforms.hh>
#include <value.hh>

using namespace specialform;


value::Value* numberop(valuelist args, std::function<double(double, double)> fn) {
	if (args.size() != 2) {
		throw std::string("call to binary op requires two arguments");
	}
	auto* val1 = static_cast<value::Number*>(args[0]);
	auto* val2 = static_cast<value::Number*>(args[1]);
	if (!val1 || !val2) {
		return new value::Nil();
	}
	if (val1->type == val2->type && val1->type == value::number) {
		return new value::Number(fn(val1->value, val2->value));
	}
	return new value::Nil();
}

#define NUMOPLAMBDA(op) \
	numberop(args, [](double a, double b) -> double { return a op b; })

value::Value* specialform::add(State* st, scope::Scope* sc, valuelist args) {
	return NUMOPLAMBDA(+);
}

value::Value* specialform::sub(State* st, scope::Scope* sc, valuelist args) {
	return NUMOPLAMBDA(-);
}

value::Value* specialform::mul(State* st, scope::Scope* sc, valuelist args) {
	return NUMOPLAMBDA(*);
}

value::Value* specialform::div(State* st, scope::Scope* sc, valuelist args) {
	return NUMOPLAMBDA(/);
}
