//
// Created by juan_ on 10/11/2020.
//

#ifndef MODULEONE_BSP_TEMP_HUM_H
#define MODULEONE_BSP_TEMP_HUM_H

typedef struct {
    float Temperature;
    float Humidity;
} DHT_DataTypeDef;

void BSP_DHT_Init();

void DHT_GetData (DHT_DataTypeDef *DHT_Data);

#endif //MODULEONE_BSP_TEMP_HUM_H
