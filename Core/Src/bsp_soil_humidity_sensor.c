//
// Created by juan_ on 16/11/2020.
//

#include "bsp_soil_humidity_sensor.h"


uint8_t BSP_Soil_Humidity_Get_Humidity(uint16_t adc_value) { // 0% hum -> 4095 -----  100% hum -> 2100
    if (adc_value > INPUT_START) adc_value = INPUT_START;
    if (adc_value <= INPUT_END) adc_value = INPUT_END + 1;
    return OUTPUT_START + ((OUTPUT_END - OUTPUT_START) / (INPUT_END - INPUT_START)) * (adc_value - INPUT_START);
}