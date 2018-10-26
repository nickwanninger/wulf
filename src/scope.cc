#include <scope.hh>
#include <value.hh>
#include <specialforms.hh>

using namespace scope;


int scope_index = 0;
/*
 * default constructor for Scope that sets the parent to NULL
 */
Scope::Scope() {
	index = scope_index++;
	parent = NULL;
	root = this;
}

/*
 * constructor for scope that takes the parent
 */
Scope::Scope(Scope* p) {
	parent = p;
	root = parent->root;
	index = scope_index++;
	// int pi = (parent != NULL) ? parent->index : -1;
	// std::cout << "created scope (" << index << ", " << pi << ")\n";
}

/*
 * spawn a child scope and return it
 */
Scope* Scope::spawn_child() {
	return new Scope(this);
}


/*
 * recursively find a variable name in the scope
 */
value::Object Scope::find(std::string name) {
	// std::cout << "scope: " << index << " name: " << name <<  " parent: " << ((parent != NULL) ? parent->index : -1) << "\n";
	// attempt to read the binding from this scope's local map
	if (bindings.find(name) != bindings.end()) {
		return bindings[name];
	}
	// the variable wasn't in this scope, we need to check the parent one
	// if it exists
	if (parent != NULL) {
		// std::cout << "parent\n";
		return parent->find(name);
	}
	// if the value wasn't found, give up and return a nil
	throw std::string("variable ") + name + " is not bound";
	return value::Object();
}


/*
 * set a binding in the current scope
 */
void Scope::set(std::string name, value::Object val) {
	bindings[name] = val;
}

void Scope::set(const char* name, value::Object val) {
	set(std::string(name), val);
}

void Scope::set(const char* name, specialformfn fn) {
	set(std::string(name), value::Object(fn));
}

void Scope::set(const char* name, double val) {
	set(name, value::Object(val));
}



#define BINDSF(name) \
	set(#name, specialform::name)

void Scope::install_default_bindings() {
	set("+", specialform::add);
	set("-", specialform::sub);
	set("*", specialform::mul);
	set("/", specialform::div);

	set("t", value::Object("t"));
	set("nil", value::Object());
	BINDSF(pow);
	BINDSF(print);
	BINDSF(quote);
	BINDSF(eval);
	BINDSF(load);

	set("if", specialform::if_stmt);

	BINDSF(lambda);
	set("->", specialform::lambda);

	BINDSF(set);
	BINDSF(setq);
	BINDSF(defun);
	BINDSF(repl);
	set("define", specialform::setq);
	set("gc/collect", specialform::gc_collect);
	set("=", specialform::equals);
	set(">", specialform::greater);
	set("<", specialform::less);
	BINDSF(nand);
}




