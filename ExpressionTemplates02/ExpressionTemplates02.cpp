
#include <iostream>
#include <cstddef>
#include <functional>

namespace xe {
    template<typename E>
    class VectorExpression {
    public:
        float operator[] (const size_t i) const {   
            return static_cast<E const&>(*this)[i];
        }
        
        float& operator[] (const size_t i) {
            return static_cast<E const&>(*this)[i];
        }
        
        size_t size() const {
            return static_cast<E const&>(*this).size();
        }
        
        E& operator() () {
            return static_cast<E&>(*this);
        }
        
        const E& operator() () const {
            return static_cast<E const&>(*this);
        }
    };
    
    class VectorValue : public VectorExpression<VectorValue> {
    public:
        VectorValue() {}
        
        VectorValue(float x, float y, float z) {
            m_values[0] = x;
            m_values[1] = y;
            m_values[2] = z;
        }
        
        template<typename E>
        VectorValue( VectorExpression<E> const &v) {
            for (size_t i=0; i<3; i++) {
                m_values[i] = v[i];
            }
        }
        
        float operator[] (const size_t i) const {   
            return m_values[i];
        }
        
        float& operator[] (const size_t i) {
            return m_values[i];
        }
        
        size_t size() const {
            return 3;
        }
    private:
        float m_values[3] = {};
    };
    
    std::ostream& operator<< (std::ostream &os, const VectorValue &v) {
        return (os << v[0] << ", " << v[1] << ", " << v[2]);
    }
    
    template<typename E1, typename E2, typename O>
    class VectorOperation : public VectorExpression<VectorOperation<E1, E2, O>> {
    public:
        VectorOperation(E1 e1, E2 e2, O o) : m_e1(e1), m_e2(e2), m_o(o) {}
        
        float operator[] (const size_t i) const {   
            return m_o(m_e1[i], m_e2[i]);
        }
        
        float& operator[] (const size_t i) {
            return m_o(m_e1[i], m_e2[i]);
        }
        
        size_t size() const {
            return 3;
        }
        
    private:
        E1 m_e1;
        E2 m_e2;
        O m_o;
    };
    
    template<typename E1, typename E2>
    VectorOperation<E1, E2, std::plus<float>> const
    operator+ (E1 const &e1, E2 const &e2) {
        return VectorOperation<E1, E2, std::plus<float>>(e1, e2, std::plus<float>());
    }
}

int main() {
    using namespace xe;
    
    xe::VectorValue v1 = {0.0f, 1.0f, 2.0f};
    xe::VectorValue v2 = {2.0f, 1.0f, 0.0f};
    
    xe::VectorValue result = v1 + v2 + v2 + v1;
    
    std::cout << result << std::endl;
    
    return 0;
}
