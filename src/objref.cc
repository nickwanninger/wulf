#include <value.hh>

using namespace value;


objref::objref() {
}

objref::objref(value::Object *ref) {
	ptr = ref;
	refcount = new long;
	*refcount = 1;
}

objref::~objref() {
	destroy();
}

void objref::destroy(void) {
	if (weak) return;
	if (refcount != NULL && ptr != NULL) {
		if (*refcount <= 1) {
			// std::cout << "would delete " << ptr << std::endl;
			// delete ptr;
			// delete refcount;
		} else {
			(*refcount)--;
		}
	}
}


void objref::init(void) {
	refcount = new long;
	*refcount = 0;
}


value::Object *objref::get() const {
	return ptr;
}

value::Object *objref::operator->() {
	return ptr;
}

value::Object &objref::operator*() {
	return *ptr;
}

inline objref::operator bool() {
	return ptr != 0;
}


void objref::clone_from(const objref & orig) {
	this->ptr = orig.ptr;
	this->refcount = orig.refcount;
	if (orig.weak) weak = true;
	if (!weak && refcount != NULL) (*refcount)++;
}


bool value::operator==(const objref & a, const objref & b) {
	return a.get() == b.get();
}
bool value::operator!=(const objref & a, const objref & b) {
	return !(a.get() == b.get());
}


objref & objref::operator=(value::Object *ref) {
	destroy();
	ptr = ref;
	refcount = new long;
	*refcount = 1;
	return *this;
}


objref & objref::operator=(const objref &orig) {
	destroy();
	clone_from(orig);
	return *this;
}
