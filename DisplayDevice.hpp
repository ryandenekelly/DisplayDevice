#pragma once

#include <cstdint>
#include <vector>
#include <string>

#include "FontClass.hpp"


class DisplayDevice
{
    public:
	DisplayDevice();
	DisplayDevice(std::uint8_t height, std::uint8_t width);

	enum Colour
	{
	    Black = 0x0000,
	    Blue = 0x001F,
	    Red = 0xF800,
	    Green = 0x07E0,
	    Cyan = 0x07FF,
	    Magenta = 0xF81F,
	    Yellow = 0xFFE0,
	    White = 0xFFFF
	};

	static inline std::uint16_t invertColour(std::uint16_t colour)
	{
	    return 0xFFFF - colour;
	}

	virtual void init() = 0;
	virtual void fillScreen(std::uint16_t colour) = 0;
	virtual void drawPixel(std::uint16_t x, std::uint16_t y, std::uint16_t colour) = 0;
	virtual void writeChar(char ch, std::uint16_t colour, std::uint16_t bgcolour) = 0;
	virtual void writeString(std::string str, std::uint16_t colour, std::uint16_t bgcolor) = 0;
	virtual void drawLine(std::uint8_t x1, std::uint8_t y1, std::uint8_t x2, std::uint8_t y2, std::uint16_t colour) = 0;
	virtual void drawPolyline(std::vector<std::pair<std::uint8_t, std::uint8_t>> vertex_list, std::uint16_t colour) = 0;
	virtual void drawCircle(std::uint8_t par_x, std::uint8_t par_y, std::uint8_t par_r, std::uint16_t colour) = 0;
	virtual void fillCircle(std::uint8_t par_x, std::uint8_t par_y, std::uint8_t par_r, std::uint16_t par_colour) = 0;
	virtual void drawRectangle(std::uint8_t x1, std::uint8_t y1, std::uint8_t x2, std::uint8_t y2, std::uint16_t colour) = 0;
	virtual void fillRectangle(std::uint16_t x, std::uint16_t y, std:: uint16_t w, std::uint16_t h, std::uint16_t colour) = 0;
	virtual void setFont(FontClass *font) = 0;
	virtual FontClass getFont() = 0;
	virtual void setCursorXY(std::uint8_t x, std::uint8_t y) = 0;
	virtual std::pair<std::uint8_t, std::uint8_t> getCursorXY() = 0;
	virtual void resetCursor() = 0;
	virtual std::uint8_t height() = 0;
	virtual std::uint8_t width() = 0;
	virtual std::uint16_t getColour(std::string colour) = 0;
	virtual void refreshScreen() = 0;

    private:
	const std::uint8_t m_width;
	const std::uint8_t m_height;


	virtual void writeCommand(std::uint8_t cmd) = 0;
	virtual void writeData(std::uint8_t* buf, std::uint8_t buf_size) = 0;
};


