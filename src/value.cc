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


static int objindex = 0;
Object::Object() {
	index = objindex++;
	// std::cout << "constructed object " << index << "\n";
}

Object::Object(double val) {
	type = value::number;
	number = val;
	Object();
}
Object::Object(std::string val) {
	type = value::ident;
	ident = val;
	Object();
}
Object::Object(std::vector<Object> val) {
	type = value::list;
	list = val;
	Object();
}

Object::Object(std::vector<std::string> args, Object* body) {
	type = value::procedure;
	proc.args = args;
	proc.body = body;
	Object();
}

Object::Object(specialformfn func) {
	proc.special = true;
	proc.func = func;
	type = value::procedure;
	Object();
}
Object::~Object() {}


std::string Object::to_string() {
	std::ostringstream buf;

	switch (type) {
		case unknown:
		case value::nil:
			buf << "nil";
			break;

		case value::list:
			buf << "(";
			for (int i = 0; i < list.size(); i++) {
				buf << list[i].to_string();
				if (i < list.size()-1) buf << " ";
			}
			buf << ")";
			break;

		case value::ident:
		case value::string:
			buf << ident;
			break;

		case value::number:
			buf << number;
			break;

			// procedure stringifier
		case value::procedure:
			if (proc.special) return "(special-form)";
			buf << "(lambda (";
			for (int i = 0; i < proc.args.size(); i++) {
				buf << proc.args[i];
				if (i < proc.args.size()-1) buf << " ";
			}
			buf << ") ";
			buf << proc.body->to_string();
			buf << ")";
			break;
	}


	return buf.str();
}


Object Object::eval(State* st, scope::Scope* sc) {
	// all these things need to evaluate to themselves
	if (type == value::nil) return *this;
	if (type == value::string) return *this;
	if (type == value::number) return *this;
	if (type == value::procedure) return *this;
	// identifiers must find their value in the scope
	if (type == value::ident) return sc->find(ident);
	if (type == value::list) return eval_list(st, sc);

	// return a nil object
	return Object();
}



Object Object::eval_list(State* st, scope::Scope* sc) {
	if (list.size() == 0)
		return Object();

	/*
	 * ope
	 *   -rator
	 *   -rand
	 */
	Object rator = list[0].eval(st, sc);
	valuelist args;

	for (int i = 1; i < list.size(); i++) {
		args.push_back(list[i]);
	}

	if (rator.type != value::procedure)
		throw std::string("attempt to apply to non-procedure ") + list[0].to_string();

	return rator.apply(st, sc, args);
}


Object Object::apply(State* st, scope::Scope* sc, std::vector<Object> args) {
	scope::Scope newscope(sc);
	if (proc.special) {
		if (proc.func == NULL)
			throw std::string("attempt to apply to special form that isn't bound");
		return proc.func(st, &newscope, args);
	}

	// modify the args in place
	for (auto& val : args ) {
		val = val.eval(st, sc);
	}

	if (args.size() != proc.args.size())
		throw "invalid argument count";

	for (int i = 0; i < args.size(); i++) {
		newscope.set(proc.args[i], args[i]);
	}
	return proc.body->eval(st, &newscope);
}


bool Object::is_true() {
	return type != value::nil;
}

// std::string Value::to_string() {
// 	return NULL;
// }


// Value* Value::eval(State* st, scope::Scope* sc) {
// 	return new Nil();
// }

// // ---------------- List ----------------

// void List::push(Value *n) {
// 	args.push_back(n);
// }

// Value *List::operator[](const int index) {
// 	return args[index];
// }

// std::string List::to_string() {
// 	std::ostringstream os;
// 	os << "(";

// 	for (int i = 0; i < args.size(); i++) {
// 		auto& arg = args[i];
// 		os << arg->to_string();
// 		if (i < args.size()-1)
// 			os << " ";
// 	}
// 	os << ")";
// 	return os.str();
// }


// Value* List::eval(State* st, scope::Scope* sc) {
// 	if (args.size() == 0) {
// 		return new Nil();
// 	}

// 	/*
// 	 * ope
// 	 *   -rator
// 	 *   -rand
// 	 */

// 	auto rator = args[0]->eval(st, sc);
// 	auto rand = NULL;
// 	// build the vector for the procedure application
// 	valuelist fargs;
// 	for (int i = 1; i < args.size(); i++) {
// 		fargs.push_back(args[i]);
// 	}
// 	Procedure* proc = static_cast<Procedure*>(rator);
// 	if (proc->type == procedure) {
// 		return proc->apply(st, sc, fargs);
// 	}
// 	if (rator->type != procedure) {
// 		std::string err = "attempt to apply to non-procedure '";
// 		err += args[0]->to_string();
// 		err += "'";
// 		throw err;
// 	}
// 	return NULL;
// }


// // ---------------- Ident ----------------

// Ident::Ident(char* val) {
// 	type = ident;
// 	value = std::string(val);
// }

// Ident::Ident(const char* val) {
// 	value = std::string(val);
// 	type = ident;
// }

// Ident::Ident(std::string v) {
// 	value = v;
// 	type = ident;
// }


// std::string Ident::to_string() {
// 	return value;
// }

// Value* Ident::eval(State* st, scope::Scope* sc) {
// 	return sc->find(value);
// }


// // ---------------- Number ----------------

// /*
//  * overloaded number constructor that
//  * parses a string value
//  */
// Number::Number(char* val) {
// 	value = std::atof(val);
// 	type = number;
// }

// /*
//  * overloaded Number constructor that
//  * takes a long
//  */
// Number::Number(long val) {
// 	value = (double)val;
// 	type = number;
// }

// /*
//  * overloaded Number constructor that
//  * takes a double
//  */
// Number::Number(double val) {
// 	value = val;
// 	type = number;
// }

// /*
//  * default Number constructor
//  */
// Number::Number() {
// 	type = number;
// 	value = 0;
// }

// // numbers always evaluate to themselves
// Value* Number::eval(State* st, scope::Scope* sc) {
// 	return this;
// }

// std::string Number::to_string() {
// 	std::ostringstream os;
// 	os.precision(10);
// 	os << value;
// 	return os.str();
// }


// // ---------------- Nil ----------------

// Nil::Nil() {
// 	type = nil;
// }

// std::string Nil::to_string() {
// 	return std::string("nil");
// }

// // nil just evaulates to itself, always
// value::Value* Nil::eval(State* st, scope::Scope* sc) {
// 	return this;
// }


// // ---------------- Procedure ----------------

// Procedure::Procedure(specialformfn func) {
// 	type = procedure;
// 	is_special_form = true;
// 	cfunc = func;
// }

// Procedure::Procedure(stringlist ar, Value* bd) {
// 	type = procedure;
// 	args = ar;
// 	body = bd;
// }

// Value* Procedure::apply(State* st, scope::Scope* sc, valuelist arglist) {
// 	if (is_special_form) {
// 		if (cfunc == NULL) {
// 			throw std::string("attempt to apply to special form that isn't bound");
// 		}
// 		return cfunc(st, sc->spawn_child(), arglist);
// 	}

// 	std::vector<Value*> vals;
// 	for (auto a : arglist)
// 		vals.push_back(a->eval(st, sc));

// 	scope::Scope* newscope = sc->spawn_child();
// 	if (args.size() != args.size()) {
// 		throw std::string("argument count invalid");
// 	}

// 	for (int i = 0; i < vals.size(); i++) {
// 		newscope->set(args[i], vals[i]);
// 	}
// 	auto res = body->eval(st, newscope);
// 	return res;
// }

// std::string Procedure::to_string() {

// 	if (is_special_form) {
// 		return "(special-form)";
// 	}

// 	std::ostringstream buf;

// 	buf << "(lambda (";

// 	for (int i = 0; i < args.size(); i++) {
// 		buf << args[i];
// 		if (i < args.size()-1) buf << " ";
// 	}

// 	buf << ") ";
// 	buf << body->to_string();
// 	buf << ")";
// 	return buf.str();
// }

// Value* Procedure::eval(State* st, scope::Scope* sc) {
// 	return this;
// }



// // ------------------ String ------------------


// String::String(char* val) {
// 	value = std::string(val);
// 	type = string;
// }

// String::String(const char* val) {
// 	value = std::string(val);
// 	type = string;
// }

// String::String(std::string val) {
// 	value = val;
// 	type = string;
// }
// // strings evaluate to themselves
// Value* String::eval(State* st, scope::Scope* sc) {
// 	return this;
// }
// std::string String::to_string() {
// 	return value;
// }


// #define CHECK_SIG(name) \
// 	bool value::name(Value* val)

// #define VALUE_T_CHECK(name, type) \
// 	CHECK_SIG(name) { return dynamic_cast<type>(val) == NULL; }

// VALUE_T_CHECK(is_number, value::Nil*)
// VALUE_T_CHECK(is_string, value::String*)
// VALUE_T_CHECK(is_list, value::List*)

// CHECK_SIG(is_true) {
// 	// if the dynamic_cast of a value to nil is a NULL ptr,
// 	// it must be true, because it's not a nil
// 	return val->type != nil;
// }

