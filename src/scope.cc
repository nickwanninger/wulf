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
value::Value* Scope::find(std::string name) {

	// attempt to read the binding from this scope's local map
	auto* found = bindings[name];
	if (found != NULL) {
		return found->val;
	}
	// the variable wasn't in this scope, we need to check the parent one
	// if it exists
	if (parent != NULL) {
		// std::cout << "parent\n";
		return parent->find(name);
	}
	// if the value wasn't found, give up and return a nil
	// TODO: decide if this should throw or not...
	return new value::Nil();
}


/*
 * set a binding in the current scope
 */
void Scope::set(std::string name, value::Value* val) {
	auto bind = new Binding(val);
	bindings[name] = bind;
}

void Scope::set(const char* name, value::Value* val) {
	set(std::string(name), val);
}

void Scope::set(const char* name, specialformfn fn) {
	set(std::string(name), new value::Procedure(fn));
}

void Scope::set(const char* name, double val) {
	set(name, new value::Number(val));
}


// void Scope::set(const char* name, value::Value

Binding::Binding(value::Value* v) {
	val = v;
}
// default constructor will bind nil to everything
Binding::Binding() {
	Binding(new value::Nil());
}


#define BINDSF(name) \
	set(#name, specialform::name)
void Scope::install_default_bindings() {
	set("+", specialform::add);
	set("-", specialform::sub);
	set("*", specialform::mul);
	set("/", specialform::div);


	BINDSF(pow);
	BINDSF(print);
	BINDSF(quote);
	BINDSF(eval);
	BINDSF(load);
	BINDSF(lambda);
	set("->", specialform::lambda);

	set("math/pi", 3.14159265358979323846);
	set("math/e",  2.71828182845904523534);
}




