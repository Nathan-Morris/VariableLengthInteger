#include "varint.h"

#include <chrono>

using namespace std;

static bigint divd(
	bigint& _u,
	bigint& _v
) {
	bigint u(_u);
	u.resize(_u.size() + _v.size() + 1);
	bigint v(_v);

    bigint q;
    q.resize(_u.size() - _v.size() + 1);

    const uint64_t b = 4294967296LL;
    uint64_t qhat;                  
    uint64_t rhat;                  
    uint64_t p;                     
    int64_t t, k;
    int s, i, j;

    if (_u.size() < _v.size() || _v.size() <= 1 || _v[_v.size() - 1] == 0)
        return 1;

    s = __lzcnt(_v[_v.size() - 1]);
    v.resize(4 * _v.size());
    for (i = _v.size() - 1; i > 0; i--)
        v[i] = (_v[i] << s) | ((uint64_t)_v[i - 1] >> (32 - s));
    v[0] = _v[0] << s;

    u.resize(4 * (_u.size() + 1));
    u[_u.size()] = (uint64_t)_u[_u.size() - 1] >> (32 - s);
    for (i = _u.size() - 1; i > 0; i--)
        u[i] = (_u[i] << s) | ((uint64_t)_u[i - 1] >> (32 - s));
    u[0] = _u[0] << s;

    for (j = _u.size() - _v.size(); j >= 0; j--) { 
        qhat = (u[j + _v.size()] * b + u[j + _v.size() - 1]) / v[_v.size() - 1];
        rhat = (u[j + _v.size()] * b + u[j + _v.size() - 1]) % v[_v.size() - 1];
    again:
        if (qhat >= b || (uint32_t)qhat * (uint64_t)v[_v.size() - 2] > b * rhat + u[j + _v.size() - 2]) {
            qhat = qhat - 1;
            rhat = rhat + v[_v.size() - 1];
            if (rhat < b) goto again;
        }

        k = 0;
        for (i = 0; i < _v.size(); i++) {
            p = (uint32_t)qhat * (uint64_t)v[i];
            t = u[i + j] - k - (p & 0xFFFFFFFFLL);
            u[i + j] = t;
            k = (p >> 32) - (t >> 32);
        }
        t = u[j + _v.size()] - k;
        u[j + _v.size()] = t;

        q[j] = qhat;         
        if (t < 0) {         
            q[j] = q[j] - 1; 
            k = 0;
            for (i = 0; i < _v.size(); i++) {
                t = (uint64_t)u[i + j] + v[i] + k;
                u[i + j] = t;
                k = t >> 32;
            }
            u[j + _v.size()] = u[j + _v.size()] + k;
        }
    }

    return q;
}

int main() {
    srand(time(NULL));

	bigint a = rand64();
    bigint b;
    b.push_back(rand32());
    b.push_back(0);

	float sum = 0.f;
	float count = 10000;

    cout << a << endl;
    cout << b << endl;
    cout << (a / b) << endl << endl;

    return -1;

	for (size_t i = 0; i <= count; i++) {
        a = (1ull << 62);
        b = (1ull << 32);
		auto pre = chrono::high_resolution_clock::now();
		a /= b;
		sum += chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now() - pre).count();
	}
	
	cout << a << endl;

	cout << (sum / count) << "us Avrg Call Time" << endl;
}