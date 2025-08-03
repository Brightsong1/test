#include <iostream>
#include <cstdint>
#include <thread>
#include <vector>
#include <mutex>
#include <algorithm>

int64_t Gcd(int64_t a, int64_t b, int64_t& x, int64_t& y) 
{
    if (a == 0) 
    {
        x = 0;
        y = 1;
        return b;
    }
    int64_t x1, y1;
    int64_t gcd = Gcd(b % a, a, x1, y1);
    x = y1 - (b / a) * x1;
    y = x1;
    return gcd;
}

int64_t modInverse(int64_t a, int64_t m) 
{
    int64_t x, y;
    int64_t gcd = Gcd(a, m, x, y);
    if (gcd != 1) 
    {
        return -1;
    }
    return (x % m + m) % m;
}

void findLcgParametersThread(int64_t x0, int64_t x1, int64_t x2, int64_t x3, int64_t x4,
                            int64_t start, int64_t end, int64_t& a, int64_t& c, int64_t& m,
                            bool& found, std::mutex& mtx) 
{
    for (int64_t localM = start; localM <= end && !found; ++localM) 
    {
        int64_t diff0 = (x1 - x0 + localM) % localM;
        int64_t diff1 = (x2 - x1 + localM) % localM;

        if (diff0 == 0)
        {
             continue;
        }

        int64_t invDiff0 = modInverse(diff0, localM);
        if (invDiff0 == -1)
        { 
            continue;
        }

        int64_t localA = (diff1 * invDiff0) % localM;
        int64_t localC = (x1 - localA * x0 + localM) % localM;
        if (x2 == (localA * x1 + localC) % localM &&
            x3 == (localA * x2 + localC) % localM &&
            x4 == (localA * x3 + localC) % localM) 
        {
            std::lock_guard<std::mutex> lock(mtx);
            if (!found) 
            {
                a = localA;
                c = localC;
                m = localM;
                found = true;
            }
        }
    }
}

bool findLcgParameters(int64_t x0, int64_t x1, int64_t x2, int64_t x3, int64_t x4,
                       int64_t& a, int64_t& c, int64_t& m, int64_t max) 
{
    int64_t x = std::max(std::max(x0, x1), std::max(x2, x3));
    int64_t start = x + 1;
    if (start > max) 
    { 
        return false;
    }

    bool found = false;
    std::mutex mtx;
    std::vector<std::thread> threads;
    int64_t rangeSize = (max - start + 1 + 15) / 16;

    for (int i = 0; i < 16; ++i) 
    {
        int64_t threadstart = start + i * rangeSize;
        int64_t threadend = std::min(threadstart + rangeSize - 1, max);
        if (threadstart <= max) 
        {
            threads.emplace_back(findLcgParametersThread, x0, x1, x2, x3, x4,
                                threadstart, threadend, std::ref(a), std::ref(c), std::ref(m),
                                std::ref(found), std::ref(mtx));
        }
    }
    for (auto& t : threads) 
    {
        t.join();
    }
    return found;
}

int64_t nextLcgNumber(int64_t x0, int64_t x1, int64_t x2, int64_t x3, int64_t x4, int64_t max = 65535) 
{
    int64_t a, c, m;
    if (!findLcgParameters(x0, x1, x2, x3, x4, a, c, m, max)) 
    {
        return -1;
    }
    return (a * x3 + c) % m;
}

int main() 
{
    int64_t x0 = 157, x1 = 5054, x2 = 25789, x3 = 13214, x4 = 16605;
    int64_t result = nextLcgNumber(x0, x1, x2, x3, x4);
    if (result == -1) 
    {
        std::cout << "No valid parameters found" << std::endl;
    } 
    else 
    {
        std::cout << "Next number: " << result << std::endl;
    }
    return 0;
}