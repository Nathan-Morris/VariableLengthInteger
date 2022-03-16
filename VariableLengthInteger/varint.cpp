#include "varint.h"

void bigint::stripTrailing0s() {
	for (size_t i = this->size() - 1; i != SIZE_MAX; i--) {
		if (this->at(i)) {
			this->erase(this->begin() + i + 1, this->end());
			break;
		}
	}
}

size_t bigint::lowestSetBitPos() const {
	// no bit set
	if (!this->size()) {
		return SIZE_MAX;
	}

	size_t i = this->size() - 1;

	for (; i > 0 && !this->at(i); i--);
	
	uint32_t b = this->at(i);

	// no bit set
	if (!b) {
		return SIZE_MAX;
	}

	return (i * sizeof(uint32_t) * 8) + numberOfTrailingZeros<uint32_t>(b);
}

//
//
//

bigint::bigint() { }

bigint::bigint(uint64_t ull) {
	this->push_back((uint32_t)(ull & 0xFFFFFFFFull));
	if ((ull >>= 32)) {
		this->push_back(ull);
	}
}

bigint::bigint(int64_t ll) {
	if ((this->mNegative = (ll & 0x8000000000000000))) {
		ll ^= 0x8000000000000000;
	}
	this->push_back((uint32_t)((uint64_t)ll & 0xFFFFFFFFull));
	if ((ll >>= 32)) {
		this->push_back(ll);
	}
}

bigint::bigint(const bigint& n) {
	this->resize(n.size());
	std::copy(n.begin(), n.end(), this->begin());
}

bigint::bigint(uint32_t n) {
	this->push_back(n);
}

bigint::bigint(int n) {
	if ((this->mNegative = (n & 0x80000000))) {
		n ^= 0x80000000;
	}
	this->push_back(n);
}

bigint::bigint(uint16_t n) : bigint((uint32_t)n) { }
bigint::bigint(short n) : bigint((int)n) { }

bigint::bigint(uint8_t n) : bigint((uint32_t)n) { }
bigint::bigint(char n) : bigint((int)n) { }

//
//
//

uint32_t* bigint::ptr() { return this->data(); }
const uint32_t* bigint::ptr() const { return this->data(); }

bool bigint::isEven() const { return !this->isOdd(); }
bool bigint::isOdd() const { return this->data()[0] & 1; }

char bigint::cmp(const bigint& n) const {
	if (this->size() != n.size()) {
		return (this->size() > n.size()) ? 1 : -1;
	}
	for (size_t i = this->size() - 1; i != SIZE_MAX; i--) {
		if (this->at(i) != n.at(i)) {
			return (this->at(i) > n.at(i)) ? 1 : -1;
		}
	}
	return 0;
}

std::string bigint::toString() const {
	std::string buf;

	return buf;
}

//
//
//

bigint& bigint::operator+=(const bigint& a) {
	size_t minSz = MIN(this->size(), a.size());

	if (!minSz) { return *this; }

	uint64_t carry = 0;
	size_t i;
	
	for (i = 0; i != minSz; i++) {
		carry += (uint64_t)this->at(i) + (uint64_t)a.at(i);
		this->at(i) = carry & 0xFFFFFFFFull;
		carry >>= 32;
	}

	if (carry) {
		if (i >= this->size()) {
			this->push_back(carry);
		}
		else {
			this->at(i++) = carry & 0xFFFFFFFFull;
		}
	}

	return *this;
}

bigint& bigint::operator-=(const bigint& a) {
	size_t minSz = MIN(this->size(), a.size());

	if (!minSz) { return *this; }

	uint32_t carry = 0;
	for (size_t i = 0; i != minSz; i++) {
		uint32_t b = a.at(i);
		b += carry;
		carry = (b ^ 0x80000000) < (carry ^ 0x80000000) ? 1 : 0;
		b = this->at(i) - b;
		carry = (b ^ 0x80000000) > (this->at(i) ^ 0x80000000) ? 1 : 0;
		this->at(i) = b;
	}

	return *this;
}

bigint& bigint::operator*=(const bigint& y) {
	bigint x(*this);

	this->resize(x.size() + y.size());

	uint64_t carry = 0;
	for (size_t j = 0; j < x.size(); j++)
	{
		carry += (uint64_t)x.at(j) * y[0];
		this->at(j) = (uint32_t)carry;
		carry >>= 32;
	}

	this->at(x.size()) = carry;

	for (size_t i = 1; i < y.size(); i++)
	{
		uint64_t carry = 0;
		for (size_t j = 0; j < x.size(); j++)
		{
			carry += ((uint64_t)x.at(j)) * y[i] + ((uint64_t)this->at(i + j));
			this->at(i + j) = (uint32_t)carry;
			carry >>= 32;
		}
		this->at(i + x.size()) = (uint32_t)carry;
	}

	this->stripTrailing0s();

	return *this;
}

bigint& bigint::operator/=(const bigint& n) {
	bigint u(*this);
	u.resize(this->size() + n.size() + 1);
	bigint v(n);

	bigint q;
	q.resize(this->size() - n.size() + 1);

	const uint64_t b = 4294967296LL;
	uint64_t qhat;
	uint64_t rhat;
	uint64_t p;
	int64_t t, k;
	int s, i, j;

	if (this->size() < n.size() || n.size() <= 1 || n[n.size() - 1] == 0)
		return (*this = 1);

	s = nlz(n[n.size() - 1]);
	v.resize(4 * n.size());
	for (i = n.size() - 1; i > 0; i--)
		v[i] = (n[i] << s) | ((uint64_t)n[i - 1] >> (32 - s));
	v[0] = n[0] << s;

	u.resize(4 * (this->size() + 1));
	u[this->size()] = (uint64_t)(*this)[this->size() - 1] >> (32 - s);
	for (i = this->size() - 1; i > 0; i--)
		u[i] = ((*this)[i] << s) | ((uint64_t)(*this)[i - 1] >> (32 - s));
	u[0] = (*this)[0] << s;

	for (j = this->size() - n.size(); j >= 0; j--) {
		qhat = (u[j + n.size()] * b + u[j + n.size() - 1]) / v[n.size() - 1];
		rhat = (u[j + n.size()] * b + u[j + n.size() - 1]) % v[n.size() - 1];
	again:
		if (qhat >= b || (uint32_t)qhat * (uint64_t)v[n.size() - 2] > b * rhat + u[j + n.size() - 2]) {
			qhat = qhat - 1;
			rhat = rhat + v[n.size() - 1];
			if (rhat < b) goto again;
		}

		k = 0;
		for (i = 0; i < n.size(); i++) {
			p = (uint32_t)qhat * (uint64_t)v[i];
			t = u[i + j] - k - (p & 0xFFFFFFFFLL);
			u[i + j] = t;
			k = (p >> 32) - (t >> 32);
		}
		t = u[j + n.size()] - k;
		u[j + n.size()] = t;

		q[j] = qhat;
		if (t < 0) {
			q[j] = q[j] - 1;
			k = 0;
			for (i = 0; i < n.size(); i++) {
				t = (uint64_t)u[i + j] + v[i] + k;
				u[i + j] = t;
				k = t >> 32;
			}
			u[j + n.size()] = u[j + n.size()] + k;
		}
	}

	*this = q;
	this->stripTrailing0s();
	return *this;
}

bigint& bigint::operator&=(const bigint& a) {
	for (size_t i = 0; i != MIN(a.size(), this->size()); i++) {
		this->at(i) &= a.at(i);
	}
	return *this;
}

bigint& bigint::operator%=(const bigint& a) {

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
		this->erase(this->begin(), this->begin() + MIN((bitShift / 32), (this->size() - 1)));
		bitShift &= 31; // modulo 32
	}

	if (bitShift && this->size()) {
		uint32_t carryPrev = 0;
		uint32_t carry = 0;
		for (size_t i = this->size() - 1; i != SIZE_MAX; i--) {
			carry = this->at(i) << (32 - bitShift);
			this->at(i) >>= bitShift;
			this->at(i) |= carryPrev;
			carryPrev = carry;
		}
	}

	this->stripTrailing0s();

	return* this;
}

// 2 reallocations of vector
// high const method
bigint& bigint::operator<<=(size_t bitShift) {
	if (bitShift >= 32) {
		size_t elemShift = bitShift / 32;
		this->reserve(this->size() + elemShift);
		this->insert(this->begin(), (bitShift / 32), 0u);
		bitShift &= 31; // modulo 32
	}

	//std::cout << bitShift << std::endl;

	if (bitShift) {
		uint32_t carryPrev = 0;
		uint32_t carry = 0;
		size_t i;
		for (i = 0; i != this->size(); i++) {
			carry = this->data()[i] >> (32 - bitShift);
			this->data()[i] = (this->data()[i] << bitShift) | carryPrev;
			carryPrev = carry;
		}
		if (carryPrev) {
			this->push_back(carryPrev);
		}
	}

	//if (bitShift) {
	//	uint32_t carryPrev = 0;
	//	uint32_t carry = 0;
	//	for (size_t i = 0; i != this->size(); i++) {
	//		carry = this->at(i) >> (32 - bitShift);
	//		this->at(i) <<= bitShift;
	//		this->at(i) |= carryPrev;
	//		carryPrev = carry;
	//	}
	//	if (carryPrev) {
	//		this->push_back(carryPrev);
	//	}
	//}

	return *this;
}

bigint& bigint::operator=(const bigint& a) {
	this->resize(a.size());
	std::copy(a.begin(), a.end(), this->begin());
	return *this;
}

//
//
//

bigint bigint::operator~() const {
	bigint a(*this);
	for (uint32_t& i : a) {
		i = ~i;
	}
	return a;
}

bigint& bigint::operator++() {
	if (this->at(0) & 1) {
		this->operator+=(1);
	}
	else {
		this->at(0) |= 1;
	}
	return *this;
}

bigint& bigint::operator--() {
	if (this->at(0) & 1) {
		this->at(0) ^= 1;
	}
	else {
		this->operator-=(1);
	}
	return *this;
}

//
//
//

bool operator==(const bigint& a, const bigint& b) {
	return !(a != b);
}

bool operator>=(const bigint& a, const bigint& b) {
	return a.cmp(b) >= 0;
}

bool operator<=(const bigint& a, const bigint& b) {
	return a.cmp(b) <= 0;
}

bool operator>(const bigint& a, const bigint& b) {
	return a.cmp(b) > 0;
}

bool operator<(const bigint& a, const bigint& b) {
	return a.cmp(b) < 0;
}

bool operator!=(const bigint& a, const bigint& b) {
	return a.cmp(b);
}


//
//
//

bigint operator+(const bigint& a, const bigint& b) {
	bigint c(a);
	return (c += b);
}

bigint operator-(const bigint& a, const bigint& b) {
	bigint c(a);
	return (c -= b);
}

bigint operator*(const bigint& a, const bigint& b) {
	bigint c(a);
	return (c *= b);
}

bigint operator/(const bigint& a, const bigint& b) {
	bigint c(a);
	return (c /= b);
}

bigint operator%(const bigint& a, const bigint& b) {
	bigint c(a);
	return (c %= b);
}

bigint operator^(const bigint& a, const bigint& b) {
	bigint c(a);
	return (c ^= b);
}

bigint operator&(const bigint& a, const bigint& b) {
	bigint c(a);
	return (c &= b);
}

bigint operator|(const bigint& a, const bigint& b) {
	bigint c(a);
	return (c |= b);
}

bigint operator<<(const bigint& a, size_t b) {
	bigint c(a);
	return (c <<= b);
}

bigint operator>>(const bigint& a, size_t b) {
	bigint c(a);
	return (c >>= b);
}