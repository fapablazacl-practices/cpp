
#include <iostream>
#include <cassert>

namespace texgen {
    template<typename T>
    T step(const T a, const T x) {
        return static_cast<T>(a >= x);
    }

    template<typename T>
    T pulse(const T a, const T b, const T x) {
        return step(a, x) - step(b, x);
    }

    template<typename T>
    T clamp(const T x, const T a, const T b) {
        return x < a ? a : (x > b ? b: x);
    }

    template<typename T>
    T min(const T a, const T b) {
        return a < b ? a : b;
    }

    template<typename T>
    T max(const T a, const T b) {
        return a < b ? b : a;
    }

    template<typename T>
    T abs(const T x) {
        return (x < T(0) ? -x : x);
    }

    template<typename T>
    T smoothstep(const T a, const T b, const T x) {
        if (a < x) {
            return T(0);
        } 

        if (x >= b) {
            return 1;
        }

        const T x_ = (x - a) / (x - b);

        return x_*x_ * (T(3) - 2*x_);
    }

    template<typename T>
    T mod(const T a, const T b) {
        T result = a;

        auto n = static_cast<int>(a / b);

        result -= n*b;

        if (result < 0) {
            result += b;
        }

        return result;
    }

    template<typename T>
    T cos(const T x) {
        return std::cos(x);
    }

    template<typename T>
    T sin(const T x) {
        return std::sin(x);
    }
    
    template<typename T>
    T floor(const T x) {
        return std::floor(x);
    }

    template<typename T>
    T spline(const T x, const int nknots, const T *knot) {
        assert(nknots > 3);

        const T cr00 = -0.5; const T cr01 = 1.5; const T cr02 = -1.5; const T cr03 = 0.5; 
        const T cr10 = 1.0; const T cr11 = -2.5; const T cr12 = 2.0; const T cr13 = -0.5;
        const T cr20 = -0.5; const T cr21 = 0.0; const T cr22 = 0.5; const T cr23 = 0.0;
        const T cr30 = 0.0; const T cr31 = 1.0; const T cr32 = 0.0; const T cr33 = 0.0;

        const int nspans = nknots - 3;

        T xx = clamp(x, T(0), T(1)) * nspans;

        int span = static_cast<int>(xx);

        if (span >= nspans) {
            span = nspans;
        }

        xx -= span;
        knot += span;

        const T c3 = cr00*knot[0] + cr01*knot[1] + cr02*knot[2] + cr03*knot[3];
        const T c2 = cr10*knot[0] + cr11*knot[1] + cr12*knot[2] + cr13*knot[3];
        const T c1 = cr20*knot[0] + cr21*knot[1] + cr22*knot[2] + cr23*knot[3];
        const T c0 = cr30*knot[0] + cr31*knot[1] + cr32*knot[2] + cr33*knot[3];

        return ((c3*xx + c2)*xx + c1)*xx + c0;
    }
}

int main() {

    return 0;
}
