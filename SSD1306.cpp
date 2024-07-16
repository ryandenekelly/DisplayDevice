#include "SSD1306.hpp"

/** @brief SSD1306 default constructor */
SSD1306::SSD1306() : m_height(0), m_width(0)
{
    m_i2cAddress = 0x00;
    m_i2c = nullptr;
    m_bufferSize = 0;
    m_currentX = 0;
    m_currentY = 0;
    m_initialised = false;
    m_diplayOn = false;
    m_timeout = 0;
}

/** @brief SSD1306 constructor.
 *  @param i2cAddress: The I2C address of SSD1306 device (not shifted).
 *  @param i2c: A pointer to HAL I2C for I2C peripheral connected to SSD1306 device
 *  @param height: The number of vertical pixels.
 *  @param width: The number of horizontal pixels.
 *  @param buffer: The memory buffer to store pixel data.
 */
SSD1306::SSD1306(std::uint8_t i2cAddress, I2C_HandleTypeDef* i2c, std::uint8_t* buffer, std::uint8_t height, std::uint8_t width) : m_height(height), m_width(width)
{
    m_i2cAddress = i2cAddress << 1;
    m_i2c = i2c;
    m_bufferSize = (m_height * m_width) / 8;
    m_buffer = buffer;
    m_currentX = 0;
    m_currentY = 0;
    m_initialised = false;
    m_diplayOn = false;
    m_timeout = 100;
}

/** @brief Initialisation function to setup SSD1306 device. */
void SSD1306::init()
{
    // Delay to allow for device to be ready.
    HAL_Delay(100);

    // Make sure display is off before configuration.
    setDisplayOn(false);

    // Set memory address mode: horizontal mode.
    writeCommand(CMD_SET_MEM_ADDR_MODE);
    writeCommand(ADDR_MODE_HOR);

    // Set page start address: page0
    writeCommand(SET_PAGE_START(0));

    // Set COM scan direction : re-map i.e. from COM[n-1] to COM[0].
    writeCommand(CMD_SET_COM_SCAN_REMAP);

    // Set low column address to 0x00.
    writeCommand(SET_LO_COL_ADDR(0));
    // Set high column address to 0x10.
    writeCommand(SET_HI_COL_ADDR(0));

    // Set display start line address to 0x00.
    writeCommand(SET_DISP_START_LINE(0));

    // Set full contrast.
    setContrast(0xFF);

    // Segment Re-map: column address 127 set to SEG0.
    writeCommand(CMD_SET_SEG_REMAP_127);

    // Set normal colour.
    writeCommand(CMD_NORMAL_DISPLAY);

    // Set MUX ratio
    if(m_height == 128)
    {
	//TODO: fix
	// I don't know what this is(!)
	writeCommand(0xFF);
    }
    else
    {	// Send CMD byte.
	writeCommand(CMD_SET_MUX_RATIO);
    }
    // Send MUX value.
    switch(m_height)
    {
	case 32: // 37 MUX
	    writeCommand(0x1F);
	    break;
	case 64: // 64 MUX
	    writeCommand(0x3F);
	    break;
	case 128: // 64 MUX
	    writeCommand(0x3F);
	    break;
	default:
	    return;
    }

    // Entire display on: follow RAM content.
    writeCommand(CMD_ENTIRE_DISPLAY_ON_RAM);

    // Set display offset:  no offset
    writeCommand(CMD_SET_DISP_OFFSET);
    writeCommand(0x00);

    // Set display clock div.
    writeCommand(CMD_SET_DISPLAY_CLK_DIV); // command
    writeCommand(DIV_RATIO_OSC_FREQ(0x0F, 0x00)); // divide ratio (0xF0)

    writeCommand(CMD_SET_PRE_CHARGE_PERIOD); //set pre-charge period cmd.
    writeCommand(SET_PRE_CHARGE_PERIOD(0x02, 0x02)); // set pre-charge period value (0x22).

    writeCommand(CMD_SET_COM_PINS); // Set COM Pins CMD.
    switch(m_height)
    {
	case 32:
	    writeCommand(SET_COM_PINS(PINS_DIS_REMAP, PINS_SEQ)); // 0x02
	    break;
	case 64:
	    writeCommand(SET_COM_PINS(PINS_DIS_REMAP, PINS_ALT)); // 0x12
	    break;
	case 128:
	    writeCommand(SET_COM_PINS(PINS_DIS_REMAP, PINS_ALT)); // 0x12
	    break;
	default:
	    return;
    }

    // Set Vcomh deselect level
    writeCommand(CMD_SET_DESELECT_LVL); // set vcomh cmd.
    writeCommand(DESELECT_077); //0.77xVcc

    writeCommand(CMD_CHARGE_PUMP_SETTING); //set DC-DC enable cmd
    writeCommand(ENABLE_CHARGE_PUMP); // set DC-DC enable value

    setDisplayOn(true); //turn on SSD1306 panel

    // Clear screen
    fillScreen(getColour("BLACK"));

    // Flush buffer to screen
    refreshScreen();


    // Set default values for screen object
    m_currentX = 0;
    m_currentY = 0;

    m_initialised = true;
}

/** @brief Turn to display on/off.
 *  @param onOff: boolean to turn on (true) or off (false)
 */
void SSD1306::setDisplayOn(bool onOff)
{
    std::uint8_t value = onOff ? CMD_DISPLAY_ON : CMD_DISPLAY_OFF;
    m_diplayOn = onOff;
    writeCommand(value);
}

/** @brief Set display contrast.
 *  @param value: contrast value between 0-255.
 */
void SSD1306::setContrast(std::uint8_t value)
{
    writeCommand(CMD_CONTRAST_CONTROL);
    writeCommand(value);
}

/** @brief Fill the display buffer with a colour.
 *  @param colour: The colour (black or white) to fill with.
 */
void SSD1306::fillScreen(std::uint16_t colour)
{
    std::fill(m_buffer, m_buffer+m_bufferSize, colour != DisplayDevice::White ? 0x00 : 0xFF);
}

/** @brief Refresh the screen and write display buffer to display RAM.
 */
void SSD1306::refreshScreen()
{
    for(std::uint8_t i=0; i< m_height/8; i++)
    {
	writeCommand(CMD_SET_PAGE_START + i); // Write to next page
	writeCommand(LO_COL_ADDR + X_OFFSET_LOWER); // write to low column
	writeCommand(HI_COL_ADDR + X_OFFSET_UPPER); // write to high column
	writeData(&m_buffer[m_width*i], m_width); // write buffer data to RAM.
    }
}

/** @brief Fill the screen with black.
 */
void SSD1306::resetScreen()
{
    fillScreen(getColour("BLACK"));
    refreshScreen();
}

/** @brief Top level function to write to a pixel and specified x,y co-ordinates.
 *  @param x: x co-ordinate to write pixel.
 *  @param y: y co-ordinate to write pixel.
 */
void SSD1306::drawPixel(std::uint16_t x, std::uint16_t y, std::uint16_t colour)
{
    // Bound checking
    if(x >= m_width || y>= m_height)
    {
	return;
    }

    // Call to lower level function.
    drawPixelBufferXY(x,y,colour);
}

/** @brief write a single character to the buffer at the current cursor location.
 *  @param ch: the character to write.
 *  @param colour: the colour of the character you want to write.
 */
void SSD1306::writeChar(char ch, std::uint16_t colour, std::uint16_t bgcolour)
{
    // check for control chars
    if(ch == '\n')
    {
	// move cursor to next line.
	m_currentX = 0;
	m_currentY += m_font->height;
	return;
    }
    // check bounds.
    if(m_width < (m_currentX + m_font->width) || m_height < (m_currentY + m_font->height))
    {
	return;
    }
    // get index of character into stored font array.
    // NB: using latin basic unicode set *FROM* the space char to DEL(replaced with '°').
    std::uint16_t fontIndex = m_font->getCharIndex(ch);
    std::vector<std::uint8_t> fontChar = m_font->getChar(fontIndex);

    for(int i = 0; i < (int)fontChar.size(); i++)
    {
	std::uint8_t fontByte = fontChar[i];
	for(int j = 0; j<m_font->width; j++)
	{
	    if(fontByte & (1 << j))
	    {
		drawPixel(m_currentX+j, m_currentY+i, colour);
	    }
	    else
	    {
		drawPixel(m_currentX+j, m_currentY+i, bgcolour);
	    }
	}
    }
    m_currentX += m_font->width; // move cursor one char width across.
    return;
}

/** @brief Set a new font object.
 *  @param font: new font to set.
 */
void SSD1306::setFont(FontClass *font)
{
    m_font = font;
}

FontClass SSD1306::getFont()
{
    return *m_font;
}

/** @brief set the cursor to a specific x,y position.
 *  @param x: x position to set.
 *  @param y: y position to set.
 */
void SSD1306::setCursorXY(std::uint8_t x, std::uint8_t y)
{
    m_currentX = x;
    m_currentY = y;
}

/** @brief Get the current cursor position.
 *  @return The cursor position as std::pair.
 */
std::pair<std::uint8_t, std::uint8_t> SSD1306::getCursorXY()
{
    return {m_currentX, m_currentY};
}

/** @brief reset the cursor to origin. **/
void SSD1306::resetCursor()
{
    m_currentX = 0;
    m_currentY = 0;
}

/** @brief write a string to the current cursor position.
 *  @param str: std::string you want to write.
 *  @colour: the colour of the text.
 */
void SSD1306::writeString(std::string str, std::uint16_t colour, std::uint16_t bgcolour)
{
    // iterate though the string and write the chars.
    for(auto c : str)
    {
	writeChar(c, colour, bgcolour);
    }
}

/** @brief draw a line from x1,y1 to x2,y2 using Bresenham's line algorithm.
 *  @param x1: origin x co-ordinate.
 *  @param y1: origin y co-ordinate.
 *  @param x2: destination x co-ordinate.
 *  @param y2: destination y co_ordinate.
 *  @return true if successful.
 */
void SSD1306::drawLine(std::uint8_t x1, std::uint8_t y1, std::uint8_t x2, std::uint8_t y2, std::uint16_t colour)
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

/** @brief Draw multi-point poly line using bresenhams line algorithm.
 *  @param vertexList: list of vertices to draw the line through.
 *  @param colour: Colour to draw the line in.
 *  @return true if successful.
 */
void SSD1306::drawPolyline(std::vector<std::pair<std::uint8_t, std::uint8_t>> vertexList, std::uint16_t colour)
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

/** @brief draw a circle using Bresenham's circle algorithm.
 *  @param par_x: x co-ordinate of circle.
 *  @param par_y: y co-ordinate of circle.
 *  @param par_r: radius of circle.
 *  @param colour: colour of the circle.
 */
void SSD1306::drawCircle(std::uint8_t par_x, std::uint8_t par_y, std::uint8_t par_r, std::uint16_t par_colour) {
    std::int32_t x = -par_r;
    std::int32_t y = 0;
    std::int32_t err = 2 - 2 * par_r;
    std::int32_t e2;

    if (par_x >= m_width || par_y >= m_height) {
        return;
    }

    do
    {
        drawPixel(par_x - x, par_y + y, par_colour);
        drawPixel(par_x + x, par_y + y, par_colour);
        drawPixel(par_x + x, par_y - y, par_colour);
        drawPixel(par_x - x, par_y - y, par_colour);
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
}
/** @brief draw a filled circle using Bresenham's circle algorithm.
 *  @param par_x: x co-ordinate of circle.
 *  @param par_y: y co-ordinate of circle.
 *  @param par_r: radius of circle.
 *  @param colour: colour of the circle.
 */
void SSD1306::fillCircle(std::uint8_t par_x, std::uint8_t par_y, std::uint8_t par_r, std::uint16_t par_colour)
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
}

/** @brief Draw a rectangle.
 *  @param x1: origin x co-ordinate.
 *  @param y2: origin y co-ordinate.
 *  @param x2: destination x co-ordinate.
 *  @param y2: destination y co-ordinate.
 *  @param colour: colour of the rectangle.
 */
void SSD1306::drawRectangle(std::uint8_t x1, std::uint8_t y1, std::uint8_t x2, std::uint8_t y2, std::uint16_t colour)
{
    drawLine(x1,y1,x2,y1,colour);
    drawLine(x2,y1,x2,y2,colour);
    drawLine(x2,y2,x1,y2,colour);
    drawLine(x1,y2,x1,y1,colour);
}


/** @brief Draw a filled rectangle.
 *  @param x1: origin x co-ordinate.
 *  @param y2: origin y co-ordinate.
 *  @param x2: destination x co-ordinate.
 *  @param y2: destination y co-ordinate.
 *  @param colour: colour of the rectangle.
 *  @return true if successful.
 */
void SSD1306::fillRectangle(std::uint16_t x, std::uint16_t y, std:: uint16_t w, std::uint16_t h, std::uint16_t colour)
{
    std::uint8_t x_start = ((x<=w) ? x : w);
    std::uint8_t x_end   = ((x<=w) ? w : x);
    std::uint8_t y_start = ((y<=h) ? y : h);
    std::uint8_t y_end   = ((y<=h) ? h : y);

    for (std::uint8_t y= y_start; (y<= y_end)&&(y<m_height); y++)
    {

        for (uint8_t x= x_start; (x<= x_end)&&(x<m_width); x++)
        {
            drawPixel(x, y, colour);
        }
    }
    return;
}

/** @brief Get the screen height in pixels.
 *  @retval The height of the screen.
 */
std::uint8_t SSD1306::height()
{
    return m_height;
}

/** @brief Get the screen width in pixels.
 *  @retval The width of the screen.
 */
std::uint8_t SSD1306::width()
{
    return m_width;
}

/** @brief Function to write commands to SSD1306 via I2C.
 *  @param cmd: The byte command to send to SSD1306.
 */
void SSD1306::writeCommand(std::uint8_t cmd)
{
    HAL_I2C_Mem_Write(m_i2c, m_i2cAddress, DC_DATA, 1, &cmd, 1, m_timeout);
}

/** @brief Function to write data (not commands) to SSD1306 via I2C.
 *  @param buffer: Buffer containing data to send to SSD1306.
 *  @param size: size of data buffer.
 */
void SSD1306::writeData(std::uint8_t* buffer, std::uint8_t size)
{
    HAL_I2C_Mem_Write(m_i2c, m_i2cAddress, DC_CTRL, 1, buffer, size, m_timeout);
}

/** @brief low-level function to write a pixel in the pixel buffer (top-left origin)
 *  @param x: x co-ordinate.
 *  @param y: y o-ordinate.
 *  @param colour: the colour of the pixel.
 */
void SSD1306::drawPixelBufferXY(std::uint8_t x, std::uint8_t y, std::uint16_t colour)
{
    std::uint16_t x_offset = (m_bufferSize - 1) - x;
    std::uint16_t y_offset = (y/8) * m_width;
    std::uint16_t xy_offset = x_offset - y_offset;
    std::uint8_t byte_offset = 7 - (y % 8);
    if(colour == DisplayDevice::White)
    {
	m_buffer[xy_offset] |= (1 << byte_offset);
    }
    else
    {
	m_buffer[xy_offset] &= ~(1 << byte_offset);
    }
}

std::uint16_t SSD1306::getColour(std::string colour)
{
    if(colour == "BLACK")
    {
	return 0x00;
    }
    else if(colour == "WHITE")
    {
	return 0x01;
    }
    else
    {
	return -1;
    }
}
