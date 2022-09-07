#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <random>

#define size 4096

int main() {
    unsigned int nThreads = std::thread::hardware_concurrency();
    unsigned int step = size / nThreads;
    std::random_device rd;
    std::default_random_engine eng(rd());
    std::uniform_real_distribution<> getFloat(-RAND_MAX, RAND_MAX);
    float A[size][size];
    float B[size][size];
    float result[size][size];
    auto start = std::chrono::system_clock::now();

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

    auto multiply {
        [&A, &B, &result](unsigned int from, unsigned int to) {
            for (unsigned int i = from; i < to; ++i) {
                for (unsigned int j = from; j < to; ++j) {
                    result[i][j] = A[i][j] * B[i][j];
                }
            }
        }
    };

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
    std::cout << "Time: " + std::to_string(elapsed_nanoseconds) << std::endl;
    // TODO time, single thread
}
