//
// Created by juan_ on 29/10/2020.
//


#include "stm32f4xx.h"
#include <stm32f4xx_hal_tim.h>
#include "stm32f4xx_hal.h"
//#include "bsp_actuators.h"
//#include "bsp_switches.h"
#include "bsp_lcd.h"
#include "bsp_lux_sensor.h"
#include "bsp_temp_hum.h"
#include "bsp_soil_humidity_sensor.h"

/**
* @brief Timer app function
* This function it's called every 10 seconds throw timers
* @param None
* @retval None
*/
extern void APP_Timer10s();

/**
* @brief Timer app function
* This function it's called every 1 second throw timers
* @param None
* @retval None
*/
extern void APP_Timer1000ms();

/**
* @brief Timer app function
* This function it's called every 100 milliseconds throw timers
* @param None
* @retval None
*/
extern void APP_Timer100ms();

/**
* @brief Timer app function
* This function it's called every 10 milliseconds throw timers
* @param None
* @retval None
*/
extern void APP_Timer10ms();

/** Defines the number of ADC channels */
#define ADC_CHANNELS 2

volatile static uint32_t gu32_ticks = 0;
uint32_t adc_values[ADC_CHANNELS];

ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim9;
I2C_HandleTypeDef hi2c1;
DHT_DataTypeDef DHT22 = {0.0f,0.0f};


void SystemClock_Config(void);
void Error_Handler(void);
void HAL_TIM_Init(void);
static void I2C1_Init(void);
static void ADC1_Init(void);
static void DMA_Init(void);

void BSP_Init() {
    HAL_Init();

    SystemClock_Config();

    BSP_DHT_Init();
    HAL_TIM_Init();
    DMA_Init();
    I2C1_Init();
    HAL_TIM_Init();
    ADC1_Init();
//    BSP_Actuators_Init();
//    BSP_Switches_Init();
    lcd16x2_i2c_init(&hi2c1);
    BSP_LCD_Initialize();
    BH1750_Init(&hi2c1);
    BH1750_SetMode(CONTINUOUS_HIGH_RES_MODE_2);
    HAL_ADC_Start_DMA(&hadc1, adc_values, ADC_CHANNELS);
}

/**
* @brief TIM Initialization
* This function configure and initialize the TIMERS
* @param None
* @retval None
*/
void HAL_TIM_Init() {
    gu32_ticks = (HAL_RCC_GetHCLKFreq() / 1000000);
    htim2.Instance = TIM2;

    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    htim2.Init.Prescaler = gu32_ticks; // deberia ser 100 pero HAL_RCC_GetHCLKFreq() devuelve 64MHz -> gu32_ticks = 64
    htim2.Init.Period = 1000;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    if (HAL_TIM_Base_Init(&htim2) != HAL_OK) Error_Handler();
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK) Error_Handler();
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK) Error_Handler();

    htim9.Instance = TIM9;
    htim9.Init.Prescaler = gu32_ticks - 1; // deberia ser 100 pero HAL_RCC_GetHCLKFreq() devuelve 64MHz -> gu32_ticks = 64
    htim9.Init.Period = 65535;
    htim9.Init.CounterMode = TIM_COUNTERMODE_UP;
    if (HAL_TIM_Base_Init(&htim9) != HAL_OK) Error_Handler();
    if (HAL_TIM_ConfigClockSource(&htim9, &sClockSourceConfig) != HAL_OK) Error_Handler();
    if (HAL_TIMEx_MasterConfigSynchronization(&htim9, &sMasterConfig) != HAL_OK) Error_Handler();

    HAL_TIM_Base_Start_IT(&htim2);
    HAL_TIM_Base_Start(&htim9);
}

/**
  * @brief  Period elapsed callback in non-blocking mode
  * @param  htim TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
    static uint16_t App_1000msTimeOut = 1000;
    static uint16_t App_100msTimeOut = 100;
    static uint16_t App_10msTimeOut = 10;
    static uint16_t App_10sTimeOut = 10;
    // Period TIM2 = 1mS
    // Period TIM9 = 1uS
    if (htim->Instance == TIM10) {
        HAL_IncTick();
    }

//    if(htim->Instance == TIM2){
//        if (App_100msTimeOut){
//            App_100msTimeOut--;
//            if (App_100msTimeOut == 0){
//                App_100msTimeOut = 100;
//                APP_Timer100ms();
//                DHT_GetData(&DHT22);
//            }
//        }
//        if (App_10msTimeOut){
//            App_10msTimeOut--;
//            if (App_10msTimeOut == 0){
//                App_10msTimeOut = 10;
//                APP_Timer10ms();
//            }
//        }
//        if (App_1000msTimeOut){
//            App_1000msTimeOut--;
//            if (App_1000msTimeOut == 0){
//                App_1000msTimeOut = 1000;
//                App_10sTimeOut--;
//                APP_Timer1000ms();
//                if(App_10sTimeOut == 0) {
//                    App_10sTimeOut = 10;
//                    APP_Timer10s();
//                }
//            }
//        }
//    }
}

/**
  * @brief  EXTI line detection callbacks.
  * @param  GPIO_Pin Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
//    BSP_Switches_Pin_Interrupt_Callback(GPIO_Pin);
    DHT_GetData(&DHT22);
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void I2C1_Init(void) {
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 100000;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c1) != HAL_OK) Error_Handler();
}

/**
* @brief Blocking delay in milliseconds order
* This function delay the whole program for about X mS time
* @param mS Specifies the time to block the program (In milliseconds)
* @retval None
*/
void BSP_Blocking_delay_ms(int mS) {
//    HAL_Delay(ms);
    while(mS > 0) {
        htim9.Instance->CNT = 0;
        mS--;
        while (htim9.Instance->CNT < 1000);
    }
}

/**
* @brief Blocking delay in microseconds order
* This function delay the whole program for about X uS time
* @param uS Specifies the time to block the program (In microseconds)
* @retval None
*/
void BSP_Blocking_delay_us(volatile uint16_t uS) {
    htim9.Instance->CNT = 0;
    while (htim9.Instance->CNT < uS);
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void ADC1_Init(void) {
    ADC_ChannelConfTypeDef sConfig = {0};

    /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
    */
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.ScanConvMode = ENABLE;
    hadc1.Init.ContinuousConvMode = ENABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 2;
    hadc1.Init.DMAContinuousRequests = ENABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    if (HAL_ADC_Init(&hadc1) != HAL_OK) Error_Handler();
    /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
    */
    sConfig.Channel = ADC_CHANNEL_1;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) Error_Handler();
    /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
    */
    sConfig.Channel = ADC_CHANNEL_2;
    sConfig.Rank = 2;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) Error_Handler();
}

/**
* @brief DMA Initialization
* This function enable and initialize the DMA controller clock
* @param None
* @retval None
*/
static void DMA_Init(void) {
    /* DMA controller clock enable */
    __HAL_RCC_DMA2_CLK_ENABLE();
    /* DMA interrupt init */
    /* DMA2_Stream0_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
}

/**
* @brief CLOCK Initialization
* This function configure and initialize the Main Clock
* @param None
* @retval None
*/
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
    */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 4;
    RCC_OscInitStruct.PLL.PLLN = 100;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 8;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
* @brief Error handler
* This function handles the errors
* @param None
* @retval None
*/
void Error_Handler(void) {}


//**************************************************************************************//

/**
* @brief LCD Set cursor
* This function set the cursor to print on the LCD
* @param row Specifies the row to set the cursor
* @param col Specifies the column to set the cursor
* @retval None
*/
void BSP_Display_Set_Cursor(uint8_t row, uint8_t col) {
    BSP_LCD_Set_Cursor(row, col);
}

/**
* @brief LCD Print
* This function prints on the LCD screen
* @param str Specifies the string to print
* @retval None
*/
void BSP_Display_Print(const char* str, ...) {
    BSP_LCD_Print(str);
}

/**
* @brief LCD Print custom character
* This function prints custom characters
* @param customChar Specifies the custom character to print
* @retval None
*/
void BSP_Display_Print_Custom_Char(char customChar) {
    BSP_LCD_Print_Custom_Char(customChar);
}

/**
* @brief Get lux meter
* This function gets the current value of the lux sensor
* @param *BH1750_lux Specifies the pointer to set the value
* @retval *BH1750_lux Specifies the pointer to set the value
*/
void BSP_Get_Lux_Meter(float *BH1750_lux) {
    BH1750_ReadLight(BH1750_lux);
}

/**
* @brief Get room temperature
* This function gets the current room temperature from the sensor
* @param None
* @retval float Returns the current room temperature
*/
float BSP_Get_Room_Temperature() {
    return DHT22.Temperature;
}

/**
* @brief Get room humidity
* This function gets the current room humidity from the sensor
* @param None
* @retval float Returns the current room humidity
*/
float BSP_Get_Room_Humidity() {
    return DHT22.Humidity;
}

/**
* @brief Get soil humidity
* This function gets the current soil humidity from the sensor
* @param None
* @retval uint32_t Returns the current soil humidity
*/
uint32_t BSP_Get_Soil_Humidity() {
    return 0;//BSP_Soil_Humidity_Get_Humidity(adc_values[0]);
}

/**
* @brief Get Co2 value
* This function gets the current Co2 value from the sensor
* @param None
* @retval uint32_t Returns the current Co2 value on ppm (p/m)
*/
uint16_t BSP_Get_Co2() {
    return 0;//BSP_Co2_Get_PPM(BSP_Co2_Get_Voltage(adc_values[1]));
}

/**
* @brief Read the temperature and humidity
* This function Read the temperature and humidity from the sensor
* @param None
* @retval None
*/
void BSP_Read_Ambient_Temp_Hum() {
    DHT_GetData(&DHT22);
}
