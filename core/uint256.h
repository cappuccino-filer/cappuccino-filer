#ifndef UINT256_H
#define UINT256_H

struct uint256_t
{
	    uint64_t bits[4];
};

bool hex_to_uint256(const std::string& hex, uint256_t&);
std::string uint256_to_hex(const uint256_t&);
uint256_t uint256_gen_random();

inline bool operator==(const uint256_t& lhs, const uint256_t& rhs)
{
	return lhs.bits[0] == rhs.bits[0] &&
	       lhs.bits[1] == rhs.bits[1] &&
	       lhs.bits[2] == rhs.bits[2] &&
	       lhs.bits[3] == rhs.bits[3];
}

#endif
