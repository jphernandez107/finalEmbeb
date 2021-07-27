//
// Created by juan_ on 31/10/2020.
//

#ifndef MODULEONE_BSP_ACTUATORS_H
#define MODULEONE_BSP_ACTUATORS_H

void BSP_Actuators_Init();
void BSP_Actuator_On(void *actuator);
void BSP_Actuator_Off(void *actuator);
void BSP_Actuator_Toggle(void *actuator);

#endif //MODULEONE_BSP_ACTUATORS_H
