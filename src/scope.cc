#include <scope.hh>
#include <value.hh>

using namespace scope;

/*
 * default constructor for Scope that sets the parent to NULL
 */
Scope::Scope() {
	parent = NULL;
	root = this;
}

/*
 * constructor for scope that takes the paren
 */
Scope::Scope(Scope* p) {
	parent = p;
	root = parent->root;
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
		return found;
	}
	// the variable wasn't in this scope, we need to check the parent one
	// if it exists
	if (parent != NULL) {
		return find(name);
	}
	// if the value wasn't found, give up and return a nil
	// TODO: decide if this should throw or not...
	return new value::Nil();
}


/*
 * set a binding in the current scope
 */
void Scope::set(std::string name, value::Value* val) {
	bindings[name] = val;
}




