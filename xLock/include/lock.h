#pragma once
#include <atomic>

namespace lck {

	class s_lock {
	private:
		std::atomic<bool> lck{ false };
	public:
		void lock() {
			while (lck.exchange(true, std::memory_order_acquire)) {

			}
			return;
		}
		bool unlock() {
			return lck.exchange(false, std::memory_order_release);
		}
	};

	class TTASSpinLock {
	private:
		std::atomic<bool> ready_{ false };
	public:
		void lock() {
			while (true) {
				while (ready_.load(std::memory_order_relaxed)) {

				}
				if (!ready_.exchange(true, std::memory_order_acquire)) {
					return;
				}
			}
		}

		void unlock() {
			ready_.store(false, std::memory_order_release);
		}
	};

	
}