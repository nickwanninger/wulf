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

#include <ast.hh>
#include <limits>

using namespace ast;


Number::Number(char* val) {
	value = std::atof(val);
}


Number::Number(long val) {
	value = (double)val;
}

Number::Number(double val) {
	value = val;
}

// default constructor (sets to zero)
Number::Number() {
	value = 0;
}


std::string Number::to_string() {
	std::ostringstream os;
	os.precision(std::numeric_limits<double>::max_digits10);
	os << value;
	return os.str();
}


value::Value Number::eval() {
	value::Value v;
	return v;
}
