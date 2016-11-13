
#include <vector>
#include <cassert>
#include <string>
#include <iostream>

namespace xe {    
    // Generic vector expression
    template<typename E>
    class VectorExpression {
    public:
        float operator[] (const std::size_t i) const {
            auto rthis = static_cast<E const &>(*this);
            
            return rthis[i];
        }
        
        std::size_t size() const {
            auto rthis = static_cast<E const &>(*this);
            
            return rthis.size();
        }
        
        std::string toString() const {
            auto rthis = static_cast<E const &>(*this);
            
            return rthis.toString();
        }
    };
    
    // Vector expression result holder
    class Vector : public VectorExpression<Vector> {
    public:
        Vector() : values(3) {}
        
        Vector(const float x, const float y, const float z) : values(3) {
            values[0] = x;
            values[1] = y;
            values[2] = z;
        }
        
        ~Vector() {}
        
        // Vector expression evaluator constructor
        template<typename E>
        Vector(VectorExpression<E> const& other) : values(other.size()) {
            for (std::size_t i=0; i<other.size(); i++) {
                values[i] = other[i];
            }
        }
        
        float operator[] (const std::size_t i) const {
            return values[i];
        }
        
        float& operator[] (const std::size_t i) {
            return values[i];
        }
        
        std::size_t size() const {
            return values.size();
        }
        
        std::string toString() const {
            std::string str;
            
            for (std::size_t i=0; i<values.size(); i++) {
                str += std::to_string(values[i]);
                 
                if (i < values.size() - 1) {
                    str += ", ";
                }
            }
        
            return "(" + str + ")";
        }
        
    private:
        std::vector<float> values;
    };
    
    // Vector Addition expression
    template<typename E1, typename E2>
    class VectorSum : public VectorExpression<VectorSum<E1, E2>> {
    public:
        VectorSum(const E1 &v1_, const E2 &v2_) : v1(v1_), v2(v2_) {
            assert(v1.size() == v2.size());
        }
        
        float operator[] (std::size_t i) const {
            return v1[i] + v2[i];
        }
        
        std::size_t size() const {
            return v1.size();
        }

        std::string toString() const {
            return v1.toString() + " + " + v2.toString();
        }
        
    private:
        const E1 &v1;
        const E2 &v2;
    };
    
    // Vector operator to build a VectorSum from two different vector expressions
    template<typename E1, typename E2>
    const VectorSum<E1, E2> operator+(const E1 &e1, const E2 &e2) {
        return VectorSum<E1, E2>(e1, e2);
    }
    
    // Vector Addition expression
    template<typename E1, typename E2>
    class VectorSubtract : public VectorExpression<VectorSum<E1, E2>> {
    public:
        VectorSubtract(const E1 &v1_, const E2 &v2_) : v1(v1_), v2(v2_) {
            assert(v1.size() == v2.size());
        }
        
        float operator[] (std::size_t i) const {
            return v1[i] - v2[i];
        }
        
        std::size_t size() const {
            return v1.size();
        }

        std::string toString() const {
            return v1.toString() + " + " + v2.toString();
        }
        
    private:
        const E1 &v1;
        const E2 &v2;
    };
    
    // Vector operator to build a VectorSum from two different vector expressions
    template<typename E1, typename E2>
    const VectorSubtract<E1, E2> operator-(const E1 &e1, const E2 &e2) {
        return VectorSum<E1, E2>(e1, e2);
    }
}

int main() {
    xe::Vector 
        v1(0.0f, 1.0f, 2.0f), 
        v2(-1.0f, 0.0f, 0.0f),
        v3(0.0f, -2.0f, 0.0f),
        v4(1.0f, 0.0f, -1.0f);
    
    auto result = v1 + v2 + v3 + v4;
    
    std::cout << result.toString() << std::endl;
    
    return 0;
}
