#include <vector>
#include <iostream>
#include <algorithm>

#define NEW_LINE (std::cout << '\n')

struct int_generator {
    static int num;
    int operator()() { return num++; }
};
int int_generator::num = 1;

int main(void) {
    std::vector<int> v(16);

    auto print = [](const int n) { std::cout << n << ' '; };

    // 通过 std::generate(仿函数形式) 生成序列
    {
        std::generate(v.begin(), v.end(), int_generator());
        std::for_each(v.cbegin(), v.cend(), print); NEW_LINE;
    }

    // 通过 std::generate(lambada形式) 生成序列
    {
        std::generate(v.begin(), v.end(), []() -> int { return int_generator::num++; });
        std::for_each(v.cbegin(), v.cend(), print); NEW_LINE;
    }

    // 通过 std::generate_n(仿函数形式) 生成序列
    {
        std::generate_n(v.begin(), v.size(), int_generator());
        std::for_each(v.cbegin(), v.cend(), print); NEW_LINE;
    }
}