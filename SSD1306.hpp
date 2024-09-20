/* Based on ssd1306 library from https://github.com/Matiasus/SSD1306 */
#pragma once
#include "DisplayDevice.hpp"
#include "i2c.h"

class SSD1306 : public DisplayDevice
{

    public:
	SSD1306();
	SSD1306(std::uint8_t i2cAddress, I2C_HandleTypeDef* i2c, std::uint8_t* buffer, std::uint8_t height = 64, std::uint8_t width = 128);
	static constexpr std::uint8_t X_OFFSET = 0;
	static constexpr std::uint8_t X_OFFSET_UPPER = 0;
	static constexpr std::uint8_t X_OFFSET_LOWER = 0;

	/* Fundamental Commands */
	static constexpr std::uint8_t CMD_CONTRAST_CONTROL = 0x81;
	static constexpr std::uint8_t CMD_ENTIRE_DISPLAY_ON_RAM = 0xA4;
	static constexpr std::uint8_t CMD_ENTIRE_DISPLAY_ON = 0xA5;
	static constexpr std::uint8_t CMD_NORMAL_DISPLAY = 0xA6;
	static constexpr std::uint8_t CMD_INVERSE_DISPLAY = 0xA7;
	static constexpr std::uint8_t CMD_DISPLAY_OFF = 0xAE;
	static constexpr std::uint8_t CMD_DISPLAY_ON = 0xAF;

	/* Scrolling Commands*/
	static constexpr std::uint8_t CMD_R_H_SCROLL = 0x26;
	static constexpr std::uint8_t CMD_L_H_SCROLL = 0x27;

	static constexpr std::uint8_t PAGE_0 = 0x00;
	static constexpr std::uint8_t PAGE_1 = 0x01;
	static constexpr std::uint8_t PAGE_2 = 0x02;
	static constexpr std::uint8_t PAGE_3 = 0x03;
	static constexpr std::uint8_t PAGE_4 = 0x04;
	static constexpr std::uint8_t PAGE_5 = 0x05;
	static constexpr std::uint8_t PAGE_6 = 0x06;
	static constexpr std::uint8_t PAGE_7 = 0x07;

	static constexpr std::uint8_t FRAMES_5   = 0x00;
	static constexpr std::uint8_t FRAMES_64  = 0x01;
	static constexpr std::uint8_t FRAMES_128 = 0x02;
	static constexpr std::uint8_t FRAMES_256 = 0x03;
	static constexpr std::uint8_t FRAMES_3   = 0x04;
	static constexpr std::uint8_t FRAMES_4   = 0x05;
	static constexpr std::uint8_t FRAMES_25  = 0x06;
	static constexpr std::uint8_t FRAMES_2   = 0x07;

	static constexpr std::uint8_t V_R_SCROLL = 0x29;
	static constexpr std::uint8_t V_L_SCROLL = 0x30;


	static constexpr std::uint8_t DEACTIVATE_SCROLL = 0x2E;
	static constexpr std::uint8_t ACTIVATE_SCROLL = 0x2F;

	/* Address Setting Commands*/
	static constexpr std::uint8_t LO_COL_ADDR  = 0x00;
	static constexpr std::uint8_t HI_COL_ADDR = 0x10;
	static inline std::uint8_t SET_LO_COL_ADDR(std::uint8_t startAddress) { return 0x00 | (0xF & startAddress);}
	static inline std::uint8_t SET_HI_COL_ADDR(std::uint8_t startAddress) { return 0x10 | (0xF & startAddress);}
	static constexpr std::uint8_t CMD_SET_MEM_ADDR_MODE = 0x20;
	static constexpr std::uint8_t ADDR_MODE_HOR  = 0x00;
	static constexpr std::uint8_t ADDR_MODE_VER  = 0x01;
	static constexpr std::uint8_t ADDR_MODE_PAGE = 0x02;


	static constexpr std::uint8_t CMD_SET_PAGE_START = 0xB0;
	static inline std::uint8_t SET_PAGE_START(std::uint8_t addr){ return (CMD_SET_PAGE_START | (addr & 0x07)); };


	/* Hardware Configuration Commands */
	static inline std::uint8_t SET_DISP_START_LINE(std::uint8_t startLine) { return 0x40 | (0x3F & startLine); }
	static constexpr std::uint8_t CMD_SET_SEG_REMAP_0 = 0xA0;
	static constexpr std::uint8_t CMD_SET_SEG_REMAP_127 = 0xA1;
	static constexpr std::uint8_t CMD_SET_MUX_RATIO = 0xA8;
	static constexpr std::uint8_t CMD_SET_COM_SCAN_NORMAL = 0xC0;
	static constexpr std::uint8_t CMD_SET_COM_SCAN_REMAP = 0xC8;
	static constexpr std::uint8_t CMD_SET_DISP_OFFSET = 0xD3;

	static constexpr std::uint8_t CMD_SET_COM_PINS = 0xDA;
	static constexpr std::uint8_t PINS_SEQ = 0x00;
	static constexpr std::uint8_t PINS_ALT = 0x01 << 4;
	static constexpr std::uint8_t PINS_DIS_REMAP = 0x00;
	static constexpr std::uint8_t PINS_EN_REMAP = 0x01 << 5;
	static inline std::uint8_t SET_COM_PINS(std::uint8_t pin5, std::uint8_t pin4){ return (0x02) | (pin5 | pin4);}

	/* Timing and Driving Scheme Setting Commands */
	static constexpr std::uint8_t CMD_SET_DISPLAY_CLK_DIV = 0xD5;
	static inline std::uint8_t DIV_RATIO_OSC_FREQ(std::uint8_t f_osc, std::uint8_t ratio){ return ((f_osc << 4) | ratio);}

	static constexpr std::uint8_t CMD_CHARGE_PUMP_SETTING = 0x8D;
	static constexpr std::uint8_t DISABLE_CHARGE_PUMP = 0x10;
	static constexpr std::uint8_t ENABLE_CHARGE_PUMP = 0x14;

	static constexpr std::uint8_t CMD_SET_PRE_CHARGE_PERIOD = 0xD9;
	static inline std::uint8_t SET_PRE_CHARGE_PERIOD(std::uint8_t phase1, std::uint8_t phase2){ return ((phase1 << 4)| phase2);}

	static constexpr std::uint8_t CMD_SET_DESELECT_LVL = 0xDB;
	static constexpr std::uint8_t DESELECT_065 = 0x00;
	static constexpr std::uint8_t DESELECT_077 = 0x20;
	static constexpr std::uint8_t DESELECT_083 = 0x30;

	static constexpr std::uint8_t NOP = 0xE3;

	static constexpr std::uint8_t DC_DATA = 0x00;
	static constexpr std::uint8_t DC_CTRL = 0x40;

	/* Overrides */
	void init();
	void fillScreen(std::uint16_t colour);
	void setFont(FontClass *font);
	FontClass getFont();
	void setCursorXY(std::uint8_t x, std::uint8_t y);
	std::pair<std::uint8_t, std::uint8_t> getCursorXY();
	void resetCursor();
	void writeString(std::string str, std::uint16_t colour, std::uint16_t bgcolour);
	void writeChar(char ch, std::uint16_t colour, std::uint16_t bgcolour);
	void drawLine(std::uint8_t x1, std::uint8_t y1, std::uint8_t x2, std::uint8_t y2, std::uint16_t colour);
	void drawPolyline(std::vector<std::pair<std::uint8_t, std::uint8_t>> vertex_list, std::uint16_t colour);
	void drawCircle(std::uint8_t par_x, std::uint8_t par_y, std::uint8_t par_r, std::uint16_t colour);
	void fillCircle(std::uint8_t par_x, std::uint8_t par_y, std::uint8_t par_r, std::uint16_t par_colour);
	void drawRectangle(std::uint8_t x1, std::uint8_t y1, std::uint8_t x2, std::uint8_t y2, std::uint16_t colour);
	void fillRectangle(std::uint16_t x, std::uint16_t y, std:: uint16_t w, std::uint16_t h, std::uint16_t colour);
	std::uint8_t height();
	std::uint8_t width();
	std::uint16_t getColour(std::string colour);
	void refreshScreen();

	/* Derived */

	void resetScreen();
	void setContrast(std::uint8_t value);
	void setDisplayOn(bool onOff);
	void getDisplayOn();

    private:
	std::uint8_t m_i2cAddress;
	I2C_HandleTypeDef *m_i2c;
	const std::uint8_t m_height;
	const std::uint8_t m_width;
	std::uint16_t m_bufferSize;
	std::uint8_t* m_buffer;
	std::uint8_t m_currentX;
	std::uint8_t m_currentY;
	bool m_initialised;
	bool m_diplayOn;
	std::uint32_t m_timeout;
	FontClass* m_font;

	/* Overrides */
	void writeCommand(std::uint8_t cmd);
	void writeData(std::uint8_t* buf, std::uint8_t buf_size);
	void drawPixel(std::uint16_t x, std::uint16_t y, std::uint16_t colour);

	/* Derived */
	void drawPixelBufferXY(std::uint8_t x, std::uint8_t y, std::uint16_t colour);
};
