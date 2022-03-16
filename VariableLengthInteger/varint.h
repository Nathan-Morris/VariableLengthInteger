#include <iostream>
#include <algorithm>
#include <vector>
#include <bitset>
#include <type_traits>


#if defined(_MSC_VER) || defined(__clang__)
#	define nlz	__lzcnt
#elif defined(__GNUC__)
#	define nlz	__builtin_ia32_lzcnt_u32
#endif


#ifndef MIN
#	define	MIN(_a, _b)	((_a) < (_b) ? (_a) : (_b))
#endif // !MIN


#ifndef MAX
#	define	MAX(_a, _b)	((_a) > (_b) ? (_a) : (_b))
#endif // !MAX

#define rand32()	((rand() << 16) | rand())
#define rand64()	(((uint64_t)rand() << 48) | ((uint64_t)rand() << 32) | ((uint64_t)rand() << 16) | (uint64_t)rand())
#define randrange(_min, _max)	((_min) + (rand() % ((_max) - (_min) + 1)))	


#pragma once

//
//
//

template<typename IntType, const bool isArithmetic = std::is_arithmetic_v<IntType>>
static uint16_t numberOfTrailingZeros(IntType j) {
	static_assert(isArithmetic, "IntType Must be Arithmetic Type");
	if (j == 0) {
		return 0;
	}
	for (uint16_t i = 0; i != sizeof(IntType) * 8; i++) {
		if (j & ((IntType)1 << i)) {
			return i;
		}
	}
}

//
//
//

class bigint : public /*private*/ std::vector<uint32_t>
{
private:
	bool mNegative = false;

private:
	void stripTrailing0s();
public:
	size_t lowestSetBitPos() const;

public:
	bigint();

	bigint(uint64_t n);
	bigint(int64_t n);

	bigint(uint32_t n);
	bigint(int n);

	bigint(uint16_t n);
	bigint(short n);

	bigint(uint8_t n);
	bigint(char n);

	bigint(const bigint& n);

public:
	uint32_t* ptr();
	const uint32_t* ptr() const;

	bool isEven() const;
	bool isOdd() const;

	char cmp(const bigint& n) const;

	std::string toString() const;

public:
	template<typename IntType, const bool isArithmeticType = std::is_arithmetic_v<IntType>>
	IntType as() {
		static_assert(isArithmeticType, "Only Able To Interpret Bigint as Arithmetic Type");
		uint8_t ret[sizeof(IntType)] = { 0 };
		for (size_t i = 0; i != MIN(sizeof(IntType), this->size() * 4); i++) {
			ret[i] = ((uint8_t*)this->data())[i];
		}
		return *(IntType*)ret;
	}

public:
	bigint& operator+=(const bigint& a);
	bigint& operator-=(const bigint& a);
	bigint& operator*=(const bigint& a);
	bigint& operator/=(const bigint& a);
	bigint& operator%=(const bigint& a);

	bigint& operator&=(const bigint& a);
	bigint& operator|=(const bigint& a);
	bigint& operator^=(const bigint& a);

	bigint& operator>>=(size_t n);
	bigint& operator<<=(size_t n);
	
	bigint& operator=(const bigint& a);
	bigint operator~() const;

	bigint& operator++();
	bigint& operator--();

public:
	friend bool operator==(const bigint& a, const bigint& b);
	friend bool operator>=(const bigint& a, const bigint& b);
	friend bool operator<=(const bigint& a, const bigint& b);
	friend bool operator>(const bigint& a, const bigint& b);
	friend bool operator<(const bigint& a, const bigint& b);
	friend bool operator!=(const bigint& a, const bigint& b);

public:
	friend bigint operator+(const bigint& a, const bigint& b);
	friend bigint operator-(const bigint& a, const bigint& b);
	friend bigint operator*(const bigint& a, const bigint& b);
	friend bigint operator/(const bigint& a, const bigint& b);
	friend bigint operator%(const bigint& a, const bigint& b);

	friend bigint operator^(const bigint& a, const bigint& b);
	friend bigint operator&(const bigint& a, const bigint& b);
	friend bigint operator|(const bigint& a, const bigint& b);

	friend bigint operator<<(const bigint& a, size_t b);
	friend bigint operator>>(const bigint& a, size_t b);


	inline friend std::ostream& operator<<(std::ostream& out, const bigint& a) {
		for (size_t i = a.size() - 1; i != SIZE_MAX; i--) {
			out << std::bitset<32>(a.at(i)) << ' ';
		}
		return out;
	}
};

