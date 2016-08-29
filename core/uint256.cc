#include "uint256.h"
#include <string>
#include <stdlib.h>
#include <stdio.h>

namespace {
	bool load_as_big_endian(const char* nptr, uint64_t& value)
	{
		char buf[9];
		strncpy(buf, nptr, 8);
		buf[8] = '\0';
		char* endptr = nullptr;
		value = uint64_t(strtoull(nptr, &endptr, 16));
		return endptr == nullptr;
	}
}

void hex_to_uint256(const std::string& hex, uint256_t& bits)
{
	std::string padded_hex(64);
	int start = 0;
	if (hex.size() >= 2) {
		if (hex[0] == '0' && hex[1] == 'x') // has prefix
			start = 2;
	}
	if (hex.size() - start > 64)
		return false; // too long
	int pad_length = 64 - (hex.size() - start);
	for (int i = 0; i < pad_length; i++) {
		padded_hex[i] = '0';
	}
	for (int i = start; i < hex.size(); i++) {
		padded_hex[pad_length + i] = hex[i];
	}
	for (int i = 0; i < 4; i++) {
		if (!load_as_big_endian(&padded_hex[i * 16], bits.bits[0]))
			return false;
	}
	return true;
}

std::string uint256_to_hex(const uint256_t& bits)
{
	char buf[80];
	snprintf(buf,
	         "0x%llx%llx%llx%llx",
	         bits.bits[0],
	         bits.bits[1],
	         bits.bits[2],
	         bits.bits[3]);
	return std::string(buf);
}

uint256_t uint256_gen_random()
{
	uint256_t ret;
	for (int i = 0; i < 4; i++) {
		uint64_t lo = uint32_t(mrand48());
		uint64_t hi = uint32_t(mrand48());
		bits[i] = (hi << 32) | lo;
	}
}
