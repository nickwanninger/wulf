/*
 * A compiler for the wulf language
 * Copyright (C) 2018  Nick Wanninger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#include <scope.hh>
#include <value.hh>

using namespace scope;

static long hashstring(const char* str);

int scope_index = 0;
/*
 * default constructor for Scope that sets the parent to NULL
 */
Scope::Scope() {
	parent = NULL;
	root = this;
	index = scope_index++;
	bindings = Valmap(30);
}

/*
 * constructor for scope that takes the parent
 */
Scope::Scope(Scope* p) {
	parent = p;
	root = parent->root;
	index = scope_index++;
	bindings = Valmap(30);
}

/*
 * spawn a child scope and return it
 */
Scope* Scope::spawn_child() {
	return new Scope(this);
}



Bucket* Scope::find_bucket(std::string name) {

	Scope *current = this;
	while (current != NULL) {
		if (current->bindings.contains(name)) {
			return current->bindings.getbucket(name);
		}
		current = current->parent;
	}
	// base case. (there wasn't a bucket)
	return NULL;
}

/*
 * find a variable name in the scope or check all parents
 */
value::Object Scope::find(std::string name) {
	Bucket* buck = find_bucket(name);
	if (buck == NULL) {
		throw std::string("variable ") + name + " is not bound";
	}
	return buck->val;
}



std::string Scope::to_string() {
	std::ostringstream buf;
	return buf.str();
}

/*
 * set a binding in the current scope
 */
void Scope::set(std::string name, value::Object val) {
	bindings.getbucket(name)->val = val;
}

void Scope::set(const char* name, value::Object val) {
	set(std::string(name), val);
}

void Scope::set(const char* name, double val) {
	set(name, value::Object(val));
}

void Scope::install_default_bindings() {
}


static size_t hashstring(const std::string str) {

	std::hash<std::string> hash_fn;

	size_t hash = hash_fn(str);
	return hash;
}


Valmap::Valmap() {
	bucketsize = 4;
	buckets = new Bucket*[bucketsize];
}


Valmap::Valmap(long s) {
	bucketsize = s;
	buckets = new Bucket*[s];
}

bool Valmap::contains(const std::string& key) {
	long ind = hashstring(key) % bucketsize;
	Bucket* b = buckets[ind];
	while (b != NULL) {
		if (b->key == key) return true;
		b = b->next;
	}
	return false;
}

Bucket* Valmap::getbucket(const std::string key) {
	long ind = hashstring(key) % bucketsize;
	Bucket* b = buckets[ind];
	while (b != NULL) {
		if (b->key == key) {
			return b;
		}
		b = b->next;
	}


	b = new Bucket();
	b->key = key;
	b->next = buckets[ind];
	buckets[ind] = b;
	return b;
}

value::Object& Valmap::operator[](const std::string key) {
	return getbucket(key)->val;
}
