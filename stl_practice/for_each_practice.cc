#include <vector>
#include <iostream>
#include <algorithm>

#define NEW_LINE (std::cout << '\n')

struct int_generator {
    static int num;
    void operator()(int &n) { n = num++; }
};
int int_generator::num = 1;

int main(void) {
    std::vector<int> v(16);

    auto print = [](const int n) { std::cout << n << ' '; };

    // 通过 std::for_each(仿函数形式) 生成序列
    {
        std::for_each(v.begin(), v.end(), int_generator());
        std::for_each(v.cbegin(), v.cend(), print); NEW_LINE;
    }

    // 通过 std::for_each(lambda形式) 生成序列
    {
        std::for_each(v.begin(), v.end(), [](int & n ){ n = int_generator::num++; });
        std::for_each(v.cbegin(), v.cend(), print); NEW_LINE;
    }

    // C++17: 通过 std::for_each_n(仿函数形式) 生成序列
    {
        std::for_each_n(v.begin(), v.size(), int_generator());
        std::for_each(v.cbegin(), v.cend(), print); NEW_LINE;
    }
}