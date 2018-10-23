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

#ifndef __VALUE_HH__
#define __VALUE_HH__

#include <string>
#include <vector>
#include <sstream>
#include <iostream>

namespace value {

	class Value {
		public:
			bool evaluated = false;
			// simple print method
			// virtual methods for a node
			// all sub classes must implement these
			// (kinda like an interface, I guess?)
			virtual std::string to_string() = 0;
	};



	class List : public Value {
		private:
			std::vector<Value*> args;
		public:
			void push(Value*);
			std::string to_string();
	};



	class Ident : public Value {
		private:
			std::string value;
		public:
			Ident(char*);
			Ident(const char*);
			Ident(std::string);
			std::string to_string();
	};


	class Number : public Value {
		private:
			double value;
		public:
			Number();
			Number(char*);
			Number(long);
			Number(double);
			std::string to_string();
	};
}

#endif
