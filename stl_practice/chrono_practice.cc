#include <chrono>
#include <thread>
#include <iostream>


#ifdef description

template <intmax_t _Num, intmax_t _Den = 1>
class ratio
{
public:
    static const intmax_t num;
    static const intmax_t den;
    typedef ratio<num, den> type;
};

template <intmax_t _Num, intmax_t _Den>
const intmax_t ratio<_Num, _Den>::num;

template <intmax_t _Num, intmax_t _Den>
const intmax_t ratio<_Num, _Den>::den;

#endif

int main(void) {
    // 比例
    {
        std::ratio<1, 10> one;
        std::cout << one.num << "/" << one.den << '\n';     // 1/10
    }

    // 计算时间差
    {
        auto beg = std::chrono::steady_clock::now();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        auto end = std::chrono::steady_clock::now();
        std::cout << "elapsed1: " << std::chrono::duration<double, std::milli>(end - beg).count() << '\n';
        std::cout << "elapsed2: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - beg).count() << '\n';
    }

    // 单位时间
    {
        std::cout << "1 seconds: " << std::chrono::seconds(1).count() << '\n';
        std::cout << "1 minutes: " << std::chrono::minutes(1).count() << '\n';
        std::cout << "1 hours: " << std::chrono::hours(1).count() << '\n';
    }

    // 时间单位转换
    {
        std::cout << "1 seconds = " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::seconds(1)).count() << " milliseconds\n";
        std::cout << "1 minutes = " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::minutes(1)).count() << " milliseconds\n";
        std::cout << "1 hours = " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::hours(1)).count() << " milliseconds\n";
    }
}