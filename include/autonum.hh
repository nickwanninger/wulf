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
#ifndef __AUTNUM_HH
#define __AUTNUM_HH

#include <stdint.h>
#include <iostream>
#include <string>

enum autonum_type {
	floating,
	integer,
};


#define AUTONUM_CAST_HEADER(t) t to_##t()

class autonum {

	private:
		void simplify(void);
		uint64_t compare(const autonum & other) const;
	public:
		autonum_type type = floating;
		union {
			double flt;
			struct {
				int64_t num;
				int64_t den;
			};
		};
		autonum(double);
		autonum(int);
		autonum(int64_t);
		autonum(int64_t, int64_t);

		autonum & operator=(const autonum & other);
		autonum operator+(const autonum & other);
		autonum operator-(const autonum & other);
		autonum operator*(const autonum & other);
		autonum operator/(const autonum & other);

		autonum operator+=(const autonum & other);
		autonum operator-=(const autonum & other);
		autonum operator*=(const autonum & other);
		autonum operator/=(const autonum & other);

		bool operator==(const autonum & other);
		bool operator!=(const autonum & other);
		bool operator<(const autonum & other);
		bool operator<=(const autonum & other);
		bool operator>(const autonum & other);
		bool operator>=(const autonum & other);

		uint64_t to_int() const;
		double to_double() const;

		std::string to_string();
};

std::ostream& operator<<(std::ostream& os, const autonum & num);

#endif
