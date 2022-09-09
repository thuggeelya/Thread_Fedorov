#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <random>

#define size 4096

void execute(float A[][size], float B[][size], unsigned int nThreads, unsigned int step) {
    int nIterations = 10;
    std::vector<long long> timeMultipleThread(nIterations);
    std::vector<long long> timeSingleThread(nIterations);

    for (int i = 0; i < nIterations; ++i) {
        std::cout << "\nIteration #" + std::to_string(i) << std::endl;
        auto result = new float[size][size];
        auto resultSingleThread = new float[size][size];

        auto multiply{
                [&A, &B, &result](unsigned int from, unsigned int to) {
                    for (unsigned int i = from; i < to; ++i) {
                        for (unsigned int j = 0; j < size; ++j) {
                            result[i][j] = 0;

                            for (unsigned int j2 = 0; j2 < size; ++j2) {
                                result[i][j] += A[i][j2] * B[j2][j];
                            }
                        }
                    }
                }
        };

        auto multiplySingleThread{
                [&A, &B, &resultSingleThread](unsigned int from, unsigned int to) {
                    for (unsigned int i = from; i < to; ++i) {
                        for (unsigned int j = 0; j < size; ++j) {
                            resultSingleThread[i][j] = 0;

                            for (unsigned int j2 = 0; j2 < size; ++j2) {
                                resultSingleThread[i][j] += A[i][j2] * B[j2][j];
                            }
                        }
                    }
                }
        };

        auto start = std::chrono::system_clock::now();
        std::vector<std::thread> threadVectorMultiplier;

        for (unsigned int k = 0; k < nThreads; ++k) {
            unsigned int remains = (k == nThreads - 1) ? size % nThreads : 0;
            threadVectorMultiplier.emplace_back(multiply, k * step, (k + 1) * step + remains);
        }

        for (auto &thread: threadVectorMultiplier) {
            thread.join();
        }

        auto end = std::chrono::system_clock::now();
        auto elapsed_nanoseconds = (end - start).count();
        std::cout << nThreads << " threads time:\n" << elapsed_nanoseconds << " nanoseconds" << std::endl;
        timeMultipleThread.at(i) = elapsed_nanoseconds;

        start = std::chrono::system_clock::now();

        multiplySingleThread(0, size);

        end = std::chrono::system_clock::now();
        elapsed_nanoseconds = (end - start).count();
        std::cout << "One thread time:\n" << elapsed_nanoseconds << " nanoseconds" << std::endl;
        timeSingleThread.at(i) = elapsed_nanoseconds;

        for (int r1 = 0; r1 < size; ++r1) {
            for (int r2 = 0; r2 < size; ++r2) {
                if (result[r1][r2] != resultSingleThread[r1][r2]) {
                    std::cerr << result[r1][r2] << " not equals " << resultSingleThread[r1][r2] << std::endl;
                    break;
                }
            }
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

int main() {
    unsigned int nThreads = std::thread::hardware_concurrency();
    unsigned int step = size / nThreads;
    std::random_device rd;
    std::default_random_engine eng(rd());
    std::uniform_real_distribution<> getFloat(-RAND_MAX, RAND_MAX);

    auto A = new float [size][size];
    auto B = new float [size][size];

    auto createFloats {
            [&A, &B, &getFloat, &eng](unsigned int from, unsigned int to) {
                for (unsigned int i = from; i < to; ++i) {
                    for (unsigned int j = 0; j < size; ++j) {
                        A[i][j] = (float) getFloat(eng);
                        B[i][j] = (float) getFloat(eng);
                    }
                }
            }
    };

    std::vector<std::thread> threadVectorRandomizer;

    for (unsigned int k = 0; k < nThreads; ++k) {
        unsigned int remains = (k == nThreads - 1) ? size % nThreads : 0;
        threadVectorRandomizer.emplace_back(createFloats, k * step, (k + 1) * step + remains);
    }

    for (auto &thread: threadVectorRandomizer) {
        thread.join();
    }

    execute(A, B, nThreads, step);
}
