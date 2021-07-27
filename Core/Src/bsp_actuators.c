//
// Created by juan_ on 29/10/2020.
//

#include "bsp_actuators.h"
#include "stdbool.h"
//#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"

#define Cooling_Pump_Pin                GPIO_PIN_14
#define Cooling_Pump_GPIO_Port          GPIOD
#define Irrigation_Pump_Pin             GPIO_PIN_15
#define Irrigation_Pump_GPIO_Port       GPIOD

typedef struct {
    GPIO_TypeDef *Port;
    uint16_t Pin;
    bool isTurnedOn;
} Actuator_TypeDef;

extern void *CoolingPump, *IrrigationPump;

Actuator_TypeDef CoolingPumpActuator = {Cooling_Pump_GPIO_Port, Cooling_Pump_Pin, false};
Actuator_TypeDef IrrigationPumpActuator = {Irrigation_Pump_GPIO_Port, Irrigation_Pump_Pin, false};

void BSP_Actuators_Init() {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOD_CLK_ENABLE();

    GPIO_InitStruct.Pin = Cooling_Pump_Pin|Irrigation_Pump_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(Irrigation_Pump_GPIO_Port, &GPIO_InitStruct);
    if (Cooling_Pump_GPIO_Port != Irrigation_Pump_GPIO_Port) HAL_GPIO_Init(Cooling_Pump_GPIO_Port, &GPIO_InitStruct);

    CoolingPump = &CoolingPumpActuator;
    IrrigationPump = &IrrigationPumpActuator;
}

void BSP_Actuator_On(void *actuator) {
    Actuator_TypeDef *act = (Actuator_TypeDef*) actuator;
    act->isTurnedOn = true;
    HAL_GPIO_WritePin(act->Port, act->Pin, GPIO_PIN_SET);
}

void BSP_Actuator_Off(void *actuator) {
    Actuator_TypeDef *act = (Actuator_TypeDef*) actuator;
    act->isTurnedOn = false;
    HAL_GPIO_WritePin(act->Port, act->Pin, GPIO_PIN_RESET);
}

void BSP_Actuator_Toggle(void *actuator) {
    Actuator_TypeDef *act = (Actuator_TypeDef*) actuator;
    act->isTurnedOn = !act->isTurnedOn;
    HAL_GPIO_TogglePin(act->Port, act->Pin);
}

