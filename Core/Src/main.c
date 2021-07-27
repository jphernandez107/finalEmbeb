#include "main.h"
#include "cmsis_os.h"
#include <stdlib.h>
#include "queue.h"

TIM_HandleTypeDef htim1;

osThreadId_t DisplayHandle;
const osThreadAttr_t Display_attributes = {
  .name = "Display",
  .priority = (osPriority_t) osPriorityHigh7,
  .stack_size = 128 * 4
};

osThreadId_t LuxHandle;
const osThreadAttr_t Lux_attributes = {
  .name = "Lux",
  .priority = (osPriority_t) osPriorityHigh6,
  .stack_size = 128 * 4
};

osThreadId_t AmbientTempHumHandle;
const osThreadAttr_t AmbientTempHum_attributes = {
  .name = "AmbientTempHum",
  .priority = (osPriority_t) osPriorityHigh6,
  .stack_size = 128 * 4
};

osThreadId_t SoilHumHandle;
const osThreadAttr_t SoilHum_attributes = {
  .name = "SoilHum",
  .priority = (osPriority_t) osPriorityHigh6,
  .stack_size = 128 * 4
};

osMessageQueueId_t AmbientQueueHandle;
const osMessageQueueAttr_t AmbientQueue_attributes = {
  .name = "AmbientQueue"
};

QueueHandle_t AmbientTempQueueHandle;
QueueHandle_t AmbientHumQueueHandle;
QueueHandle_t AmbientLuxQueueHandle;

void StartDisplay(void *argument);
void StartLux(void *argument);
void StartAmbientTempHum(void *argument);
void StartSoilHum(void *argument);

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    BSP_Init();

    osKernelInitialize();

    AmbientTempQueueHandle = xQueueCreate(100, sizeof(float));
    AmbientHumQueueHandle = xQueueCreate(100, sizeof(float));
    AmbientLuxQueueHandle = xQueueCreate(100, sizeof(float));

    DisplayHandle = osThreadNew(StartDisplay, NULL, &Display_attributes);
    LuxHandle = osThreadNew(StartLux, NULL, &Lux_attributes);
    AmbientTempHumHandle = osThreadNew(StartAmbientTempHum, NULL, &AmbientTempHum_attributes);
    SoilHumHandle = osThreadNew(StartSoilHum, NULL, &SoilHum_attributes);

    osKernelStart();

    while (1){}
}

void APP_Display_Lux_Meter(float lux) {
    BSP_Display_Set_Cursor(1, 10);
    BSP_Display_Print_Custom_Char(LIGHTBULB);
    BSP_Display_Print("%0.0fLx  ", lux);
}

void APP_Display_Room_Temperature(float RoomTemp) {
    BSP_LCD_Set_Cursor(0, 0);
    BSP_LCD_Print_Custom_Char(THERMOMETER);
    BSP_LCD_Print("%0.1f%c ", RoomTemp, (char) 223);
}

void APP_Display_Soil_Humidity(uint32_t hum) {
    BSP_LCD_Set_Cursor(1, 3);   //0,12
    BSP_LCD_Print("S%d%c", hum, '%');
    BSP_LCD_Print("  ");
}

void APP_Display_Room_Humidity(float RoomHum) {
    if (RoomHum >= 100) RoomHum = 99;
    if (RoomHum <= 0) RoomHum = 0;
    BSP_LCD_Set_Cursor(0, 7);
    BSP_LCD_Print_Custom_Char(DROP);
    BSP_LCD_Print("%0.0f%c ", RoomHum, '%');
}

/**
  * @brief  Function implementing the Display thread.
  * @param  argument: Not used
  * @retval None
  */
void StartDisplay(void *argument)
{
    float tempValue = 0;
    float humValue = 0;
    float luxValue = 0;

  for(;;)
  {
      if (AmbientTempQueueHandle != NULL &&
      xQueueReceive(AmbientTempQueueHandle, &tempValue, 0)){
          xQueueReset(AmbientTempQueueHandle);
          APP_Display_Room_Temperature(tempValue);
      }
      if (AmbientHumQueueHandle != NULL &&
      xQueueReceive(AmbientHumQueueHandle, &humValue, 0)){
          xQueueReset(AmbientHumQueueHandle);
          APP_Display_Room_Humidity(humValue);
      }
      if (AmbientLuxQueueHandle != NULL &&
      xQueueReceive(AmbientLuxQueueHandle, &luxValue, 0)){
          xQueueReset(AmbientLuxQueueHandle);
          APP_Display_Lux_Meter(luxValue);
      }
      APP_Display_Soil_Humidity(BSP_Get_Soil_Humidity());
      vTaskDelay(100);
  }
}

/**
* @brief Function implementing the Lux thread.
* @param argument: Not used
* @retval None
*/
void StartLux(void *argument)
{
  float luxValue = 0;
  for(;;)
  {
      BSP_Get_Lux_Meter(&luxValue);
      xQueueSend(AmbientLuxQueueHandle, (void*) &luxValue, 500);
      vTaskDelay(100);
  }
}

/**
* @brief Function implementing the AmbientTempHum thread.
* @param argument: Not used
* @retval None
*/
void StartAmbientTempHum(void *argument)
{
  float tempValue = 10;
  float humValue = 10;

  for(;;)
  {
    taskENTER_CRITICAL();
    BSP_Read_Ambient_Temp_Hum();
    taskEXIT_CRITICAL();
    tempValue = BSP_Get_Room_Temperature();
    humValue = BSP_Get_Room_Humidity();

    xQueueSend(AmbientTempQueueHandle, (void*) &tempValue, 500);
    xQueueSend(AmbientHumQueueHandle, (void*) &humValue, 500);
    vTaskDelay(1000);
  }
}

/**
* @brief Function implementing the SoilHum thread.
* @param argument: Not used
* @retval None
*/
void StartSoilHum(void *argument)
{
  for(;;)
  {
    BSP_Read_Soil_Humidity();
    vTaskDelay(500);
  }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
