#pragma once
#include "DisplayDevice.hpp"
#include "spi.h"

class ST7735 : public DisplayDevice
{
    public:

	ST7735();
	ST7735(SPI_HandleTypeDef* spiHandler, std::uint16_t resetPin, GPIO_TypeDef * resetPort, std::uint16_t nCSPin,
	       GPIO_TypeDef * nCSPort, std::uint16_t DCPin, GPIO_TypeDef * DCPort,
	       std::uint16_t width = 128, std::uint16_t height = 128);

	static constexpr std::uint8_t DELAY = 0x80;

	static constexpr std::uint8_t CMD_NOP = 0x00;
	static constexpr std::uint8_t CMD_SWRESET = 0x01;
	static constexpr std::uint8_t CMD_RDDID   = 0x04;
	static constexpr std::uint8_t CMD_RDDST   = 0x09;

	static constexpr std::uint8_t CMD_SLPIN   = 0x10;
	static constexpr std::uint8_t CMD_SLPOUT  = 0x11;
	static constexpr std::uint8_t CMD_PTLON   = 0x12;
	static constexpr std::uint8_t CMD_NORON   = 0x13;

	static constexpr std::uint8_t CMD_INVOFF  = 0x20;
	static constexpr std::uint8_t CMD_INVON   = 0x21;
	static constexpr std::uint8_t CMD_GAMSET  = 0x26;
	static constexpr std::uint8_t CMD_DISPOFF = 0x28;
	static constexpr std::uint8_t CMD_DISPON  = 0x29;
	static constexpr std::uint8_t CMD_CASET   = 0x2A;
	static constexpr std::uint8_t CMD_RASET   = 0x2B;
	static constexpr std::uint8_t CMD_RAMWR   = 0x2C;
	static constexpr std::uint8_t CMD_RAMRD   = 0x2E;

	static constexpr std::uint8_t CMD_PTLAR   = 0x30;
	static constexpr std::uint8_t CMD_COLMOD  = 0x3A;
	static constexpr std::uint8_t CMD_MADCTL  = 0x36;

	static constexpr std::uint8_t CMD_FRMCTR1 = 0xB1;
	static constexpr std::uint8_t CMD_FRMCTR2 = 0xB2;
	static constexpr std::uint8_t CMD_FRMCTR3 = 0xB3;
	static constexpr std::uint8_t CMD_INVCTR  = 0xB4;
	static constexpr std::uint8_t CMD_DISSET5 = 0xB6;

	static constexpr std::uint8_t CMD_PWCTR1  = 0xC0;
	static constexpr std::uint8_t CMD_PWCTR2  = 0xC1;
	static constexpr std::uint8_t CMD_PWCTR3  = 0xC2;
	static constexpr std::uint8_t CMD_PWCTR4  = 0xC3;
	static constexpr std::uint8_t CMD_PWCTR5  = 0xC4;
	static constexpr std::uint8_t CMD_VMCTR1  = 0xC5;

	static constexpr std::uint8_t CMD_RDID1   = 0xDA;
	static constexpr std::uint8_t CMD_RDID2   = 0xDB;
	static constexpr std::uint8_t CMD_RDID3   = 0xDC;
	static constexpr std::uint8_t CMD_RDID4   = 0xDD;

	static constexpr std::uint8_t CMD_PWCTR6  = 0xFC;

	static constexpr std::uint8_t CMD_GMCTRP1 = 0xE0;
	static constexpr std::uint8_t CMD_GMCTRN1 = 0xE1;

	static constexpr std::uint8_t MADCTL_MY  = 0x80;
	static constexpr std::uint8_t MADCTL_MX  = 0x40;
	static constexpr std::uint8_t MADCTL_MV  = 0x20;
	static constexpr std::uint8_t MADCTL_ML  = 0x10;
	static constexpr std::uint8_t MADCTL_RGB = 0x00;
	static constexpr std::uint8_t MADCTL_BGR = 0x08;
	static constexpr std::uint8_t MADCTL_MH  = 0x04;

	static constexpr std::uint8_t IS_128X128 = 1;

	static constexpr std::uint8_t XSTART = 2;
	static constexpr std::uint8_t YSTART = 3;
	static constexpr std::uint8_t ROTATION = (MADCTL_MX | MADCTL_MY | MADCTL_BGR);



	static inline std::uint32_t COLOUR565(std::uint8_t r, std::uint8_t g, std::uint8_t b)
	{
	    return (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3));
	}

	static constexpr std::uint8_t G10 = 0x01;
	static constexpr std::uint8_t G25 = 0x02;
	static constexpr std::uint8_t G22 = 0x04;
	static constexpr std::uint8_t G18 = 0x08;

	/* Base */
	void init();
	void fillScreen(std::uint16_t colour);
	void drawPixel(std::uint16_t x, std::uint16_t y, std::uint16_t colour);
	void writeChar(char ch, std::uint16_t colour, std::uint16_t bgcolour);
	void writeString(std::string str, std::uint16_t colour, std::uint16_t bgcolor);
	void drawLine(std::uint8_t x1, std::uint8_t y1, std::uint8_t x2, std::uint8_t y2, std::uint16_t colour);
	void drawPolyline(std::vector<std::pair<std::uint8_t, std::uint8_t>> vertex_list, std::uint16_t colour);
	void drawCircle(std::uint8_t par_x, std::uint8_t par_y, std::uint8_t par_r, std::uint16_t colour);
	void fillCircle(std::uint8_t par_x, std::uint8_t par_y, std::uint8_t par_r, std::uint16_t par_colour);
	void drawRectangle(std::uint8_t x1, std::uint8_t y1, std::uint8_t x2, std::uint8_t y2, std::uint16_t colour);
	void fillRectangle(std::uint16_t x, std::uint16_t y, std:: uint16_t w, std::uint16_t h, std::uint16_t colour);
	void setFont(FontClass *font);
	FontClass getFont();
	void setCursorXY(std::uint8_t x, std::uint8_t y);
	std::pair<std::uint8_t, std::uint8_t> getCursorXY();
	void resetCursor();
	std::uint8_t height();
	std::uint8_t width();
	std::uint16_t getColour(std::string colour);
	void refreshScreen();

	/* Derived */
	void select();
	void unselect();
	void reset();
	void drawImage(std::uint16_t x, std::uint16_t y, std::uint16_t w, std::uint16_t h, const std::uint16_t* data);
	void invertColors(bool invert);
	void setGamma(std::uint8_t gamma);

    private:
	SPI_HandleTypeDef* m_spiHandler;
	std::uint16_t m_resetPin;
	GPIO_TypeDef * m_resetPort;
	std::uint16_t m_nCSPin;
	GPIO_TypeDef * m_nCSPort;
	std::uint16_t m_DCPin;
	GPIO_TypeDef * m_DCPort;
	const std::uint8_t m_width;
	const std::uint8_t m_height;
	std::uint8_t m_currentX;
	std::uint8_t m_currentY;
	FontClass* m_font;

	/* Base */
	void writeCommand(std::uint8_t cmd);
	void writeData(std::uint8_t* buf, std::uint8_t buf_size);

	/* Derived */
	void executeCommandList(const uint8_t *addr);
	void executeCommand(std::uint8_t cmd, std::uint8_t argsSize, std::vector<std::uint8_t> argsList = {0});
	void setAddressWindow(std::uint8_t x0, std::uint8_t y0, std::uint8_t x1, std::uint8_t y1);
};
