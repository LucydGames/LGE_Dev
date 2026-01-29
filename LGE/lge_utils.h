#pragma once

#include <functional>
#include <random>
#include <cstdint>

namespace lge {

	// from: https://stackoverflow.com/a/57595105
	template <typename T, typename... Rest>
	void hashCombine(std::size_t& seed, const T& v, const Rest&... rest) 
	{
		seed ^= std::hash<T>{}(v)+0x9e3779b9 + (seed << 6) + (seed >> 2);
		(hashCombine(seed, rest), ...);
	};

	uint32_t ConvertRGBAToUInt32Color(float r, float g, float b, float a)
	{
		uint8_t convR = static_cast<uint8_t>(r * 255.0f);
		uint8_t convG = static_cast<uint8_t>(g * 255.0f);
		uint8_t convB = static_cast<uint8_t>(b * 255.0f);
		uint8_t ConvA = static_cast<uint8_t>(a * 255.0f);
		return (ConvA << 24) | (convB << 16) | (convG << 8) | convR;
	};

	uint32_t ConvertRGBToUInt32Color(float r, float g, float b)
	{
		return ConvertRGBAToUInt32Color(r, g, b, 1.0f);
	};

	uint32_t GenerateRandomUInt32_tColor()
	{
		unsigned int min = 0;
		unsigned int max = 255;
		static std::mt19937 generator(std::random_device{}());
		static std::uniform_int_distribution<unsigned int> distribution(min, max);
		uint8_t r = distribution(generator);
		uint8_t g = distribution(generator);
		uint8_t b = distribution(generator);
		uint8_t a = 255; // Full opacity
		return (a << 24) | (b << 16) | (g << 8) | r;
	};
}  // namespace lge
