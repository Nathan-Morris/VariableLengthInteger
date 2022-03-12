#include "varint.h"

bigint::bigint(unsigned long long ull) {
	this->resize(2);
	*(unsigned long long*)this->ptr() = ull;
}

bigint::bigint(long long ll) {
	this->resize(2);
	*(long long*)this->ptr() = ll;
}

bigint::bigint(const bigint& n) {
	this->resize(n.size());
	std::copy(n.begin(), n.end(), this->begin());
}

unsigned int* bigint::ptr() { return this->data(); }

const unsigned int* bigint::ptr() const { return this->data(); }

bigint& bigint::operator+=(const bigint& a) {
	if (a.size() > this->size()) {
		this->resize(a.size());
	}
	
	unsigned long long carrySum;
	
	for (size_t i = 0; i != this->size(); i++) {
		carrySum = (unsigned long long)this->at(i) + (unsigned long long)a.at(i);
		this->at(i) = (unsigned int)(carrySum & 0xFFFFFFFF);
		carrySum >>= 32;
	}

	return *this;
}

bigint& bigint::operator-=(const bigint& a) {
	return (*this += (~a - 1ull));
}

bigint& bigint::operator*=(const bigint& a) {
	return *this;
}

bigint& bigint::operator/=(const bigint& a) {
	return *this;
}

bigint& bigint::operator&=(const bigint& a) {
	for (size_t i = 0; i != MIN(a.size(), this->size()); i++) {
		this->at(i) &= a.at(i);
	}
	return *this;
}

bigint& bigint::operator|=(const bigint& a) {
	for (size_t i = 0; i != MIN(a.size(), this->size()); i++) {
		this->at(i) |= a.at(i);
	}
	return *this;
}

bigint& bigint::operator^=(const bigint& a) {
	for (size_t i = 0; i != MIN(a.size(), this->size()); i++) {
		this->at(i) ^= a.at(i);
	}
	return *this;
}

bigint& bigint::operator>>=(size_t bitShift) {
	// shift 32 bit elements in array to the right
	if (bitShift >= 32) {
		std::rotate(this->rbegin(), this->rbegin() + (bitShift / 32), this->rend());
	}

	bitShift &= 31; // modulo 32

	if (bitShift) {
		unsigned int carry = 0;
		unsigned int store = 0;
		for (size_t i = 0; i != this->size(); i++) {
			store = this->at(i);
			this->at(i) = carry & (store >> bitShift);
			carry = store << (32 - bitShift);
		}
	}

	return* this;
}

bigint& bigint::operator<<=(size_t n) {
	return *this;
}

bigint bigint::operator~() const {
	bigint a(*this);
	for (unsigned int& i : a) {
		i = ~i;
	}
	return a;
}

//
//
//

bigint operator+(const bigint& a, const bigint& b) {
	bigint c(a);
	c += b;
	return a;
}

bigint operator-(const bigint& a, const bigint& b) {
	bigint c(a);
	c -= b;
	return a;
}

bigint operator*(const bigint& a, const bigint& b) {
	bigint c(a);
	c *= b;
	return a;
}

bigint operator/(const bigint& a, const bigint& b) {
	bigint c(a);
	c /= b;
	return a;
}

bigint operator^(const bigint& a, const bigint& b) {
	bigint c(a);
	c ^= b;
	return a;
}

bigint operator&(const bigint& a, const bigint& b) {
	bigint c(a);
	c &= b;
	return a;
}

bigint operator|(const bigint& a, const bigint& b) {
	bigint c(a);
	c |= b;
	return a;
}
