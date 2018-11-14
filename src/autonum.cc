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
#include <autonum.hh>


autonum::autonum(double v) {
	type = floating;
	nf = v;
}

autonum::autonum(int v) {
	type = integer;
	ni = v;
}
autonum::autonum(int64_t v) {
	type = integer;
	ni = v;
}

#define AUTONUM_OP_BODY(op) \
	if (type == floating) { \
		if (other.type == floating) return nf op other.nf; \
		if (other.type == integer) return nf op other.ni; \
	} \
	if (type == integer) { \
		if (other.type == floating) return ni op other.nf; \
		if (other.type == integer) return ni op other.ni; \
	} \

autonum autonum::operator+(const autonum & other) {
	AUTONUM_OP_BODY(+);
	return autonum(0);
}

autonum autonum::operator-(const autonum & other) {
	AUTONUM_OP_BODY(-);
	return autonum(0);
}

autonum autonum::operator*(const autonum & other) {
	AUTONUM_OP_BODY(*);
	return autonum(0);
}

autonum autonum::operator/(const autonum & other) {
	AUTONUM_OP_BODY(/);
	return autonum(0);
}


autonum autonum::operator+=(const autonum & other) {
	AUTONUM_OP_BODY(+=);
	return autonum(0);
}

autonum autonum::operator-=(const autonum & other) {
	AUTONUM_OP_BODY(-=);
	return autonum(0);
}

autonum autonum::operator*=(const autonum & other) {
	AUTONUM_OP_BODY(*=);
	return autonum(0);
}

autonum autonum::operator/=(const autonum & other) {
	AUTONUM_OP_BODY(/=);
	return autonum(0);
}


bool autonum::operator==(const autonum & other) {
	AUTONUM_OP_BODY(==);
	return false;
}

bool autonum::operator!=(const autonum & other) {
	AUTONUM_OP_BODY(!=);
	return false;
}

bool autonum::operator<(const autonum & other) {
	AUTONUM_OP_BODY(<);
	return false;
}

bool autonum::operator<=(const autonum & other) {
	AUTONUM_OP_BODY(<=);
	return false;
}

bool autonum::operator>(const autonum & other) {
	AUTONUM_OP_BODY(==);
	return false;
}

bool autonum::operator>=(const autonum & other) {
	AUTONUM_OP_BODY(!=);
	return false;
}
#define AUTONUM_CAST_IMPL(t) t autonum::to_##t() { return (type == integer) ? (t)ni : (t)nf; }

AUTONUM_CAST_IMPL(int);
AUTONUM_CAST_IMPL(long);
AUTONUM_CAST_IMPL(float);
AUTONUM_CAST_IMPL(double);

std::string autonum::to_string() {
	if (type == floating) {
		auto str = std::to_string(nf);

		long len = str.length();
		for (int i = len-1; i > 0; i--) {
			if (str[i] == '0') {
				str.pop_back();
			}	else if (str[i] == '.') {
				str.pop_back();
				break;
			} else {
				break;
			}
		}
		return str;
	}
	return std::to_string(ni);
}

std::ostream& operator<<(std::ostream& os, const autonum & num) {
		os << ((autonum&)num).to_string();
    return os;
}
