#pragma once

#include <random>
#include <string>
#include <assert.h>

namespace jlib
{

template <class T = std::mt19937, std::size_t N = T::state_size>
inline auto seeded_random_engine() -> typename std::enable_if<!!N, T>::type {
	typename T::result_type random_data[N];
	std::random_device source;
	std::generate(std::begin(random_data), std::end(random_data), std::ref(source));
	std::seed_seq seeds(std::begin(random_data), std::end(random_data));
	T seed_engine(seeds);
	return seed_engine;
};

static constexpr const char default_rand_source[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890"; // 63
static constexpr size_t default_rand_source_size = sizeof(default_rand_source) - 1; // 62


inline std::string nonce(const char* source = default_rand_source, 
						 size_t source_size = default_rand_source_size, 
						 size_t dst_size = default_rand_source_size)
{
	assert(dst_size <= source_size);
	auto rgn = seeded_random_engine();
	std::string nonce;
	std::uniform_int_distribution<size_t> dist(0, source_size);
	for (size_t i = 0; i < dst_size; i++) {
		nonce += source[dist(rgn)];
	}

	return nonce;
}
    
} // namespace jlib
