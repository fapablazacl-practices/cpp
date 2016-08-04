
#include <vector>
#include <boost/phoenix.hpp>

bool is_odd(int arg1) {
    return arg1 % 2 == 1;
}

int main() {
    std::vector<int> values = {
        0, 1, 3, 1, 3, 7, 6, -1
    };

    auto it = std::find_if(values.begin(), values.end(), &is_odd);

    return 0;
}
