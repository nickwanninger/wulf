#include <value.hh>

using namespace value;


objref::objref() {
}

objref::objref(value::Object *ref) {
	ptr = ref;
	if (ptr != nullptr)
		ptr->retain();
}

objref::~objref() {
	if (ptr != nullptr) {
		if (ptr->release() == 0) {
			// delete ptr;
		}
	} else {
	}
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


bool value::operator==(const objref & a, const objref & b) {
	return a.get() == b.get();
}
bool value::operator!=(const objref & a, const objref & b) {
	return !(a.get() == b.get());
}


objref & objref::operator=(value::Object *ref) {
	ptr = ref;
	if (ptr != nullptr)
		ptr->retain();
	return *this;
}


objref & objref::operator=(const objref &other) {
	if (this != &other) {
		if (ptr != nullptr) {
			// Decrement the old reference count
			// if reference become zero delete the old data
			if (ptr->release() == 0) {
				// delete ptr;
			}
		}

		// Copy the data and reference pointer
		// and increment the reference count
		ptr = other.ptr;
		if (ptr != nullptr) {
			ptr->retain();
		}
	}
	return *this;
}
