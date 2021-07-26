//
// Created by juan_ on 6/11/2020.
//

#include "bsp_lcd.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

/* LCD Commands */
#define LCD_CLEARDISPLAY    0x01
#define LCD_RETURNHOME      0x02
#define LCD_ENTRYMODESET    0x04
#define LCD_DISPLAYCONTROL  0x08
#define LCD_CURSORSHIFT     0x10
#define LCD_FUNCTIONSET     0x20
#define LCD_SETCGRAMADDR    0x40
#define LCD_SETDDRAMADDR    0x80

/* Commands bitfields */
//1) Entry mode Bitfields
#define LCD_ENTRY_SH      0x01
#define LCD_ENTRY_ID      0x02
//2) Display control
#define LCD_DISPLAY_B     0x01
#define LCD_DISPLAY_C     0x02
#define LCD_DISPLAY_D     0x04
//3) Shift control
#define LCD_SHIFT_RL      0x04
#define LCD_SHIFT_SC      0x08
//4) Function set control
#define LCD_FUNCTION_F    0x04
#define LCD_FUNCTION_N    0x08
#define LCD_FUNCTION_DL   0x10

/* I2C Control bits */
#define LCD_RS        (1 << 0)
#define LCD_RW        (1 << 1)
#define LCD_EN        (1 << 2)
#define LCD_BK_LIGHT  (1 << 3)

/* Library variables */
static I2C_HandleTypeDef* lcd16x2_i2cHandle;
static uint8_t LCD_I2C_SLAVE_ADDRESS=0;
extern void BSP_Blocking_delay_ms(int ms);
#define LCD_I2C_SLAVE_ADDRESS_0  0x4E
#define LCD_I2C_SLAVE_ADDRESS_1  0x7E

uint8_t customChars[CUSTOM_CHAR_ARRAY_SIZE][CUSTOM_CHAR_ARRAY_BYTE_SIZE] = {
        {0b01110,0b10001,0b10001,0b01010,0b01110,0b00000,0b01110,0b00000},  // Lightbulb
        {0b00100,0b01010,0b01010,0b01110,0b01110,0b11111,0b11111,0b01110},  // Thermometer
        {0b00100,0b00100,0b01110,0b01110,0b11111,0b11101,0b11001,0b01110},  // Drop
        {0b00000,0b00000,0b00111,0b01000,0b01000,0b00111,0b00000,0b00000},  // co2-1
        {0b00000,0b00000,0b01110,0b10001,0b10001,0b01110,0b00000,0b00000},  // co2-2
        {0b00000,0b00000,0b00000,0b11100,0b00100,0b11100,0b10000,0b11100},  // co2-3
        {0b01110,0b01001,0b01110,0b01000,0b01001,0b00010,0b00100,0b00000},  // ppm-1
        {0b00100,0b01000,0b10000,0b00000,0b11010,0b10101,0b10101,0b10001}  // ppm-2
};

uint8_t initCustomChars[6][CUSTOM_CHAR_ARRAY_BYTE_SIZE] = {
        {0b00000,0b00000,0b00000,0b00000,0b00000,0b00000,0b00000,0b00000},  // Cero
        {0b10000,0b10000,0b10000,0b10000,0b10000,0b10000,0b10000,0b10000},  // Twenty
        {0b11000,0b11000,0b11000,0b11000,0b11000,0b11000,0b11000,0b11000},  // Forty
        {0b11100,0b11100,0b11100,0b11100,0b11100,0b11100,0b11100,0b11100},  // Sixty
        {0b11110,0b11110,0b11110,0b11110,0b11110,0b11110,0b11110,0b11110},  // Eighty
        {0b11111,0b11111,0b11111,0b11111,0b11111,0b11111,0b11111,0b11111}   // Hundred
};

/* Private functions */
static void lcd16x2_i2c_sendCommand(uint8_t command)
{
    const uint8_t command_0_3 = (0xF0 & (command<<4));
    const uint8_t command_4_7 = (0xF0 & command);
    uint8_t i2cData[4] =
            {
                    command_4_7 | LCD_EN | LCD_BK_LIGHT,
                    command_4_7 | LCD_BK_LIGHT,
                    command_0_3 | LCD_EN | LCD_BK_LIGHT,
                    command_0_3 | LCD_BK_LIGHT,
            };
    HAL_I2C_Master_Transmit(lcd16x2_i2cHandle, LCD_I2C_SLAVE_ADDRESS, i2cData, 4, 200);
}

static void lcd16x2_i2c_sendData(uint8_t data)
{
    const uint8_t data_0_3 = (0xF0 & (data<<4));
    const uint8_t data_4_7 = (0xF0 & data);
    uint8_t i2cData[4] =
            {
                    data_4_7 | LCD_EN | LCD_BK_LIGHT | LCD_RS,
                    data_4_7 | LCD_BK_LIGHT | LCD_RS,
                    data_0_3 | LCD_EN | LCD_BK_LIGHT | LCD_RS,
                    data_0_3 | LCD_BK_LIGHT | LCD_RS,
            };
    HAL_I2C_Master_Transmit(lcd16x2_i2cHandle, LCD_I2C_SLAVE_ADDRESS, i2cData, 4, 200);
}


/**
 * @brief Initialise LCD16x2
 * @param[in] *pI2cHandle - pointer to HAL I2C handle
 */
bool lcd16x2_i2c_init(I2C_HandleTypeDef *pI2cHandle)
{
    BSP_Blocking_delay_ms(50);
    lcd16x2_i2cHandle = pI2cHandle;
    if(HAL_I2C_IsDeviceReady(lcd16x2_i2cHandle, LCD_I2C_SLAVE_ADDRESS_0, 5, 500) != HAL_OK)
    {
        if(HAL_I2C_IsDeviceReady(lcd16x2_i2cHandle, LCD_I2C_SLAVE_ADDRESS_1, 5, 500) != HAL_OK)
        {
            return false;
        }
        else
        {
            LCD_I2C_SLAVE_ADDRESS = LCD_I2C_SLAVE_ADDRESS_1;
        }
    }
    else
    {
        LCD_I2C_SLAVE_ADDRESS = LCD_I2C_SLAVE_ADDRESS_0;
    }
    //Initialise LCD for 4-bit operation
    //1. Wait at least 15ms
    BSP_Blocking_delay_ms(45);
    //2. Attentions sequence
    lcd16x2_i2c_sendCommand(0x30);
    BSP_Blocking_delay_ms(5);
    lcd16x2_i2c_sendCommand(0x30);
    BSP_Blocking_delay_ms(1);
    lcd16x2_i2c_sendCommand(0x30);
    BSP_Blocking_delay_ms(8);
    lcd16x2_i2c_sendCommand(0x20);
    BSP_Blocking_delay_ms(8);

    lcd16x2_i2c_sendCommand(LCD_FUNCTIONSET | LCD_FUNCTION_N);
    BSP_Blocking_delay_ms(1);
    lcd16x2_i2c_sendCommand(LCD_DISPLAYCONTROL);
    BSP_Blocking_delay_ms(1);
    lcd16x2_i2c_sendCommand(LCD_CLEARDISPLAY);
    BSP_Blocking_delay_ms(3);
    lcd16x2_i2c_sendCommand(0x04 | LCD_ENTRY_ID);
    BSP_Blocking_delay_ms(1);
    lcd16x2_i2c_sendCommand(LCD_DISPLAYCONTROL | LCD_DISPLAY_D);
    BSP_Blocking_delay_ms(3);

    return true;
}

/**
 * @brief Set cursor position
 * @param[in] row - 0 or 1 for line1 or line2
 * @param[in] col - 0 - 15 (16 columns LCD)
 */
void BSP_LCD_Set_Cursor(uint8_t row, uint8_t col)
{
    uint8_t maskData;
    maskData = (col)&0x0F;
    if(row==0)
    {
        maskData |= (0x80);
        lcd16x2_i2c_sendCommand(maskData);
    }
    else
    {
        maskData |= (0xc0);
        lcd16x2_i2c_sendCommand(maskData);
    }
}

/**
 * @brief Move to beginning of 1st line
 */
void lcd16x2_i2c_1stLine(void)
{
    BSP_LCD_Set_Cursor(0, 0);
}
/**
 * @brief Move to beginning of 2nd line
 */
void lcd16x2_i2c_2ndLine(void)
{
    BSP_LCD_Set_Cursor(1, 0);
}

/**
 * @brief Select LCD Number of lines mode
 */
void lcd16x2_i2c_TwoLines(void)
{
    lcd16x2_i2c_sendCommand(LCD_FUNCTIONSET | LCD_FUNCTION_N);
}
void lcd16x2_i2c_OneLine(void)
{
    lcd16x2_i2c_sendCommand(LCD_FUNCTIONSET);
}

/**
 * @brief Cursor ON/OFF
 */
void lcd16x2_i2c_cursorShow(bool state)
{
    if(state)
    {
        lcd16x2_i2c_sendCommand(LCD_DISPLAYCONTROL | LCD_DISPLAY_B | LCD_DISPLAY_C | LCD_DISPLAY_D);
    }
    else
    {
        lcd16x2_i2c_sendCommand(LCD_DISPLAYCONTROL | LCD_DISPLAY_D);
    }
}

/**
 * @brief Display clear
 */
void lcd16x2_i2c_clear(void)
{
    lcd16x2_i2c_sendCommand(LCD_CLEARDISPLAY);
    BSP_Blocking_delay_ms(3);
}

/**
 * @brief Display ON/OFF, to hide all characters, but not clear
 */
void lcd16x2_i2c_display(bool state)
{
    if(state)
    {
        lcd16x2_i2c_sendCommand(LCD_DISPLAYCONTROL | LCD_DISPLAY_B | LCD_DISPLAY_C | LCD_DISPLAY_D);
    }
    else
    {
        lcd16x2_i2c_sendCommand(LCD_DISPLAYCONTROL | LCD_DISPLAY_B | LCD_DISPLAY_C);
    }
}

/**
 * @brief Shift content to right
 */
void lcd16x2_i2c_shiftRight(uint8_t offset)
{
    for(uint8_t i=0; i<offset;i++)
    {
        lcd16x2_i2c_sendCommand(0x1c);
    }
}

/**
 * @brief Shift content to left
 */
void lcd16x2_i2c_shiftLeft(uint8_t offset)
{
    for(uint8_t i=0; i<offset;i++)
    {
        lcd16x2_i2c_sendCommand(0x18);
    }
}

/**
 * @brief Print to display
 */
void BSP_LCD_Print(const char* str, ...)
{
    char stringArray[20];
    va_list args;
    va_start(args, str);
    vsprintf(stringArray, str, args);
    va_end(args);
    for(uint8_t i=0;  i<strlen(stringArray) && i<16; i++)
    {
        lcd16x2_i2c_sendData((uint8_t)stringArray[i]);
    }
}

void lcd16x2_i2c_create_char(uint8_t location, uint8_t newChar[]) {
    location &= 0x7;
    lcd16x2_i2c_sendCommand(LCD_SETCGRAMADDR | (location << 3));
    for (int i=0; i<8; i++) {
        lcd16x2_i2c_sendData(newChar[i]);
    }
}

void lcd16x2_i2c_create_custom_chars(){
    for (int i=0; i<CUSTOM_CHAR_ARRAY_SIZE; i++) {
        lcd16x2_i2c_create_char(i, customChars[i]);
    }
}

void lcd16x2_i2c_create_init_custom_chars(){
    for (int i=0; i<INIT_CUSTOM_CHAR_ARRAY_SIZE; i++) {
        lcd16x2_i2c_create_char(i, initCustomChars[i]);
    }
}

void BSP_LCD_Print_Custom_Char(const char customChar) {
    lcd16x2_i2c_sendData(customChar);
}

void BSP_LCD_Initialize() {
    int delay = 1000 / 16 / 6;
    lcd16x2_i2c_clear();
    lcd16x2_i2c_create_init_custom_chars();
    BSP_LCD_Set_Cursor(0, 0);
    BSP_LCD_Print("Initializing...");
    for (int i=0; i<16; i++) {
        for (int j=0; j<INIT_CUSTOM_CHAR_ARRAY_SIZE; j++) {
            BSP_LCD_Set_Cursor(1, i);
            BSP_LCD_Print_Custom_Char(j);
            BSP_Blocking_delay_ms(delay);
        }
    }
    lcd16x2_i2c_clear();
    lcd16x2_i2c_create_custom_chars();
}
