//
// Created by juan_ on 10/11/2020.
//

#include "bsp_temp_hum.h"


/************** MAKE CHANGES HERE ********************/
#include "stm32f4xx_hal.h"

#define DHT_PORT GPIOA          //// DHT Port
#define DHT_PIN GPIO_PIN_0      //// DHT Pin

/*******************************************     NO CHANGES AFTER THIS LINE      ****************************************************/

uint8_t Rh_byte1, Rh_byte2, Temp_byte1, Temp_byte2;
uint16_t SUM; uint8_t Presence = 0;

extern void BSP_Blocking_delay_us(volatile uint16_t uS);

__STATIC_INLINE void delay(volatile uint16_t microseconds) {
    BSP_Blocking_delay_us(microseconds);
//    delay_us(microseconds);
}

void Set_Pin_Output (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void Set_Pin_Input (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void BSP_DHT_Init() {

    __HAL_RCC_GPIOA_CLK_ENABLE();

    Set_Pin_Input(GPIOA, GPIO_PIN_0);
}

/**
 * Iniciamos el proceso de lectura del sensor DHT22
 **/
void DHT_Start (void) {
    /********  Tbe  ********/
    Set_Pin_Output (DHT_PORT, DHT_PIN);  // set the pin as output
    HAL_GPIO_WritePin (DHT_PORT, DHT_PIN, 0);   // pull the pin low
    delay (1200);  // >1ms delay
//    delay_ms(19); //DHT11

    /********  Tgo  ********/
    Set_Pin_Input(DHT_PORT, DHT_PIN);
    delay (30);   // wait for 30us
}


/**
 * Esperamos la respuesta inicial del sensor DHT22
 **/
uint8_t DHT_Check_Response (void) {
    uint8_t Response = 0;
    uint16_t timeOut = 0xfff;
    if (!(HAL_GPIO_ReadPin (DHT_PORT, DHT_PIN)))
    {
        delay (80);
        if ((HAL_GPIO_ReadPin (DHT_PORT, DHT_PIN))) Response = 1;
        else Response = -1;
    }
    while ((HAL_GPIO_ReadPin (DHT_PORT, DHT_PIN)) && timeOut) { timeOut--; }   // wait for the pin to go low

    return Response;
}


/**
 * Leemos la respuesta del sensor DHT22
 **/
uint8_t DHT_Read (void) {
    uint8_t i,j;
    uint16_t timeOut = 0xfff;
    for (j=0;j<8;j++)
    {
        while (!(HAL_GPIO_ReadPin (DHT_PORT, DHT_PIN)) && timeOut) { timeOut--; } // wait for the pin to go high
        timeOut = 0xfff;
        delay (35);   // wait for 40 us
        if (!(HAL_GPIO_ReadPin (DHT_PORT, DHT_PIN)))   // if the pin is low
        {
            i&= ~(1<<(7-j));   // write 0
        }
        else i|= (1<<(7-j));  // if the pin is high, write 1
        while ((HAL_GPIO_ReadPin (DHT_PORT, DHT_PIN)) && timeOut) { timeOut--; }  // wait for the pin to go low
    }
    return i;
}


/**
 * Iniciamos la toma de datos y la almacenamos en nuestra estructura basica
 **/
void DHT_GetData (DHT_DataTypeDef *DHT_Data) {
    DHT_Start ();
    Presence = DHT_Check_Response ();
    Rh_byte1 = DHT_Read ();
    Rh_byte2 = DHT_Read ();
    Temp_byte1 = DHT_Read ();
    Temp_byte2 = DHT_Read ();
    SUM = DHT_Read();

    if (SUM == (uint8_t)(Rh_byte1+Rh_byte2+Temp_byte1+Temp_byte2)){
        DHT_Data->Temperature = ((Temp_byte1<<8)|Temp_byte2) / 10.0;
        DHT_Data->Humidity = ((Rh_byte1<<8)|Rh_byte2) / 10.0;
    }
//      DHT11
//	if (SUM == (uint8_t)(Rh_byte1+Rh_byte2+Temp_byte1+Temp_byte2)){
//        DHT_Data->Temperature = (float)Temp_byte1 / 1.0;
//        DHT_Data->Humidity = Rh_byte1 / 1.0;
//	}
}
