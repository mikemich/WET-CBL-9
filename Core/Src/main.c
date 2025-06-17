/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/*

Code by Michail Andreas Michalopoulos  m.a.michalopoulos@student.tue.nl 
* @attention
  * This code is for educational purposes only and is not intended for production use.
  * It is provided "as is" without any warranties or guarantees.
  * Use at your own risk.
  *
  * This code is part of a project to control a buck converter using an stm32 series microcontroller.
  * It includes functionalities for PWM generation, ADC readings, and UART communication.
  * 
  *
  * 
  * 





















*/



















/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "gpio.h"
#include "tim.h"
#include "adc.h"
#include "usart.h"
#include <string.h>
#include <stdio.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define LEM_SAMPLES  50  // number of samples for moving average

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint32_t pwm_freq_hz = 50000; // 85 kHz
uint32_t duty_cycle_percent = 40; // Default 50%
uint32_t voltage_raw = 0;
uint32_t current_raw = 0;
uint32_t Deadtimevar = 2;
float voltage_adc = 0.0f;
float voltage_input = 0.0f;
float current_adc = 0.0f;
float current_value = 0.0f;
uint8_t buffer1[] = "works";
#define INPUT_PIN   GPIO_PIN_4
#define INPUT_PORT  GPIOB

#define OUTPUT_PIN  GPIO_PIN_5
#define OUTPUT_PORT GPIOB
float duty_cycle = 35.0f;     // Start from 0
float prev_voltage = 0.0f;   // Previous loop voltage
float step = 0.45f;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void Configure_PWM(uint32_t freq_hz, uint32_t duty_percent);
void Play_melody(void);
void Play_Rickroll(void);
void MaxVoltage_Update(float pv_voltage);


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM1_Init();
  MX_ADC1_Init();
  MX_USART2_UART_Init();
  MX_ADC2_Init();
  /* USER CODE BEGIN 2 */
  Configure_PWM(pwm_freq_hz, duty_cycle_percent);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
  HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2); // for complementary PA12
  __HAL_TIM_MOE_ENABLE(&htim1); // Required for complementary outputs

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
//  uint32_t sum_current_adc = 0;
//  uint32_t avg_current_adc = 0;
  int calint = 0;
  float calib = 0;
 // HAL_GPIO_WritePin(OUTPUT_PORT, OUTPUT_PIN, GPIO_PIN_SET);
  while (1)
  {


	  Play_Melody();
	 // Play_Rickroll();
	   HAL_Delay(10); // optional debounce

	  //Play_Melody();
	  uint32_t sum_current_adc = 0;
	  uint32_t avg_current_adc = 0;
	  uint32_t adc_raw;
	  float adc_voltage = 0.0f;
	  float input_voltage = 0.0f;
	  uint32_t adc_current;

	  HAL_ADC_Start(&hadc1);// adc1
	  HAL_ADC_PollForConversion(&hadc1, 10);
	  uint32_t val1 = HAL_ADC_GetValue(&hadc1);

	  HAL_ADC_Start(&hadc2);//adc2
	  HAL_ADC_PollForConversion(&hadc2, 10);
	  uint32_t val2 = HAL_ADC_GetValue(&hadc2);
	  adc_raw = val1;
	 // adc_current = val2;

	  HAL_ADC_Start(&hadc2);//adc2
	 	  HAL_ADC_PollForConversion(&hadc2, 10);
	 	 //
	 float diff1= HAL_ADC_GetValue(&hadc2);
	 HAL_ADC_Start(&hadc2);//adc2
		  HAL_ADC_PollForConversion(&hadc2, 10);

	 float diff2= HAL_ADC_GetValue(&hadc2);
	 HAL_ADC_Start(&hadc2);//adc2
		  HAL_ADC_PollForConversion(&hadc2, 10);

	 float diff3= HAL_ADC_GetValue(&hadc2);
	 HAL_ADC_Start(&hadc2);//adc2
		  HAL_ADC_PollForConversion(&hadc2, 10);

	 float diff4= HAL_ADC_GetValue(&hadc2);
	 HAL_ADC_Start(&hadc2);//adc2
		  HAL_ADC_PollForConversion(&hadc2, 10);


		  float diff5= HAL_ADC_GetValue(&hadc2);
		  HAL_ADC_Start(&hadc2);//adc2
		 	  HAL_ADC_PollForConversion(&hadc2, 10);

		  float diff6= HAL_ADC_GetValue(&hadc2);

	 float n1 = diff1 - diff2;
	 float n2 = diff3 - diff4;
	 float n3 = diff5 - diff6;
	 float ma_sum = 0.33*(n1*n1+n2*n2+n3*n3) * 300;

	  for (int i = 0; i < 6; i++) {
	       // assuming ADC2 = LEM
	      HAL_ADC_PollForConversion(&hadc2, 10);
	      sum_current_adc += val2;
	  }
	  avg_current_adc = sum_current_adc / LEM_SAMPLES;
	  float vadc = (val2 / 4095.0f) * 3.3f;
	  float zero = 0;           // LEM's zero-current point in volts
	  float sensitivity = 1;   // Example: 20 mV/A

	  float current = (ma_sum);

	  float input_current = 2* current;
	  char msg3[64];
	  snprintf(msg3, sizeof(msg3), "Current in avg: %lu | Current: %.2f A\r\n", avg_current_adc, current);
	  HAL_UART_Transmit(&huart2, (uint8_t*)msg3, strlen(msg3), HAL_MAX_DELAY);
//
	  adc_voltage = (val1 / 4095.0f) * 3.3f;
	  input_voltage = adc_voltage * ((100.0f + 6.8f) / 6.8f);  // Adjust resistors here
	  val1 = input_voltage + 1.5;

	  char msg[64];
	  if (calint>5){
	 // val2 = (val2-calib) / 1;
	  }
	  //val2 = calint;
	  snprintf(msg, sizeof(msg), "ADC1: %lu | ADC2: %lu ||", val1, val2);
	  HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
	  if (calint < 5){
		  calib = calib + val2;
	  }
	  if (calint ==5){
		  calib = calib/(calint + 1);
	  }
	  calint++;
//== ADC 1.8v 950 6.39     |A
//== ADC2.6 8.77 1300	 |A
//== ADC	 |A
//== ADC	 |A
//== ADC	 |A
//== ADC	 |A


    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  if (val1 >= 65)
	      {
	        // If PB4 is HIGH: stop PWM, set PB5 HIGH
	       // HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
		  	Configure_PWM(pwm_freq_hz, 0);
		  	char msg5 [64];
		  	snprintf(msg5, sizeof(msg5), "\nNo load!!\n");
		  	HAL_Delay(5);
	        HAL_GPIO_WritePin(OUTPUT_PORT, OUTPUT_PIN, GPIO_PIN_SET);

	        /*
	        while(1){

	        	HAL_Delay(500);

	        	Configure_PWM(pwm_freq_hz, duty_cycle_percent);
	        	HAL_GPIO_WritePin(OUTPUT_PORT, OUTPUT_PIN, GPIO_PIN_RESET);

	        	// take 3 readings
	        	HAL_ADC_Start(&hadc1);// adc1
	        		  HAL_ADC_PollForConversion(&hadc1, 10);
	        		  uint32_t val1 = HAL_ADC_GetValue(&hadc1);
	        		  adc_voltage = (val1 / 4095.0f) * 3.3f;
	        		  input_voltage = adc_voltage * ((100.0f + 6.8f) / 6.8f);  // Adjust resistors here
	        		  val1 = input_voltage + 1.5;

	        	// if load break
	        	if(val1<28){

	    		  	snprintf(msg5, sizeof(msg5), "\nLoad detected again!!\n");
	    		  	break;
	        	}
*/

	        	HAL_Delay(3000);



	        }

	        Configure_PWM(pwm_freq_hz, duty_cycle_percent);
	        HAL_Delay(5);
	       	HAL_GPIO_WritePin(OUTPUT_PORT, OUTPUT_PIN, GPIO_PIN_RESET);


	      }





  /* USER CODE END 3 */
}









/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
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
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_TIM1|RCC_PERIPHCLK_ADC12;
  PeriphClkInit.Adc12ClockSelection = RCC_ADC12PLLCLK_DIV1;
  PeriphClkInit.Tim1ClockSelection = RCC_TIM1CLK_HCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void Configure_PWM(uint32_t freq_hz, uint32_t duty_percent)
{
  uint32_t timer_clk = HAL_RCC_GetPCLK2Freq(); // TIM1 on APB2
  uint32_t prescaler = 0;
  uint32_t period = (timer_clk / freq_hz) - 1;
  uint32_t pulse = (period + 1) * duty_percent / 100;

  htim1.Init.Prescaler = prescaler;
  htim1.Init.Period = period;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  HAL_TIM_PWM_Init(&htim1);

  TIM_OC_InitTypeDef sConfigOC = {0};
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = pulse;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2);

  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = Deadtimevar;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_ENABLE;
  HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig);

  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
  HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2); // Complementary
}

void Play_Melody(void)
{
  // Frequencies (Hz) for Star Wars intro (short version)
  uint32_t melody[] = {
    440, 440, 440, 349, 440, 523, 440, 349, 440, 523, 440, 0, // first bar
    659, 659, 659, 698, 659, 587, 523, 440, 349 // second bar (simplified)
  };

  // Durations in ms for each note (adjust for rhythm)
  uint16_t duration[] = {
    500, 500, 500, 350, 150, 500, 350, 150, 500, 350, 150, 500,
    500, 500, 500, 350, 150, 500, 350, 150, 500
  };

  uint8_t num_notes = sizeof(melody) / sizeof(melody[0]);

  for (uint8_t i = 0; i < num_notes; i++)
  {
    if (melody[i] == 0) {
      // Rest
      HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
      HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);
      HAL_Delay(duration[i]);
    } else {
      uint32_t timer_clk = HAL_RCC_GetPCLK2Freq();
      uint32_t period = (timer_clk / melody[i]) - 1;
      uint32_t pulse = (period + 1) * 50 / 100; // 50% duty

      htim1.Init.Period = period;
      htim1.Init.Prescaler = 0;
      HAL_TIM_PWM_Init(&htim1);

      TIM_OC_InitTypeDef sConfigOC = {0};
      sConfigOC.OCMode = TIM_OCMODE_PWM1;
      sConfigOC.Pulse = pulse;
      sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
      sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
      sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
      sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
      sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
      HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2);

      HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
      HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
      __HAL_TIM_MOE_ENABLE(&htim1);

      HAL_Delay(duration[i]);
    }
  }

  // Stop at the end
  HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
  HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);
}

void Play_Rickroll(void)
{
  // Rick Astley "Never Gonna Give You Up" opening notes (approximate)
  // Frequencies in Hz, duration in ms
  uint32_t melody[]   = { 659, 659, 659, 523, 587, 659, 784, 784, 659, 523, 587, 659 }; // E E E C D E G G E C D E
  uint32_t duration[] = { 300, 300, 300, 300, 300, 600, 300, 300, 300, 300, 300, 600 };

  uint8_t num_notes = sizeof(melody) / sizeof(melody[0]);

  for (uint8_t i = 0; i < num_notes; i++)
  {
    // Calculate TIM1 parameters for this note
    uint32_t timer_clk = HAL_RCC_GetPCLK2Freq();
    uint32_t period = (timer_clk / melody[i]) - 1;
    uint32_t pulse = (period + 1) / 2; // 50% duty

    // Update PWM settings
    htim1.Init.Period = period;
    htim1.Init.Prescaler = 0;
    HAL_TIM_PWM_Init(&htim1);

    TIM_OC_InitTypeDef sConfigOC = {0};
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = pulse;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2);

    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
    __HAL_TIM_MOE_ENABLE(&htim1);

    HAL_Delay(duration[i]); // play this note
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2); // small pause between notes
    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);
    HAL_Delay(50);
  }
}






int dir = 1;
float best_volt = 0;
float best_DC = 20.0f;
int counter = 0;
int sweeped = 0;
void MaxVoltage_Update(float pv_voltage)
{
	counter++;
    // Basic perturb and observe to reach max voltage

	if(sweeped == 0){
		duty_cycle=20.0;

		for(; duty_cycle < 70; duty_cycle=duty_cycle+0.3 ){
		// Apply to PWM
			 HAL_ADC_Start(&hadc1);// adc1
				  HAL_ADC_PollForConversion(&hadc1, 10);
				  pv_voltage  = HAL_ADC_GetValue(&hadc1);

			Configure_PWM(pwm_freq_hz, (uint32_t)duty_cycle);
			HAL_Delay(30);
			//;take reading
			if(pv_voltage >= best_volt){
				best_volt = pv_voltage;
				best_DC = duty_cycle;
			}

		}
	sweeped=1;
	}else if(pv_voltage < best_volt - 3)//change occured in voltage
		sweeped = 0;

    // Print status
    char msg[64];
    snprintf(msg, sizeof(msg), "PV Volt: %.2f V | Duty: %.2f %%\r\n", pv_voltage, duty_cycle);
    HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

    // Update reference for next iteration

	Configure_PWM(pwm_freq_hz, (uint32_t)best_DC);
	HAL_Delay(30);

}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
