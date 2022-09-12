#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <numeric>

int main() {
    int nIterations = 10;
    std::vector<long long> timeMultipleThread(nIterations);
    std::vector<long long> timeSingleThread(nIterations);
    unsigned int nThreads = std::thread::hardware_concurrency();
    long long size = 10000000000LL;
    long long step = size / nThreads;

    for (int i = 0; i < nIterations; ++i) {
        std::cout << "\nIteration #" + std::to_string(i) << std::endl;
        std::vector<unsigned long long> sumVector(nThreads, 0);

        auto separateSum{
                [&sumVector](unsigned int pos, unsigned long long first, unsigned long long last) {
                    unsigned long long sum = 0;

                    for (unsigned long long i = first + 1; i < last + 1; ++i) {
                        sum += i;
                    }

                    sumVector.at(pos) = sum;
                }
        };

        auto start = std::chrono::system_clock::now();
        std::vector<std::thread> threadVector;

        for (unsigned int k = 0; k < nThreads; ++k) {
            long long remains = (k == nThreads - 1)
                                ? size % nThreads
                                : 0;
            threadVector.emplace_back(separateSum, k, k * step, (k + 1) * step + remains);
        }

        for (auto &thread: threadVector) {
            thread.join();
        }

        unsigned long long result = std::accumulate(sumVector.begin(), sumVector.end(), (unsigned long long) 0LL);
        auto end = std::chrono::system_clock::now();
        auto elapsed_nanoseconds = (end - start).count();
        std::cout << nThreads << " threads:\n" << result << ". Time: " << elapsed_nanoseconds << " nanoseconds" << std::endl;
        timeMultipleThread.at(i) = elapsed_nanoseconds;

        start = std::chrono::system_clock::now();
        unsigned long long res1 = 0;

        for (unsigned long long j = 1; j <= size; ++j) {
            res1 += j;
        }

        end = std::chrono::system_clock::now();
        elapsed_nanoseconds = (end - start).count();
        std::cout << "One thread:\n" << res1 << ". Time: " << elapsed_nanoseconds << " nanoseconds" << std::endl;
        timeSingleThread.at(i) = elapsed_nanoseconds;

        if (result != res1) {
            std::cerr << result << " not equals " << res1 << std::endl;
            break;
        }
    }

    std::cout << std::endl;
    long long avgMultiple = std::accumulate(timeMultipleThread.begin(), timeMultipleThread.end(),
                                            (long long) 0LL) / nIterations;
    long long avgSingle = std::accumulate(timeSingleThread.begin(), timeSingleThread.end(),
                                          (long long) 0LL) / nIterations;
    std::cout << "\nAverage time for multiple threads (nanoseconds): " + std::to_string(avgMultiple) << std::endl;
    std::cout << "Average time for single thread (nanoseconds)   : " + std::to_string(avgSingle) << std::endl;
    std::cout << "Multithreading is faster in " + std::to_string((double) avgSingle / (double) avgMultiple) + " times";
}
