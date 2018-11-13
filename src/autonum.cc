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

autonum::autonum(long long v) {
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
	return 0ll; \

autonum autonum::operator+(const autonum & other) {
	AUTONUM_OP_BODY(+);
}

autonum autonum::operator-(const autonum & other) {
	AUTONUM_OP_BODY(-);
}

autonum autonum::operator*(const autonum & other) {
	AUTONUM_OP_BODY(*);
}

autonum autonum::operator/(const autonum & other) {
	AUTONUM_OP_BODY(/);
}


autonum autonum::operator+=(const autonum & other) {
	AUTONUM_OP_BODY(+=);
}

autonum autonum::operator-=(const autonum & other) {
	AUTONUM_OP_BODY(-=);
}

autonum autonum::operator*=(const autonum & other) {
	AUTONUM_OP_BODY(*=);
}

autonum autonum::operator/=(const autonum & other) {
	AUTONUM_OP_BODY(/=);
}


bool autonum::operator==(const autonum & other) {
	AUTONUM_OP_BODY(==);
}

bool autonum::operator!=(const autonum & other) {
	AUTONUM_OP_BODY(!=);
}

bool autonum::operator<(const autonum & other) {
	AUTONUM_OP_BODY(<);
}

bool autonum::operator<=(const autonum & other) {
	AUTONUM_OP_BODY(<=);
}

bool autonum::operator>(const autonum & other) {
	AUTONUM_OP_BODY(==);
}

bool autonum::operator>=(const autonum & other) {
	AUTONUM_OP_BODY(!=);
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
