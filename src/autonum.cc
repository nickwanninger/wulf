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
	flt = v;
}

autonum::autonum(int v) {
	type = integer;
	num = v;
	den = 1;
}

autonum::autonum(int64_t v) {
	type = integer;
	num = v;
	den = 1;
}
autonum::autonum(int64_t n, int64_t d) {
	type = integer;
	num = n;
	den = d;
	simplify();
}



void autonum::simplify(void) {
	int64_t rn, rd, gcd;
	if (num < den) {
		gcd = num;
	} else {
		gcd = den;
	}
	if (num == 0 || den == 0) {
		rn = 0;
		rd = 0;
	} else {
		while (gcd > 1) {
			if (num % gcd == 0 && den % gcd == 0)
				break;
			gcd--;
		}
		rn = num / gcd;
		rd = den / gcd;
	}

	//num = rn;
	//den = rd;
}

#define AUTONUM_OP_BODY(op) \
	if (type == floating) { \
		if (other.type == floating) return flt op other.flt; \
		if (other.type == integer) return flt op other.to_double(); \
	} \
	if (type == integer) { \
		if (other.type == floating) return flt op other.flt; \
		if (other.type == integer) return num op other.num; \
	} \



autonum & autonum::operator=(const autonum & b) {
	return *this;
}

autonum autonum::operator+(const autonum & b) {
	// if either of them is a float/double, return them both as doubles
	if (type == floating || b.type == floating) return to_double() + b.to_double();
	if (den == 1 && den == b.den) return num + b.num;
	autonum a = *this;
	autonum res = 1;
	res.num = a.num * b.den + b.num * a.den;
	res.den = a.den * b.den;
	res.simplify();
	return res;
}

autonum autonum::operator-(const autonum & b) {
	// if either of them is a float/double, return them both as doubles
	if (type == floating || b.type == floating) return to_double() - b.to_double();
	if (den == 1 && den == b.den) return num - b.num;
	autonum a = *this;
	autonum res = 1;
	res.num = a.num * b.den - b.num * a.den;
	res.den = a.den * b.den;
	res.simplify();
	return res;
}


autonum autonum::operator*(const autonum & b) {
	// if either of them is a float/double, return them both as doubles
	if (type == floating || b.type == floating) return to_double() * b.to_double();
	if (den == 1 && den == b.den) return num * b.num;
	autonum a = *this;
	autonum res = 1;
	res.num = a.num * b.num;
	res.den = a.den * b.den;
	res.simplify();
	return res;
}

autonum autonum::operator/(const autonum & b) {
	// if either of them is a float/double, return them both as doubles
	if (type == floating || b.type == floating) return to_double() * b.to_double();
	autonum a = *this;
	autonum res = 1;
	res.num = a.num * b.den;
	res.den = a.den * b.num;
	res.simplify();
	return res;
}


autonum autonum::operator+=(const autonum & other) {
	*this = *this + other;
	return *this;
}

autonum autonum::operator-=(const autonum & other) {
	*this = *this - other;
	return *this;
}

autonum autonum::operator*=(const autonum & other) {
	*this = *this * other;
	return *this;
}

autonum autonum::operator/=(const autonum & other) {
	*this = *this / other;
	return *this;
}


bool autonum::operator==(const autonum & b) {
	// if either of them is a float/double, return them both as doubles
	if (type == floating || b.type == floating) return to_double() == b.to_double();
	autonum a = *this;
	return a.num == b.num && a.den == b.den;
}

bool autonum::operator!=(const autonum & other) {
	return !(*this == other);
}

bool autonum::operator<(const autonum & other) {
	return compare(other) < 0;
}

bool autonum::operator<=(const autonum & other) {
	return compare(other) <= 0;
}

bool autonum::operator>(const autonum & other) {
	return compare(other) > 0;
}

bool autonum::operator>=(const autonum & other) {
	return compare(other) >= 0;
}
// compare two fractions
uint64_t autonum::compare(const autonum & r) const {
	return to_double() - r.to_double();
}


uint64_t autonum::to_int() const {
	return (uint64_t)to_double();
}


double autonum::to_double() const {
	if (type == integer) {
		return (double)num / (double)den;
	} else {
		return flt;
	}
}

std::string autonum::to_string() {
	if (type == floating) {
		auto str = std::to_string(flt);

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

	std::string res;
	res += std::to_string(num);
	if (den != 1) {
		res += "/";
		res += std::to_string(den);
	}
	return res;
}

std::ostream& operator<<(std::ostream& os, const autonum & num) {
		os << ((autonum&)num).to_string();
    return os;
}
