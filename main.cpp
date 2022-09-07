#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <numeric>

int main() {
    unsigned int nThreads = std::thread::hardware_concurrency();
    long long size = 10000000000LL;
    std::vector<unsigned long long> sumVector(nThreads, 0);
    long long step = size / nThreads;
    auto start = std::chrono::system_clock::now();

    auto separateSum{
            [&sumVector](unsigned int pos, unsigned long long first, unsigned long long last) {
                unsigned long long sum = 0;

                for (unsigned long long i = first; i < last; ++i) {
                    sum += i;
                }

                sumVector.at(pos) = sum;
            }
    };

    std::vector<std::thread> threadVector;

    for (unsigned int i = 0; i < nThreads; ++i) {
        long long remains = (i == nThreads - 1)
                            ? size % nThreads
                            : 0;
        threadVector.emplace_back(separateSum, i, i * step, (i + 1) * step + remains);
    }

    for (auto &thread: threadVector) {
        thread.join();
    }

    unsigned long long result = std::accumulate(sumVector.begin(), sumVector.end(), (unsigned long long int) 0LL);
    auto end = std::chrono::system_clock::now();
    auto elapsed_seconds = end - start;
    std::cout << std::to_string(nThreads) + " threads:\n" + std::to_string(result) + ". Time: " +
                 std::to_string(elapsed_seconds.count()) + " seconds" << std::endl;
    std::cout << std::endl;
    start = std::chrono::system_clock::now();
    unsigned long long res1 = 0;

    for (unsigned long long i = 1; i <= size; ++i) {
        res1 += i;
    }

    end = std::chrono::system_clock::now();
    elapsed_seconds = end - start;
    std::cout << "One thread:\n" + std::to_string(res1) + ". Time: " + std::to_string(elapsed_seconds.count()) +
                 " seconds";
}
