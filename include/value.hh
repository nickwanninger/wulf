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

#include <wulf.hh>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <scope.hh>
#include <state.hh>
#include <specialforms.hh>

NSCLASS(scope, Scope)
NSCLASS(value, Value) // forward define value

using stringlist = std::vector<std::string>;
typedef std::vector<value::Value*> valuelist;


namespace value {

	enum Type {
		unknown,
		list,
		ident,
		string,
		number,
		procedure,
		nil,
	};

	class Value {
		public:
			Type type = unknown;
			// simple print method
			// virtual methods for a node
			// all sub classes must implement these
			// (kinda like an interface, I guess?)
			virtual std::string to_string() = 0;
			virtual Value* eval(State*, scope::Scope*);
	};



	class List : public Value {
		public:
			valuelist args;
			void push(Value*);
			Value *operator[](const int index);
			std::string to_string();
			Value* eval(State*, scope::Scope*);
	};



	class Ident : public Value {
		private:
			std::string value;
		public:
			Ident(char*);
			Ident(const char*);
			Ident(std::string);
			std::string to_string();
			Value* eval(State*, scope::Scope*);
	};

	class Number : public Value {
		public:
			double value;
			Number();
			Number(char*);
			Number(long);
			Number(double);
			std::string to_string();
			Value* eval(State*, scope::Scope*);
	};

	class Nil : public Value {
		public:
			Nil();
			std::string to_string();
			Value* eval(State*, scope::Scope*);
	};


	class Procedure : public Value {
		specialformfn cfunc;
		bool is_special_form = false;
		public:
			Procedure(specialformfn);
			Procedure(stringlist, Value*);
			stringlist args;
			Value* body;
			std::string to_string();
			Value* apply(State*, scope::Scope*, valuelist);
			Value* eval(State*, scope::Scope*);
	};


	class String : public Value {
		private:
			std::string value;
		public:
			String(char*);
			String(const char*);
			String(std::string);
			std::string to_string();
			Value* eval(State*, scope::Scope*);
	};


	/*
	 * helper functions
	 */
	#define CHECK_DECL(name) bool name(Value*)
	CHECK_DECL(is_true);
	CHECK_DECL(is_number);
	CHECK_DECL(is_string);
	CHECK_DECL(is_list);
}


#endif
