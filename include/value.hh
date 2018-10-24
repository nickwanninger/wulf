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
#include <scope.hh>
#include <state.hh>

namespace scope {
	class Scope;
}

namespace value {

	enum Type {
		unknown,
		list,
		ident,
		number,
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
		private:
			std::vector<Value*> args;
		public:
			Type type = list;
			void push(Value*);
			Value *operator[](const int index);
			std::string to_string();
			Value* eval(State*, scope::Scope*);
	};



	class Ident : public Value {
		private:
			std::string value;
		public:
			Type type = ident;
			Ident(char*);
			Ident(const char*);
			Ident(std::string);
			std::string to_string();
			Value* eval(State*, scope::Scope*);
	};



	class Number : public Value {
		private:
			double value;
		public:
			Type type = number;
			Number();
			Number(char*);
			Number(long);
			Number(double);
			std::string to_string();
			Value* eval(State*, scope::Scope*);
	};

	class Nil : public Value {
		public:
			Type type = nil;
			Nil();
			std::string to_string();
			Value* eval(State*, scope::Scope*);
	};
}


#endif
