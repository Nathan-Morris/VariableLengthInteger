#include <iostream>
#include <algorithm>
#include <vector>
#include <bitset>

#ifndef MIN
#	define	MIN(_a, _b)	((_a) < (_b) ? (_a) : (_b))
#endif	

#pragma once

class bigint : private std::vector<unsigned int>
{
public:
	bigint(unsigned long long ull);
	bigint(long long ll);


	bigint(const bigint& n);

public:
	unsigned int* ptr();
	const unsigned int* ptr() const;

public:
	bigint& operator+=(const bigint& a);
	bigint& operator-=(const bigint& a);
	bigint& operator*=(const bigint& a);
	bigint& operator/=(const bigint& a);
	bigint& operator&=(const bigint& a);
	bigint& operator|=(const bigint& a);
	bigint& operator^=(const bigint& a);
	bigint& operator>>=(size_t n);
	bigint& operator<<=(size_t n);
	bigint operator~() const;

public:
	friend bigint operator+(const bigint& a, const bigint& b);
	friend bigint operator-(const bigint& a, const bigint& b);
	friend bigint operator*(const bigint& a, const bigint& b);
	friend bigint operator/(const bigint& a, const bigint& b);

	friend bigint operator^(const bigint& a, const bigint& b);
	friend bigint operator&(const bigint& a, const bigint& b);
	friend bigint operator|(const bigint& a, const bigint& b);

	inline friend std::ostream& operator<<(std::ostream& out, const bigint& a) {
		for (size_t i = 0; i != a.size(); i++) {
			out << std::bitset<32>(a.at(i)) << ' ';
		}
		return out;
	}
};

