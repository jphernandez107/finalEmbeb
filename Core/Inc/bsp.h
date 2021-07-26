//
// Created by juan_ on 31/10/2020.
//

#ifndef MODULEONE_BSP_H
#define MODULEONE_BSP_H

#include "stdint.h"

/**
* @brief BSP Initialization
* This function initialize every other bsp init
* @param None
* @retval None
*/
void BSP_Init();

/**
* @brief Blocking delay in milliseconds order
* This function delay the whole program for about X mS time
* @param mS Specifies the time to block the program (In milliseconds)
* @retval None
*/
void BSP_Blocking_delay_ms(int ms);

/**
* @brief LCD Set cursor
* This function set the cursor to print on the LCD
* @param row Specifies the row to set the cursor
* @param col Specifies the column to set the cursor
* @retval None
*/
void BSP_Display_Set_Cursor(uint8_t row, uint8_t col);

/**
* @brief LCD Print
* This function prints on the LCD screen
* @param str Specifies the string to print
* @retval None
*/
void BSP_Display_Print(const char* str, ...);

/**
* @brief LCD Print custom character
* This function prints custom characters
* @param customChar Specifies the custom character to print
* @retval None
*/
void BSP_Display_Print_Custom_Char(char customChar);

/**
* @brief Get lux meter
* This function gets the current value of the lux sensor
* @param *BH1750_lux Specifies the pointer to set the value
* @retval *BH1750_lux Specifies the pointer to set the value
*/
void BSP_Get_Lux_Meter(float *BH1750_lux);

/**
* @brief Get room temperature
* This function gets the current room temperature from the sensor
* @param None
* @retval float Returns the current room temperature
*/
float BSP_Get_Room_Temperature();

/**
* @brief Get room humidity
* This function gets the current room humidity from the sensor
* @param None
* @retval float Returns the current room humidity
*/
float BSP_Get_Room_Humidity();

/**
* @brief Get soil humidity
* This function gets the current soil humidity from the sensor
* @param None
* @retval uint32_t Returns the current soil humidity
*/
uint32_t BSP_Get_Soil_Humidity();

/**
* @brief Get Co2 value
* This function gets the current Co2 value from the sensor
* @param None
* @retval uint32_t Returns the current Co2 value on ppm (p/m)
*/
uint16_t BSP_Get_Co2();

/**
* @brief Read the temperature and humidity
* This function Read the temperature and humidity from the sensor
* @param None
* @retval None
*/
void BSP_Read_Ambient_Temp_Hum();

/**
 * @brief Set cursor position
 * @param[in] row - 0 or 1 for line1 or line2
 * @param[in] col - 0 - 15 (16 columns LCD)
 */
inline void BSP_LCD_Set_Cursor(uint8_t row, uint8_t col);

/**
* @brief LCD Print
* This function prints on the LCD screen
* @param str Specifies the string to print
* @retval None
*/
void BSP_LCD_Print(const char* str, ...);

/**
* @brief LCD Print custom character
* This function prints custom characters
* @param customChar Specifies the custom character to print
* @retval None
*/
void BSP_LCD_Print_Custom_Char(char customChar);

enum BSP_LCD_Custom_Char {LIGHTBULB, THERMOMETER, DROP, CO2_1, CO2_2, CO2_3, PPM_1, PPM_2};

/**
* @brief Actuator Turn On
* This function turns on the actuator
* @param actuator Specifies the actuator to be turned on
* @retval None
*/
void BSP_Actuator_On(void *actuator);

/**
* @brief Actuator Turn Off
* This function turns off the actuator
* @param actuator Specifies the actuator to be turned off
* @retval None
*/
void BSP_Actuator_Off(void *actuator);

/**
* @brief Actuator Toggle
* This function toggles the actuator
* @param actuator Specifies the actuator to be toggled
* @retval None
*/
void BSP_Actuator_Toggle(void *actuator);




#endif //MODULEONE_BSP_H
