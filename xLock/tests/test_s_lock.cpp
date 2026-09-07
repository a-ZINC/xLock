#include <iostream>
#include <thread>
#include <vector>
#include "../include/lock.h"

constexpr int NUM_THREADS = 4;
constexpr uint64_t ITR = 1'000'000;
constexpr uint64_t EXPECTED_VALUE = static_cast<uint64_t>(NUM_THREADS) * ITR;


uint64_t run_positive() {
	uint64_t cnt = 0;
	lck::s_lock lck;

	auto worker = [&]() {
		for (uint64_t i = 0; i < ITR; i++) {
			lck.lock();
			cnt++;
			lck.unlock();
		}
		};

	{
		std::vector<std::jthread> threads;
		for (int i = 0; i < NUM_THREADS; i++) threads.emplace_back(worker);
	}
	
	return cnt;

}

uint64_t run_negative() {
	uint64_t cnt = 0;
	lck::s_lock lck;

	auto worker = [&]() {
		for (uint64_t i = 0; i < ITR; i++) {
			cnt++;
		}
		};

	{
		std::vector<std::jthread> threads;
		for (int i = 0; i < NUM_THREADS; i++) threads.emplace_back(worker);
	}

	return cnt;
}

int main() {
	std::cout << "====" << "TEST: simple lock" << "====" << std::endl;
	uint64_t positive = run_positive();
	bool positive_ok = (positive == EXPECTED_VALUE);
	if (!positive_ok) {
		std::cout << "[ERROR]: " << "Failed positive test, val: " << positive << std::endl;
		return -1;
	}

	uint64_t negative = run_negative();
	bool negative_ok = (negative == EXPECTED_VALUE);
	if (!negative_ok) {
		std::cout << "[BUG]: " << "Lost update, val: " << negative << std::endl;
	}

	return 0;
}