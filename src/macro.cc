#include <macro.hh>
#include <vm.hh>

using namespace macro;

// #define MACRO_DEBUG
value::Object macro::Expansion::expand(vm::Machine* machine, std::vector<value::Object> arg_objs, scope::Scope* calling_scope) {

#ifdef MACRO_DEBUG
	std::cout << ";\tExpanding macro " << this->name << "\n";
	std::cout << ";\targs expected: (";
	for (auto a : args) {
		if (a.type == value::rest) std::cout << ":rest ";
		std::cout << a.name << " ";
	}
	std::cout << "\b)\n";
	std::cout << ";\targs passed: (";
	for (auto a : arg_objs) std::cout << a.to_string() << " ";
	std::cout << "\b)\n";

	std::cout << ";\tbody: " << body.to_string() << "\n";
#endif

	scope::Scope *evaluation_scope = calling_scope->spawn_child();
	value::argument_scope_expand(args, arg_objs, evaluation_scope);
	value::Object exp = machine->eval(body, evaluation_scope);
#ifdef MACRO_DEBUG
	std::cout << ";\tExpansion: " << exp.to_string() << "\n";
#endif
	return exp;
}
