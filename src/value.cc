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

#include <value.hh>

using namespace value;

std::string Value::to_string() {
	return NULL;
}


Value* Value::eval(State* st, scope::Scope* sc) {
	return new Nil();
}

// ---------------- List ----------------

void List::push(Value *n) {
	args.push_back(n);
}

Value *List::operator[](const int index) {
	return args[index];
}

std::string List::to_string() {
	std::ostringstream os;
	os << "(";

	for (int i = 0; i < args.size(); i++) {
		auto& arg = args[i];
		os << arg->to_string();
		if (i < args.size()-1)
			os << " ";
	}
	os << ")";
	return os.str();
}

Value* List::eval(State* st, scope::Scope* sc) {
	if (args.size() == 0) {
		return new Nil();
	}
	/*
	 * ope
	 *   -rator
	 *   -rand
	 */
	auto rator = args[0];
	auto rand = NULL;
	return NULL;
}


// ---------------- Ident ----------------

Ident::Ident(char* val) {
	value = std::string(val);
}

Ident::Ident(const char* val) {
	value = std::string(val);
}

Ident::Ident(std::string v) {
	value = v;
}


std::string Ident::to_string() {
	return value;
}

Value* Ident::eval(State* st, scope::Scope* sc) {
	return NULL;
}


// ---------------- Number ----------------

/*
 * overloaded number constructor that
 * parses a string value
 */
Number::Number(char* val) {
	value = std::atof(val);
}

/*
 * overloaded Number constructor that
 * takes a long
 */
Number::Number(long val) {
	value = (double)val;
}

/*
 * overloaded Number constructor that
 * takes a double
 */
Number::Number(double val) {
	value = val;
}

/*
 * default Number constructor
 */
Number::Number() {
	value = 0;
}

// numbers always evaluate to themselves
Value* Number::eval(State* st, scope::Scope* sc) {
	return this;
}

std::string Number::to_string() {
	std::ostringstream os;
	os.precision(10);
	os << value;
	return os.str();
}


// ---------------- Nil ----------------

Nil::Nil() {}

std::string Nil::to_string() {
	return std::string("nil");
}

// nil just evaulates to itself, always
value::Value* Nil::eval(State* st, scope::Scope* sc) {
	return this;
}


