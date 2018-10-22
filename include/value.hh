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

namespace value {
	/*
	 * Value is a class that other classes will
	 * extend. When they do such, the derived classes
	 * will be wulf value types. (ie: Number, String, List)
	 */
	class Value {
	public:

		Value();
		virtual Value operator+(const Value&) const;
		virtual Value operator-(const Value&) const;
		virtual Value operator*(const Value&) const;
		virtual Value operator/(const Value&) const;
		virtual Value operator>(const Value&) const;
		virtual Value operator<(const Value&) const;

		virtual std::string to_string();
	};

}


#endif
