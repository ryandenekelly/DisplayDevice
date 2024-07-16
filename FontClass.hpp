#pragma once

#include <cstdint>
#include <vector>


class FontClass
{
    public:
	FontClass();
	FontClass(std::uint8_t p_width, std::uint8_t p_height, const std::uint8_t fontArray[]);
	FontClass(std::uint8_t p_width, std::uint8_t p_height, const std::uint8_t fontArray[],
			     std::uint16_t p_dataLength);
	static constexpr std::uint16_t MAX_CHARS = 105;
	std::uint8_t width;
	std::uint8_t height;
	const std::uint8_t* data;
	std::uint16_t dataLength;

	std::uint16_t getCharIndex(char ch);
	std::vector<std::uint8_t> getChar(std::uint16_t fontIndex);

};
	extern const std::uint8_t font8x8[];
