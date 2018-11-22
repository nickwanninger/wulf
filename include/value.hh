/*
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

#ifndef __VALUE_HH
#define __VALUE_HH

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <config.hh>

#define NSCLASS(ns, name) namespace ns { class name; }
NSCLASS(scope, Scope);
NSCLASS(value, Object); // forward define value
NSCLASS(vm, Machine);
NSCLASS(vm, Bytecode);
class State;

namespace value {

	enum ArgType {
		plain,
		rest,
	};
	class Argument {
		public:
			char* name;
			ArgType type = plain;
	};

	/*
	 * the Type enum is a listing of various types an object can embody
	 */
	enum Type {
		unknown, // 0
		list,    // 1
		ident,   // 2
		string,  // 3
		number,  // 4
		procedure, // 5
		keyword, // 6
		nil,     // 7
		custom,  // 8
	};

	class objref {
		public:
			union {
				value::Object *ptr;
				// value::Object val;
			};

			objref();
			objref(value::Object*);
			~objref();
			value::Object *get() const;
			value::Object *operator->();
			value::Object & operator*();
			objref & operator=(const objref &);
			objref & operator=(value::Object *);

			inline explicit operator bool();
	};

	bool operator==(const objref &, const objref &);
	bool operator!=(const objref &, const objref &);

	// using obj = objref;
	using obj = value::Object*;

	class Object {
		public:
			long refcount;
			Type type = nil;
			/*
			 * a union for the various data types that can be
			 * stored in an Object
			 */
			union {
				double number;
				char* string;

				struct {
					obj car;
					obj cdr;
				};

				/*
				 * a procedure just stores a listing of the bytecode used to
				 * call that procedure. So we need to store the code in the object
				 * and information about argument names.
				 */
				struct {
					vm::Bytecode* code;
					std::vector<Argument> *args;
					scope::Scope* defining_scope;
				};


				struct {
					void *payload;
					const char *type_name;
				};
			};
			/*
			 *
			 */

			Object();
			~Object();
			static obj create(Type);


			obj getptr();


			void write_stream(std::ostream & stream, bool human = false);
			std::string to_string(bool human = false);

			// return if the object is a call to a function name (used in the compiler)
			bool is_call(const char*);
			void compile(vm::Machine*, scope::Scope*, vm::Bytecode*);
			void compile_quasiquote(vm::Machine*, scope::Scope*, vm::Bytecode*);
			void append(value::obj);
			bool is_true();
			size_t length();

			long release();
			long retain();

			bool is_pair();
			obj operator[] (int);
			obj get(int);
	};



	obj copy(obj);

	typedef value::obj (*bind_func_t)(int, value::obj*, State*, scope::Scope*);

	obj newobj(Type);
	obj newobj();
	obj newobj(double);
	obj newobj(char*);
	obj newobj(const char*);

	obj newident(const char*);

	std::vector<Argument>* parse_fn_args(value::obj);
	void argument_scope_expand(std::vector<Argument>, std::vector<value::obj>, scope::Scope*);
}


#endif
