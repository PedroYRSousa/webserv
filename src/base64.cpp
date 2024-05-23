#include "lib.hpp"

static const std::string base64_chars =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"abcdefghijklmnopqrstuvwxyz"
	"0123456789+/";

std::string encode(const std::string &input)
{
	std::string encoded;
	int val = 0;
	int bits = 0;

	for (size_t i = 0; i < input.length(); i++)
	{
		char c = input[i];

		val = (val << 8) | c;
		bits += 8;
		while (bits >= 6)
		{
			encoded.push_back(base64_chars[(val >> (bits - 6)) & 0x3F]);
			bits -= 6;
		}
	}

	if (bits > 0)
	{
		val = val << (6 - bits);
		encoded.push_back(base64_chars[val & 0x3F]);
	}

	while (encoded.size() % 4 != 0)
	{
		encoded.push_back('=');
	}

	return encoded;
}
