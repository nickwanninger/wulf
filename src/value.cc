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

	auto rator = args[0]->eval(st, sc);
	auto rand = NULL;
	// build the vector for the procedure application
	valuelist fargs;
	for (int i = 1; i < args.size(); i++) {
		fargs.push_back(args[i]);
	}
	Procedure* proc = static_cast<Procedure*>(rator);
	if (proc->type == procedure) {
		return proc->apply(st, sc, fargs);
	}
	if (rator->type != procedure) {
		std::string err = "attempt to apply to non-procedure '";
		err += args[0]->to_string();
		err += "'";
		throw err;
	}
	return NULL;
}


// ---------------- Ident ----------------

Ident::Ident(char* val) {
	type = ident;
	value = std::string(val);
}

Ident::Ident(const char* val) {
	value = std::string(val);
	type = ident;
}

Ident::Ident(std::string v) {
	value = v;
	type = ident;
}


std::string Ident::to_string() {
	return value;
}

Value* Ident::eval(State* st, scope::Scope* sc) {
	return sc->find(value);
}


// ---------------- Number ----------------

/*
 * overloaded number constructor that
 * parses a string value
 */
Number::Number(char* val) {
	value = std::atof(val);
	type = number;
}

/*
 * overloaded Number constructor that
 * takes a long
 */
Number::Number(long val) {
	value = (double)val;
	type = number;
}

/*
 * overloaded Number constructor that
 * takes a double
 */
Number::Number(double val) {
	value = val;
	type = number;
}

/*
 * default Number constructor
 */
Number::Number() {
	type = number;
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

Nil::Nil() {
	type = nil;
}

std::string Nil::to_string() {
	return std::string("nil");
}

// nil just evaulates to itself, always
value::Value* Nil::eval(State* st, scope::Scope* sc) {
	return this;
}


// ---------------- Procedure ----------------

Procedure::Procedure(specialformfn func) {
	type = procedure;
	is_special_form = true;
	cfunc = func;
}

Procedure::Procedure(stringlist ar, Value* bd) {
	type = procedure;
	args = ar;
	body = bd;
}

Value* Procedure::apply(State* st, scope::Scope* sc, valuelist arglist) {
	if (is_special_form) {
		if (cfunc == NULL) {
			throw std::string("attempt to apply to special form that isn't bound");
		}
		return cfunc(st, sc->spawn_child(), arglist);
	}

	std::vector<Value*> vals;
	for (auto a : arglist)
		vals.push_back(a->eval(st, sc));

	scope::Scope* newscope = sc->spawn_child();
	if (args.size() != args.size()) {
		throw std::string("argument count invalid");
	}

	for (int i = 0; i < vals.size(); i++) {
		newscope->set(args[i], vals[i]);
	}
	auto res = body->eval(st, newscope);
	return res;
}

std::string Procedure::to_string() {

	if (is_special_form) {
		return "(special-form)";
	}

	std::ostringstream buf;

	buf << "(lambda (";

	for (int i = 0; i < args.size(); i++) {
		buf << args[i];
		if (i < args.size()-1) buf << " ";
	}

	buf << ") ";
	buf << body->to_string();
	buf << ")";
	return buf.str();
}

Value* Procedure::eval(State* st, scope::Scope* sc) {
	return this;
}



// ------------------ String ------------------


String::String(char* val) {
	value = std::string(val);
	type = string;
}

String::String(const char* val) {
	value = std::string(val);
	type = string;
}

String::String(std::string val) {
	value = val;
	type = string;
}
// strings evaluate to themselves
Value* String::eval(State* st, scope::Scope* sc) {
	return this;
}
std::string String::to_string() {
	return value;
}


#define CHECK_SIG(name) \
	bool value::name(Value* val)

#define VALUE_T_CHECK(name, type) \
	CHECK_SIG(name) { return dynamic_cast<type>(val) == NULL; }

VALUE_T_CHECK(is_number, value::Nil*)
VALUE_T_CHECK(is_string, value::String*)
VALUE_T_CHECK(is_list, value::List*)

CHECK_SIG(is_true) {
	// if the dynamic_cast of a value to nil is a NULL ptr,
	// it must be true, because it's not a nil
	return val->type != nil;
}

