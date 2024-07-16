#include "ST7735.hpp"

ST7735::ST7735() : m_width(128), m_height(128)
{
    m_spiHandler = nullptr;
    m_resetPin = -1;
    m_resetPort = nullptr;
    m_nCSPin = -1;
    m_nCSPort = nullptr;
    m_DCPin = -1;
    m_DCPort = nullptr;
    m_currentX = 0;
    m_currentY = 0;
    m_font = nullptr;
}

ST7735::ST7735(SPI_HandleTypeDef* spiHandler, std::uint16_t resetPin, GPIO_TypeDef * resetPort,
	       std::uint16_t nCSPin, GPIO_TypeDef * nCSPort, std::uint16_t DCPin, GPIO_TypeDef * DCPort,
	       std::uint16_t width, std::uint16_t height)  : m_width(width), m_height(height)
{
    m_spiHandler = spiHandler;
    m_resetPin = resetPin;
    m_resetPort = resetPort;
    m_nCSPin = nCSPin;
    m_nCSPort = nCSPort;
    m_DCPin = DCPin;
    m_DCPort = DCPort;
    m_currentX = 0;
    m_currentY = 0;
    m_font = nullptr;
}

void ST7735::select()
{
    HAL_GPIO_WritePin(m_nCSPort, m_nCSPin, GPIO_PIN_RESET);
}

void ST7735::unselect()
{
    HAL_GPIO_WritePin(m_nCSPort, m_nCSPin, GPIO_PIN_SET);
}

void ST7735::reset()
{
    HAL_GPIO_WritePin(m_resetPort, m_resetPin, GPIO_PIN_RESET);
    HAL_Delay(5);
    HAL_GPIO_WritePin(m_resetPort, m_resetPin, GPIO_PIN_SET);
}

void ST7735::writeCommand(std::uint8_t cmd)
{
    HAL_GPIO_WritePin(m_DCPort, m_DCPin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(m_spiHandler, &cmd, sizeof(cmd), HAL_MAX_DELAY);
}

void ST7735::writeData(std::uint8_t* buf, std::uint8_t buf_size)
{
    HAL_GPIO_WritePin(m_DCPort, m_DCPin, GPIO_PIN_SET);
    HAL_SPI_Transmit(m_spiHandler, buf, buf_size, HAL_MAX_DELAY);
}

void ST7735::executeCommandList(const std::uint8_t *addr)
{
    uint8_t numCommands, numArgs;
    uint16_t ms;

    numCommands = *addr++;
    while(numCommands--) {
        uint8_t cmd = *addr++;
        writeCommand(cmd);

        numArgs = *addr++;
        // If high bit set, delay follows args
        ms = numArgs & DELAY;
        numArgs &= ~DELAY;
        if(numArgs) {
            writeData((uint8_t*)addr, numArgs);
            addr += numArgs;
        }

        if(ms) {
            ms = *addr++;
            if(ms == 255) ms = 500;
            HAL_Delay(ms);
        }
    }
}

void ST7735::executeCommand(std::uint8_t cmd, std::uint8_t argsSize, std::vector<std::uint8_t> argsList)
{
    writeCommand(cmd);
    if(argsSize == DELAY)
    {
	std::uint16_t delay_ms = argsList[0];
	if(delay_ms == 255) delay_ms = 500;
	HAL_Delay(delay_ms);
    }
    else
    {
	writeData((uint8_t*)argsList.data(), argsSize);
    }
}

void ST7735::setAddressWindow(std::uint8_t x0, std::uint8_t y0, std::uint8_t x1, std::uint8_t y1)
{
    // column address set
    writeCommand(CMD_CASET);
    uint8_t data[] = { 0x00, x0 + XSTART, 0x00, x1 + XSTART };
    writeData(data, sizeof(data));

    // row address set
    writeCommand(CMD_RASET);
    data[1] = y0 + YSTART;
    data[3] = y1 + YSTART;
    writeData(data, sizeof(data));

    // write to RAM
    writeCommand(CMD_RAMWR);
}

void ST7735::init()
{
    select();
    reset();

    // Init for 7735R, part 1 (red or green tab)
    executeCommand(CMD_SWRESET, DELAY,  {150}); //1: Software reset, 0 args, w/delay , 150 ms delay
    executeCommand(CMD_SLPOUT, DELAY,  {255}); //2: Out of sleep mode, 0 args, w/delay , 500 ms delay
    executeCommand(CMD_FRMCTR1, 3, {0x01, 0x2C, 0x2D});  //3: Frame rate ctrl - normal mode, 3 args: Rate = fosc/(1x2+40) * (LINE+2C+2D)
    executeCommand(CMD_FRMCTR2, 3, {0x01, 0x2C, 0x2D});//4: Frame rate control - idle mode, 3 args:Rate = fosc/(1x2+40) * (LINE+2C+2D)
    executeCommand(CMD_FRMCTR3, 6, {0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D});   //5: Frame rate ctrl - partial mode, 6 args: Dot inversion mode, Line inversion mode
    executeCommand(CMD_INVCTR, 1, {0x07}); //  6: Display inversion ctrl, 1 arg, no delay:, No inversion
    executeCommand(CMD_PWCTR1, 3, {0xA2, 0x02, 0x84});//  7: Power control, 3 args, no delay:, -4.6V, AUTO mode
    executeCommand(CMD_PWCTR2, 1, {0xC5});  //  8: Power control, 1 arg, no delay: VGH25 = 2.4C, VGSEL = -10 VGH = 3 * AVDD
    executeCommand(CMD_PWCTR3, 2, {0x0A, 0x00}); //  9: Power control, 2 args, no delay: Opamp current small, Boost frequency
    executeCommand(CMD_PWCTR4, 2, {0x8A, 0x2A});// 10: Power control, 2 args, no delay: BCLK/2, Opamp current small & Medium low
    executeCommand(CMD_PWCTR5, 2, {0x8A, 0xEE});// 11: Power control, 2 args, no delay:
    executeCommand(CMD_VMCTR1, 1, {0x0E}); // 12: Power control, 1 arg, no delay:
    executeCommand(CMD_INVOFF, 0);  // 13: Don't invert display, no args, no delay
    executeCommand(CMD_MADCTL, 1, {ROTATION}); // 14: Memory access control (directions), 1 arg: row addr/col addr, bottom to top refresh
    executeCommand(CMD_COLMOD, 1, {0x05});  // 15: set color mode, 1 arg, no delay:

    // Init for 7735R, part 2 (1.44" display)
    executeCommand(CMD_CASET, 4, {0x00, 0x00, 0x00, 0x7F}); //  1: Column addr set, 4 args, no delay:, XSTART = 0, XEND = 127
    executeCommand(CMD_RASET, 4, {0x00, 0x00, 0x00, 0x7F}); //  2: Row addr set, 4 args, no delay:, XSTART = 0

    // Init for 7735R, part 3 (red or green tab)
    executeCommand(CMD_GMCTRP1, 16, {0x02, 0x1c, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2d,
				    0x29, 0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10}); //  1: Gamma Adjustments (pos. polarity), 16 args, no delay:
    executeCommand(CMD_GMCTRN1, 16, {0x03, 0x1d, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D,
				    0x2E, 0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10}); //  2: Gamma Adjustments (neg. polarity), 16 args, no delay:
    executeCommand(CMD_NORON, DELAY, {100}); // 3: Normal display on, no args, w/delay, 10 ms delay
    executeCommand(CMD_DISPON, DELAY, {100}); // 4: Main screen turn on, no args w/delay

    unselect();
}

void ST7735::fillRectangle(std::uint16_t x, std::uint16_t y, std:: uint16_t w, std::uint16_t h, std::uint16_t colour)
{
    // clipping
    if((x >= m_width) || (y >= m_height)) return;
    if((x + w - 1) >= m_width) w = m_width - x;
    if((y + h - 1) >= m_height) h = m_height - y;



    select();
    setAddressWindow(x, y, x+w-1, y+h-1);

    uint8_t data[] = { colour >> 8, colour & 0xFF };
    HAL_GPIO_WritePin(m_DCPort, m_DCPin, GPIO_PIN_SET);
    for(std::uint8_t i = h; i > y; i--) {
        for(std::uint8_t j = w; j > x; j--) {
            HAL_SPI_Transmit(m_spiHandler, data, sizeof(data), HAL_MAX_DELAY);
        }
    }

    unselect();
}

void ST7735::fillScreen(std::uint16_t colour)
{
    fillRectangle(0, 0, m_width, m_height, colour);
}

void ST7735::drawPixel(std::uint16_t x, std::uint16_t y, std::uint16_t colour)
{
    if((x >= m_width) || (y >= m_height))
        return;

    select();

    setAddressWindow(x, y, x+1, y+1);
    std::uint8_t data[] = { colour >> 8, colour & 0xFF };
    writeData(data, sizeof(data));

    unselect();
}

void ST7735::writeChar(char ch, std::uint16_t colour, std::uint16_t bgcolour)
{
    setAddressWindow(m_currentX, m_currentY, m_currentX+m_font->width-1, m_currentY+m_font->height-1);

    std::vector<std::uint8_t> fontChar = m_font->getChar(m_font->getCharIndex(ch));
    for(std::uint8_t fontByte : fontChar)
    {

	for(int i = 0; i<m_font->width; i++)
	{
	    if(fontByte & (1 << i))
	    {
		std::uint8_t data[] = { colour >> 8, colour & 0xFF };
		writeData(data, sizeof(data));
	    }
	    else
	    {
		std::uint8_t data[] = { bgcolour >> 8, bgcolour & 0xFF };
		writeData(data, sizeof(data));
	    }
	}
    }
    m_currentX += m_font->width; // move cursor one char width across.
}

void ST7735::writeString(std::string str, std::uint16_t colour, std::uint16_t bgcolour)
{
    select();
    // iterate though the string and write the chars.
    for(auto c : str)
    {
	writeChar(c, colour, bgcolour);
    }
    unselect();
}

void ST7735::drawImage(std::uint16_t x, std::uint16_t y, std::uint16_t w, std::uint16_t h, const std::uint16_t* data)
{
    if((x >= m_width) || (y >= m_height)) return;
    if((x + w - 1) >= m_width) return;
    if((y + h - 1) >= m_height) return;

    select();
    setAddressWindow(x, y, x+w-1, y+h-1);
    writeData((std::uint8_t*)data, sizeof(std::uint16_t)*w*h);
    unselect();
}

void ST7735::invertColors(bool invert)
{
    select();
    writeCommand(invert ? CMD_INVON : CMD_INVOFF);
    unselect();
}

void ST7735::setGamma(std::uint8_t gamma)
{
	select();
	writeCommand(CMD_GAMSET);
	writeData((std::uint8_t *) &gamma, sizeof(gamma));
	unselect();
}

void ST7735::drawLine(std::uint8_t x1, std::uint8_t y1, std::uint8_t x2, std::uint8_t y2, std::uint16_t colour)
{
    std::int32_t deltaX = abs(x2 - x1);
    std::int32_t deltaY = abs(y2 - y1);
    std::int32_t signX = ((x1 < x2) ? 1 : -1);
    std::int32_t signY = ((y1 < y2) ? 1 : -1);

    std::int32_t error = deltaX - deltaY;
    std::int32_t error2;

    // draw last pixel
    drawPixel(x2, y2, colour);

    // while there are still pixels to draw.
    while((x1 != x2) || (y1 != y2))
    {
	// draw current pixel.
	drawPixel(x1, y1, colour);
	error2 = error * 2;

	// determine which side of the slop for the next pixel.
	if(error2 > -deltaY)
	{
	    error -= deltaY;
	    x1 += signX;
	}

	if(error2 < deltaX)
	{
	    error += deltaX;
	    y1 += signY;
	}
    }
    return;
}
void ST7735::drawCircle(std::uint8_t par_x, std::uint8_t par_y, std::uint8_t par_r, std::uint16_t colour)
{
    std::int32_t x = -par_r;
    std::int32_t y = 0;
    std::int32_t err = 2 - 2 * par_r;
    std::int32_t e2;

    if (par_x >= m_width || par_y >= m_height) {
        return;
    }

    do
    {
        drawPixel(par_x - x, par_y + y, colour);
        drawPixel(par_x + x, par_y + y, colour);
        drawPixel(par_x + x, par_y - y, colour);
        drawPixel(par_x - x, par_y - y, colour);
        e2 = err;

        if (e2 <= y)
        {
            y++;
            err = err + (y * 2 + 1);
            if(-x == y && e2 <= x)
            {
                e2 = 0;
            }
        }

        if (e2 > x)
        {
            x++;
            err = err + (x * 2 + 1);
        }
    }
    while (x <= 0);

    return;
}
void ST7735::fillCircle(std::uint8_t par_x, std::uint8_t par_y, std::uint8_t par_r, std::uint16_t par_colour)
{
    std::int32_t x = -par_r;
    std::int32_t y = 0;
    std::int32_t err = 2 - 2 * par_r;
    std::int32_t e2;

    if (par_x >= m_width || par_y >= m_height)
    {
        return;
    }

    do
    {
        for (uint8_t _y = (par_y + y); _y >= (par_y - y); _y--)
        {
            for (uint8_t _x = (par_x - x); _x >= (par_x + x); _x--)
            {
                drawPixel(_x, _y, par_colour);
            }
        }

        e2 = err;
        if (e2 <= y)
        {
            y++;
            err = err + (y * 2 + 1);
            if (-x == y && e2 <= x)
            {
                e2 = 0;
            }
        }

        if (e2 > x)
        {
            x++;
            err = err + (x * 2 + 1);
        }
    }
    while (x <= 0);

    return;
}
void ST7735::drawPolyline(std::vector<std::pair<std::uint8_t, std::uint8_t>> vertexList, std::uint16_t colour)
{
    if(vertexList.size() == 0)
    {
	return;
    }

    for(std::uint16_t i=0; i<vertexList.size()-1; i++)
    {
	std::pair<std::uint8_t, std::uint8_t> v1 = vertexList[i];
	std::pair<std::uint8_t, std::uint8_t> v2 = vertexList[i+1];
	drawLine(v1.first, v1.second, v2.first, v2.second, colour);
    }

    return;
}

void ST7735::drawRectangle(std::uint8_t x1, std::uint8_t y1, std::uint8_t x2, std::uint8_t y2, std::uint16_t colour)
{
    drawLine(x1,y1,x2,y1,colour);
    drawLine(x2,y1,x2,y2,colour);
    drawLine(x2,y2,x1,y2,colour);
    drawLine(x1,y2,x1,y1,colour);
}

void ST7735::setFont(FontClass *font)
{
    m_font = font;
}

FontClass ST7735::getFont()
{
    return *m_font;
}

/** @brief set the cursor to a specific x,y position.
 *  @param x: x position to set.
 *  @param y: y position to set.
 */
void ST7735::setCursorXY(std::uint8_t x, std::uint8_t y)
{
    m_currentX = x;
    m_currentY = y;
}

/** @brief Get the current cursor position.
 *  @return The cursor position as std::pair.
 */
std::pair<std::uint8_t, std::uint8_t> ST7735::getCursorXY()
{
    return {m_currentX, m_currentY};
}

/** @brief reset the cursor to origin. **/
void ST7735::resetCursor()
{
    m_currentX = 0;
    m_currentY = 0;
}

/** @brief Get the screen height in pixels.
 *  @retval The height of the screen.
 */
std::uint8_t ST7735::height()
{
    return m_height;
}

/** @brief Get the screen width in pixels.
 *  @retval The width of the screen.
 */
std::uint8_t ST7735::width()
{
    return m_width;
}

std::uint16_t ST7735::getColour(std::string colour)
{

    if(colour == "BLACK")
    {
	return 0x0000;
    }
    else if(colour == "BLUE")
    {
	return 0x001F;
    }
    else if(colour == "RED")
    {
	return 0xF800;
    }
    else if(colour == "GREEN")
    {
	return 0x07E0;
    }
    else if(colour == "CYAN")
    {
	return 0x07FF;
    }
    else if(colour == "MAGENTA")
    {
	return 0xF81F;
    }
    else if(colour == "YELLOW")
    {
	return 0xFFE0;
    }

    else if(colour == "WHITE")
    {
	return 0xFFFF;
    }
    else
    {
	return -1;
    }
}

void ST7735::refreshScreen()
{

}
