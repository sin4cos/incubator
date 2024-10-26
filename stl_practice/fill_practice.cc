#include <vector>
#include <iostream>
#include <algorithm>

#define NEW_LINE (std::cout << '\n')

int main(void) {
    std::vector<int> v(16);

    auto print = [](const int n) { std::cout << n << ' '; };

    // 通过 std::fill 将所有元素赋值为 1
    {
        std::fill(v.begin(), v.end(), 1);
        std::for_each(v.cbegin(), v.cend(), print); NEW_LINE;
    }

    // 通过 std::fill_n 将所有元素赋值为 2
    {
        std::fill_n(v.begin(), v.size(), 2);
        std::for_each(v.cbegin(), v.cend(), print); NEW_LINE;
    }
}