//
// Created by juan_ on 31/10/2020.
//

#ifndef MODULEONE_BSP_SWITCHES_H
#define MODULEONE_BSP_SWITCHES_H

#include "stm32f4xx.h"

void BSP_Switches_Init();
void BSP_Switches_Pin_Interrupt_Callback(uint16_t GPIO_Pin);


#endif //MODULEONE_BSP_SWITCHES_H
