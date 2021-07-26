//
// Created by juan_ on 16/11/2020.
//

#ifndef MODULEONE_BSP_SOIL_HUMIDITY_SENSOR_H
#define MODULEONE_BSP_SOIL_HUMIDITY_SENSOR_H

#include <stdint.h>

#define INPUT_START 4095.0
#define INPUT_END 2100.0
#define OUTPUT_START 0.0
#define OUTPUT_END 100.0

uint8_t BSP_Soil_Humidity_Get_Humidity(uint16_t adc_value);

#endif //MODULEONE_BSP_SOIL_HUMIDITY_SENSOR_H
