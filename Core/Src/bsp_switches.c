//
// Created by juan_ on 31/10/2020.
//

#include "bsp_switches.h"

#define Cooling_Switch_Pin              GPIO_PIN_0
#define Cooling_Switch_GPIO_Port        GPIOD
#define Irrigation_Switch_Pin           GPIO_PIN_2
#define Irrigation_Switch_GPIO_Port     GPIOD

typedef struct {
    GPIO_TypeDef *Port;
    uint16_t Pin;
} Switch_TypeDef;

extern void *CoolingSwitch, *IrrigationSwitch;
extern void APP_Switch_Release_Event(void *pressedSwitch);

Switch_TypeDef CoolingPumpSwitch = {Cooling_Switch_GPIO_Port, Cooling_Switch_Pin};
Switch_TypeDef IrrigationPumpSwitch = {Irrigation_Switch_GPIO_Port, Irrigation_Switch_Pin};

void BSP_Switches_Init() {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOD_CLK_ENABLE();

    GPIO_InitStruct.Pin = Cooling_Switch_Pin|Irrigation_Switch_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(Irrigation_Switch_GPIO_Port, &GPIO_InitStruct);
    if (Cooling_Switch_GPIO_Port != Irrigation_Switch_GPIO_Port) HAL_GPIO_Init(Cooling_Switch_GPIO_Port, &GPIO_InitStruct);

    /* EXTI interrupt init*/
    HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);     // Enabling EXTI for Pin 0
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);

    HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);     // Enabling EXTI for Pin 2
    HAL_NVIC_EnableIRQ(EXTI2_IRQn);

    CoolingSwitch = &CoolingPumpSwitch;
    IrrigationSwitch = &IrrigationPumpSwitch;
}


inline void BSP_Switches_Pin_Interrupt_Callback(uint16_t GPIO_Pin) {

    if(GPIO_Pin == CoolingPumpSwitch.Pin){
        APP_Switch_Release_Event(CoolingSwitch);
    } else if (GPIO_Pin == IrrigationPumpSwitch.Pin) {
        APP_Switch_Release_Event(IrrigationSwitch);
    }

}
