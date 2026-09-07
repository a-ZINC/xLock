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

	
}