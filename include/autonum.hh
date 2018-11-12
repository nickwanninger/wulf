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
	public:
		autonum_type type = floating;
		union {
			double nf;
			int64_t ni;
		};
		autonum(double);
		autonum(int);
		autonum(int64_t);
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

		AUTONUM_CAST_HEADER(int);
		AUTONUM_CAST_HEADER(long);
		AUTONUM_CAST_HEADER(float);
		AUTONUM_CAST_HEADER(double);

		std::string to_string();
};

std::ostream& operator<<(std::ostream& os, const autonum & num);

#endif
