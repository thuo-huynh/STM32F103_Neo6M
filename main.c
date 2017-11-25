
/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"


/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart3;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART3_UART_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
	



/* USER CODE END PFP */

/*
  *
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE  
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART */
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 100);


    return ch;
}

char ch;
uint8_t char_counter = 0;
uint8_t char_counter_tmp = 0;
char x_text [100];
uint8_t flag_x = 0;
char st1[] = "AT+HTTPPARA=URL,http://www.vmig2016.16mb.com/get_location.php?raw_var=";
char st2[] = "\r\n";

void S_delay_ms(uint32_t ms){
	long rr = ms*300;
	while (rr != 0){
		rr--;
	}
}


void scan_for_dots(){
	uint8_t i = 0;
	while (i!=26){
		if (x_text [i] == ','){
			x_text [i] = 'y';
		}
		i++;
	}
}


uint8_t comm_1[] = "AT+SAPBR=3,1,Contype,GPRS\r\n";
uint8_t comm_2[] = "AT+SAPBR=3,1,APN,YOUR_APN_HERE\r\n";
uint8_t comm_3[] = "AT+SAPBR=1,1\r\n";
uint8_t comm_4[] = "AT+HTTPINIT\r\n";
uint8_t comm_5[] = "AT+HTTPPARA=CID,1\r\n";

void init_devs(){
	//GSM Power On
	S_delay_ms(400000);
	
	//GSM Modem On
	S_delay_ms(800000);
	HAL_UART_Transmit(&huart3, comm_1, 27, 9000000);
	S_delay_ms(100000);
	HAL_UART_Transmit(&huart3, comm_2, 24, 9000000);
	S_delay_ms(100000);
	HAL_UART_Transmit(&huart3, comm_3, 14, 9000000);
	S_delay_ms(100000);
	HAL_UART_Transmit(&huart3, comm_4, 13, 9000000);
	S_delay_ms(100000);
	HAL_UART_Transmit(&huart3, comm_5, 19, 9000000);
	S_delay_ms(100000);
	
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
}

uint8_t comm_6[] = "AT+HTTPACTION=0\r\n";

void maincall(){
	if (ch == '$'){
		char_counter = 1;
		x_text[char_counter-1] = '$';
	}
	if (char_counter == 2){
		if (ch == 'G'){
		x_text[char_counter-1] = 'G';
		}
	}
	if (char_counter == 3){
		if (ch == 'P'){
		x_text[char_counter-1] = 'P';
		}
	}
	if (char_counter == 4){
		if (ch == 'R'){
		x_text[char_counter-1] = 'R';
		flag_x++;
		}
	}
	if (char_counter == 5){
		if (ch == 'M'){
		x_text[char_counter-1] = 'M';
		flag_x++;
			
		}
	}
	if (char_counter == 6){
		if (ch == 'C'){
		x_text[char_counter-1] = 'C';
		flag_x++;
		}
	}
	if (ch == '*'){
		if (flag_x == 3){
//			x_text[10] = 'x';
//			x_text[12] = 'x';
//			x_text[24] = 'x';
//			scan_for_dots();
			S_delay_ms(5000);
			printf("%s\n", x_text);
			S_delay_ms(5000);
		}
		flag_x = 0;
		char_counter = 0;
	}
	if (0 < char_counter){
		if (char_counter < 99){
			if (flag_x == 3){
				char_counter_tmp = char_counter-1;
				x_text [char_counter_tmp] = ch;
				char_counter++;
			}
			else {
				char_counter++;
			}
		}
	}
}

//Ham ngat Uart1 de nhan gia tri GPS
void GPS_USART_RX_ISR(){
	if(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE) != RESET){
		ch = (uint8_t)((&huart1)->Instance->DR & (uint8_t)0x00FF);
		maincall();
	}
}


int main(void)
{

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();


  /* Infinite loop */
  while (1)
  {
GPS_USART_RX_ISR();

  }

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USART1 init function */
void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&huart1);

}

/* USART3 init function */
void MX_USART3_UART_Init(void)
{

  huart3.Instance = USART3;
  huart3.Init.BaudRate = 9600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  HAL_UART_Init(&huart3);

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
