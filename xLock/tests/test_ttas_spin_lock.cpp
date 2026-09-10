#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include "../include/lock.h"

int NUM_THREADS = 4;
constexpr uint64_t ITR = 1'000'000;
uint64_t EXPECTED_VALUE = static_cast<uint64_t>(NUM_THREADS) * ITR;

void tas_run() {
	lck::s_lock lck;
	uint64_t cnt = 0;

	auto worker = [&]() {
		for (int i = 0; i < ITR; i++) {
			lck.lock();
			cnt++;
			lck.unlock();
		}
	};

	auto start = std::chrono::high_resolution_clock::now();
	{
		std::vector<std::jthread> threads;
		for (int i = 0; i < NUM_THREADS; i++) {
			threads.push_back(std::jthread(worker));
		}
	}
	auto end = std::chrono::high_resolution_clock::now();
	auto totalTime = std::chrono::duration<double>(end - start).count();

	std::cout << "[STATS]: TAS[" << NUM_THREADS << "]" << std::endl;
	std::cout << "               Throughput:     " << (NUM_THREADS * ITR) / (totalTime * 1000000) << "million ops/sec" << std::endl;
	std::cout << "               Count:          " << cnt << std::endl;
}

void ttas_run() {
	lck::TTASSpinLock lck;
	uint64_t cnt = 0;

	auto worker = [&]() {
		for (int i = 0; i < ITR; i++) {
			lck.lock();
			cnt++;
			lck.unlock();
		}
		};

	auto start = std::chrono::high_resolution_clock::now();
	{
		std::vector<std::jthread> threads;
		for (int i = 0; i < NUM_THREADS; i++) {
			threads.push_back(std::jthread(worker));
		}
	}
	auto end = std::chrono::high_resolution_clock::now();
	auto totalTime = std::chrono::duration<double>(end - start).count();

	std::cout << "[STATS]: TTAS[" << NUM_THREADS << "]" << std::endl;
	std::cout << "               Throughput:     " << (NUM_THREADS * ITR) / (totalTime * 1000000) << "million ops/sec" << std::endl;
	std::cout << "               Count:          " << cnt << std::endl;
}


int main() {

	for (int i = 0; i < 1; i++) {
		NUM_THREADS *= (i + 1);
		tas_run();
		ttas_run();
	}

	return 0;
}