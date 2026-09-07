#include <iostream>
#include <thread>
#include <vector>
#include "../include/lock.h"
#include <chrono>

constexpr int NUM_THREADS = 4;
constexpr uint64_t ITR = 1'000'000;
constexpr uint64_t EXPECTED_VALUE = static_cast<uint64_t>(NUM_THREADS) * ITR;


uint64_t run_positive() {
	uint64_t cnt = 0;
	lck::s_lock lck;
	std::vector<double> time_took_get_locks(NUM_THREADS, 0.0);
	std::vector<double> actual_work_time(NUM_THREADS, 0.0);
	auto worker = [&](double& waste, double& actual) {
		for (uint64_t i = 0; i < ITR; i++) {
			auto before_lock = std::chrono::high_resolution_clock::now();
			lck.lock();
			auto after_lock = std::chrono::high_resolution_clock::now();
			cnt++;
			lck.unlock();
			auto after_work = std::chrono::high_resolution_clock::now();

			waste += std::chrono::duration<double>(after_lock - before_lock).count();
			actual += std::chrono::duration<double>(after_work - after_lock).count();
		}
	};

	{
		std::vector<std::jthread> threads;
		for (int i = 0; i < NUM_THREADS; i++) threads.emplace_back(worker, std::ref(time_took_get_locks[i]), std::ref(actual_work_time[i]));
	}

	std::cout << "[STATS]:  " << std::endl;
	std::cout << "          CPU WASTE USAGE" << std::endl;
	for (int i = 0; i < NUM_THREADS; i++) {
		std::cout << "          - Thread " << i << ": " << ((double)(time_took_get_locks[i]) / (time_took_get_locks[i] + actual_work_time[i])) * 100 << "%" << std::endl;
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