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

NSCLASS(scope, Scope);
NSCLASS(value, Object); // forward define value

using stringlist = std::vector<std::string>;
typedef std::vector<value::Object> valuelist;


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


	class Object {
		private:
			Object eval_list(State*, scope::Scope*);
			int index = 0;

		public:
			Type type = nil;

			double number;
			std::string ident;
			std::vector<Object> list;
			struct {
				bool special = false;
				std::vector<std::string> args;
				Object* body = NULL;
				specialformfn func;
			} proc;

			/*
			 * Constructors
			 */
			Object();
			~Object();
			Object(double);
			Object(std::string);
			Object(std::vector<Object>);
			Object(std::vector<std::string> names, Object* bod);
			Object(specialformfn);

			std::string to_string();
			Object eval(State*, scope::Scope*);
			Object apply(State*, scope::Scope*, std::vector<Object>);
			bool is_true();
	};
}


#endif
