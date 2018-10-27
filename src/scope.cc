#include <scope.hh>
#include <value.hh>
#include <specialforms.hh>

using namespace scope;

static long hashstring(const char* str);

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
 * find a variable name in the scope or check all parents
 */
value::Object Scope::find(std::string name) {

	// attempt to read the binding from this scope's local map
	value::Object val;
	Scope* current = this;
	bool found = false;
	while (current != NULL) {
		if (current->bindings.contains(name)) {
			val = current->bindings[name];
			found = true;
			break;
		}
		current = current->parent;
	}
	if (!found) throw std::string("variable ") + name + " is not bound";
	bindings[name] = val;
	return val;

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
	set("define", specialform::setq);
	set("gc/collect", specialform::gc_collect);
	set("=", specialform::equals);
	set(">", specialform::greater);
	set("<", specialform::less);
	set("if", specialform::if_stmt);
	set("->", specialform::lambda);

	BINDSF(pow);
	BINDSF(print);
	BINDSF(quote);
	BINDSF(eval);
	BINDSF(load);
	BINDSF(lambda);
	BINDSF(set);
	BINDSF(setq);
	BINDSF(defun);
	BINDSF(repl);
	BINDSF(nand);
	BINDSF(first);
	BINDSF(rest);
}




static long hashstring(const char* str) {
	long hash = 5381;
	int size = strlen(str);
	for (long c = 0; c < size; c++) {
		hash = ((hash << 5) + hash) + c;
	}
	return hash;
}



bool Valmap::contains(const std::string& key) {
	long ind = hashstring(key.c_str());
	std::list<Bucket>& list = buckets[ind % BUCKET_SIZE];
	for (Bucket& buck : list) {
		if (buck.key == key) return true;
	}
	return false;
}

value::Object& Valmap::operator[](const std::string key) {
	long ind = hashstring(key.c_str());
	std::list<Bucket>& list = buckets[ind % BUCKET_SIZE];
	for (Bucket& buck : list) {
		if (buck.key == key) return *buck.val;
	}

	value::Object o;

	Bucket buck;
	buck.key = key;
	buck.val = new value::Object();
	list.push_front(buck);
	return *list.front().val;
}
