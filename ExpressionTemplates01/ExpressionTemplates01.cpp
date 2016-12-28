
#include <iostream>
#include <cstddef>
#include <functional>

template<typename T, size_t C>
class DotProduct {
public:
    static T evaluate(const T *array1, const T *array2) {
        const T first = DotProduct<T, 1>::evaluate(array1, array2);
        const T remaining = DotProduct<T, C - 1>::evaluate(array1 + 1, array2 + 1);

        return first + remaining;
    }
};

// dot product core
template<typename T>
class DotProduct<T, 1> {
public:
    static T evaluate(const T *array1, const T *array2) {
        return array1[0] * array2[0];
    }
};

namespace lazy {
    template<typename T, size_t C> 
    inline T dot(const T* array1, const T* array2) {
        return DotProduct<T, C>::evaluate(array1, array2);
    }
}


// arithmetic expressions

// Literal (AKA constant)
class Literal {
public:
    Literal(const double value) 
        : m_value(value) {}
    
    double evaluate(double) const {
        return m_value;
    }

private:
    const double m_value;
};

template<typename T>
class Identity {
public:
    T evaluate(T value) const {
        return value;
    }
};

// expression traits
template<typename Expression> struct ExpressionTraits {
    typedef Expression expression_type;
};

template<> struct ExpressionTraits<double> {
    typedef Literal expression_type;
};

template<> struct ExpressionTraits<int> {
    typedef Literal expression_type;
};

template<> struct ExpressionTraits<float> {
    typedef Literal expression_type;
};

// expressions
template<typename Expression, typename UnaryOperation>
class UnaryExpression {
public:
    UnaryExpression(Expression e, UnaryOperation op=UnaryOperation()) 
        : m_e(e), m_op(op) {}

    double evaluate(double value) const {
        return m_op(m_e.evaluate(value));
    }

private:
    Expression m_e;
    UnaryOperation m_op;
};

template<typename Expression1, typename Expression2, typename BinaryOperation>
class BinaryExpression {
public:
    BinaryExpression(Expression1 e1, Expression2 e2, BinaryOperation op=BinaryOperation()) 
        : m_e1(e1), m_e2(e2), m_op(op) {}

    double evaluate(double value) const {
        return m_op(m_e1.evaluate(value), m_e2.evaluate(value));
    }

private:
    typename ExpressionTraits<Expression1>::expression_type m_e1;
    typename ExpressionTraits<Expression2>::expression_type m_e2;
    BinaryOperation m_op;
};

template <typename Expression1, typename Expression2>
BinaryExpression<Expression1, Expression2, std::plus<double>> operator+ (Expression1 e1, Expression2 e2) {
    return BinaryExpression<Expression1, Expression2, std::plus<double>>(e1, e2);
}

template <typename Expression1, typename Expression2>
BinaryExpression<Expression1, Expression2, std::multiplies<double>> operator*(Expression1 e1, Expression2 e2) {
    return BinaryExpression<Expression1, Expression2, std::multiplies<double>>(e1, e2);
}


template <typename Expression1, typename Expression2>
BinaryExpression<Expression1, Expression2, std::divides<double>> operator/(Expression1 e1, Expression2 e2) {
    return BinaryExpression<Expression1, Expression2, std::divides<double>>(e1, e2);
}

template<typename Expression>
double integrate(Expression e, const double from, const double to, const size_t n) {
    const double step = (to - from) / n;

    double sum = 0.0;
    
    for (double i=from + step*0.5; i<to; i+=step) {
        sum += e.evaluate(i);
    }

    return sum;
}

int main() {
    const float array1[] = {1.0f, 1.0f, 1.0f};
    const float array2[] = {1.5f, 1.0f, 1.0f};

    const float result = lazy::dot<float, 3>(array1, array2);

    std::cout << result  << std::endl;

    Identity<double> x;

    double result2 = integrate( x / (1.0 + x), 1.0, 5.0, 10);

    std::cout << result2 << std::endl;

    return 0;
}
