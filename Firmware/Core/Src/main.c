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
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "MD.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "usbd_cdc_if.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
	uint8_t receiveBuffer[16][64];
	uint8_t cmdbuff[64];
	uint8_t transmitBuffer[1024];
	uint32_t datacnt;
	uint8_t buffcnt;
	uint32_t addj;
	uint32_t bank = 0;
	uint32_t endadd;
	uint16_t usetime = 0;
	char usetimes[30];
	char displaybuff[50];		
	
	void CDC_Transmit(const char* str)
		{
			uint16_t len = strlen(str);
			while (CDC_Transmit_FS((uint8_t*)str, len) == USBD_BUSY) {
			}
		}	
		
	int fputc(int ch, FILE *f)
		{
			HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xffff);
			return ch;
		}
		
	void memclear(void)
		{
			for(uint8_t i=0;i<15;i++)
				{
					for(uint8_t j = 0;j<64;j++)
					{
						receiveBuffer[i][j]=0x00;
					}
				}
		}
		
	void memclearTX(void)
		{
			for(uint32_t i=0;i<1024;i++)
				{
					transmitBuffer[i]=0x00;
				}
		}
		

		
	void cmdclear(void)
		{
			for(uint8_t j = 0;j<64;j++)
			{
				cmdbuff[j]=0x00;
			}
		}
	
	
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
  MX_USART1_UART_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */
	HAL_Delay(10);
	addj=0;
	endadd=0;
	memclear();
	cmdclear();
	write_mode();
	memclearTX();
	buffcnt = 0;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		if (cmdbuff[0] == 0x0A) {//MD CHOOSE SIZE DUMP
			if((cmdbuff[1] == 0xAA)&&(cmdbuff[2] == 0x55)&&(cmdbuff[3] == 0xAA)&&(cmdbuff[4] == 0xBB)){
				uint32_t wsize = 0;
				if(cmdbuff[5] == 0x04){
				CDC_Transmit("4M ROM DUMP START!!!\r\n");
				wsize = 4096;
				}
				else if(cmdbuff[5] == 0x02){
				CDC_Transmit("1M ROM DUMP START!!!\r\n");
				wsize = 1024;
				}
				else if(cmdbuff[5] == 0x03){
				CDC_Transmit("2M ROM DUMP START!!!\r\n");
				wsize = 2048;
				}
				else{
				CDC_Transmit("512K ROM DUMP START!!!\r\n");
				wsize = 512;	
				}
				read_mode();
				MD_WR = 1;
				MD_RD = 1;
				MD_CS = 1;
				memclearTX();
				HAL_Delay(100);				
				for(uint32_t j=0 ;j < wsize ; j++){						
					for(uint16_t i = 0;i<512;i++){
							setAddress(j*512+i);
							MD_CS = 0;
							MD_RD = 0;
							Delay_nop(30);
							transmitBuffer[i*2] = ((GPIOE->IDR) >>8) & 0xff;
							transmitBuffer[i*2+1] = (GPIOE->IDR) & 0xff;							
							MD_RD = 1;
							MD_CS = 1;
							Delay_nop(50);							
						}
					CDC_Transmit_FS(transmitBuffer, 1024);	
					}
				HAL_Delay(150);
				CDC_Transmit("DUMPER ROM FINISH!!!\r\n");
			}
			CDC_Transmit("PUSH SAVE GAME BUTTON!!!\r\n");
			write_mode();
			cmdclear();
			buffcnt = 0;
			}		
		if (cmdbuff[0] == 0x1A) {//MD SRAM CHOOSE SIZE DUMP
			if((cmdbuff[1] == 0xAA)&&(cmdbuff[2] == 0x55)&&(cmdbuff[3] == 0xAA)&&(cmdbuff[4] == 0xBB)){
				uint32_t wsize = 0;
				if(cmdbuff[5] == 0x01){
				CDC_Transmit("32K RAM DUMP START!!!\r\n");
				wsize = 32;
				}
				else{
				CDC_Transmit("8K ROM DUMP START!!!\r\n");
				wsize = 8;	
				}
				enableSram_MD(1);			
				MD_WR = 1;//WR=1
				MD_RD = 1;//RD=1
				MD_CS = 1;//CS=1
				memclearTX();
				HAL_Delay(100);				
				for(uint32_t j=0 ;j < wsize ; j++){						
					for(uint16_t i = 0;i<1024;i++){
							setAddress(j*1024+i);
							PAout(4) = 1;//A20=1
							MD_CS = 0;//CS=0
							MD_RD = 0;//RD=0
							Delay_nop(30);
							transmitBuffer[i] = (GPIOE->IDR) & 0xff;							
							MD_CS = 1;//RD=1
							MD_CS = 1;//CS=1
							Delay_nop(50);							
						}
					CDC_Transmit_FS(transmitBuffer, 1024);	
					}
				HAL_Delay(150);
				enableSram_MD(0);
				CDC_Transmit("DUMPER RAM FINISH!!!\r\n");				
			}
			//CDC_Transmit("PUSH SAVE BUTTON!!!\r\n");
			write_mode();
			cmdclear();
			buffcnt = 0;
		}					
		if (cmdbuff[0] == 0x0B) {//MD FLASH WRITE
			if((cmdbuff[1] == 0xAA)&&(cmdbuff[2] == 0x55)&&(cmdbuff[3] == 0xAA)&&(cmdbuff[4] == 0xBB)){
				uint32_t addj = cmdbuff[5];
				uint32_t bank = cmdbuff[6];
				uint32_t addw = bank*64*512+addj*512;
				for(uint16_t i = 0;i<1024;i=i+2)
					{
						if((receiveBuffer[i/64][i%64]==0xff)&(receiveBuffer[i/64][i%64+1]==0xff))
							{
							}
						else
							{	
								setByte(0x555,0xaa);
								setByte(0x2aa,0x55);
								setByte(0x555,0xa0);	
								GPIO_WriteHigh(GPIOE, receiveBuffer[i/64][i%64]);								
								GPIO_WriteLow(GPIOE,receiveBuffer[i/64][i%64+1]);
								setAddress(i/2+addw);		
								MD_CS = 0;
								MD_WR = 0;
								Delay_nop(35);
								MD_WR = 1;
								MD_CS = 1;
								Delay_nop(80);
							}
					}	
				buffcnt = 0;
				memclear();
				cmdclear();
				bank=bank+1;
				sprintf(displaybuff,"ADD:0x%X WRITE OK\r\n",addw);				
				CDC_Transmit(displaybuff);
			}
		}
		if (cmdbuff[0] == 0x1B) {//MD FLASH WRITE
			if((cmdbuff[1] == 0xAA)&&(cmdbuff[2] == 0x55)&&(cmdbuff[3] == 0xAA)&&(cmdbuff[4] == 0xBB)){
				uint32_t addj = cmdbuff[5];
				uint32_t bank = cmdbuff[6];
				uint32_t addw = bank*64*1024+addj*1024;
				enableSram_MD(1);
				write_mode();
				for(uint16_t i = 0;i<1024;i++)
					{
						GPIO_WriteLow(GPIOE,receiveBuffer[i/64][i%64]);								
						setAddress(i+addw);	
						PAout(4) = 1;//A20=1	
						Delay_nop(200);			
						MD_CS = 0;//CS=0
						MD_WR = 0;//WR=0						
						Delay_nop(200);
						MD_WR = 1;//CS=0
						MD_CS = 1;//WR=0
						Delay_nop(200);
					}	
				enableSram_MD(0);
				buffcnt = 0;
				memclear();
				cmdclear();
				bank=bank+1;
				sprintf(displaybuff,"ADD:0x%X WRITE GK\r\n",addw);				
				CDC_Transmit(displaybuff);
			}
		}
		if (cmdbuff[0] == 0x0C) {//MD DUMPER CONNECT
			if((cmdbuff[1] == 0xAA)&&(cmdbuff[2] == 0x55)&&(cmdbuff[3] == 0xAA)&&(cmdbuff[4] == 0xBB)){
			HAL_Delay(100);
			CDC_Transmit("FlashMaster MD Dumper is connected\r\n");
			cmdclear();
				
      buffcnt = 0;
			}
    }
		if (cmdbuff[0] == 0x0D) {//MD CHECK HEADER
			if((cmdbuff[1] == 0xAA)&&(cmdbuff[2] == 0x55)&&(cmdbuff[3] == 0xAA)&&(cmdbuff[4] == 0xBB)){
			read_mode();
			checkid();
			HAL_Delay(100);
			//checkheadermenu();
			write_mode();
			cmdclear();
			buffcnt = 0;
			}
		}		
		if (cmdbuff[0] == 0x0E) {//MDFLASHERASE
			if((cmdbuff[1] == 0xAA)&&(cmdbuff[2] == 0x55)&&(cmdbuff[3] == 0xAA)&&(cmdbuff[4] == 0xBB)){
				eraseFLASH();
				HAL_Delay(100);
				CDC_Transmit("SRAM ERASE START\r\n");
				enableSram_MD(1);
				write_mode();
					for(uint32_t j = 0;j<32768;j++)
						{
							GPIO_WriteLow(GPIOE,0x00);								
							setAddress(j);	
							PAout(4) = 1;//A20=1	
							Delay_nop(200);			
							MD_CS = 0;//CS=0
							MD_WR = 0;//WR=0						
							Delay_nop(200);
							MD_WR = 1;//CS=0
							MD_CS = 1;//WR=0
							Delay_nop(200);
						}	
				enableSram_MD(0);
				CDC_Transmit("SRAM ERASE FINISH!!!\r\n");
				cmdclear();
				buffcnt = 0;
			}
		}
		if (cmdbuff[0] == 0x1E) {//MDCHOOSE SIZE SECTORERASE
			if((cmdbuff[1] == 0xAA)&&(cmdbuff[2] == 0x55)&&(cmdbuff[3] == 0xAA)&&(cmdbuff[4] == 0xBB)){
				uint32_t address = 0;
				uint32_t sectoradd = 0;
				if (cmdbuff[5] == 0x1)
					{
						CDC_Transmit("512K ERASEING\r\n");	
						sectoradd = 0x40000;
					}
				else if (cmdbuff[5] == 0x2)
					{
						CDC_Transmit("1M ERASEING\r\n");	
						sectoradd = 0x80000;
					}
				else if (cmdbuff[5] == 0x3)
					{
						CDC_Transmit("2M ERASEING\r\n");	
						sectoradd = 0x100000;
					}
				else if (cmdbuff[5] == 0x4)
					{
						CDC_Transmit("4M ERASEING\r\n");	
						sectoradd = 0x200000;
					}
				else if (cmdbuff[5] == 0x0)
					{	
						sectoradd = 1;
						address = cmdbuff[6];
						address =(address<<8)+cmdbuff[7];
						address =(address<<8)+cmdbuff[8];
						sprintf(displaybuff,"SECTORADD:0x%X ERASEING\r\n",address);
						CDC_Transmit(displaybuff);
					}
				else
					{
						CDC_Transmit("512K ERASEING\r\n");	
						sectoradd = 0x40000;	
					}
				if(cmdbuff[5] < 0x5){
						for(uint32_t i = 0;i < sectoradd ;)
							{
								erase_sector(address);
								Delay_nop(100);
								CDC_Transmit(".");
								if(address<0x8000)
									{
										address = address+0x1000;	
										i = i+0x1000;	
									}
								else
									{
										address = address+0x8000;
										i = i+0x8000;	
									}		
							}
					}
				else{
						eraseFLASH();
					}
				if (cmdbuff[5] == 0x1)
					{
						CDC_Transmit("\r\n512K ERASE OK!\r\n");	
					}
				else if (cmdbuff[5] == 0x2)
					{
						CDC_Transmit("\r\n1M ERASE OK!\r\n");	
					}
				else if (cmdbuff[5] == 0x3)
					{
						CDC_Transmit("\r\n2M ERASE OK!\r\n");	
					}
				else if (cmdbuff[5] == 0x4)
					{
						CDC_Transmit("\r\n4M ERASE OK!\r\n");	
					}
				else if (cmdbuff[5] == 0x5)
					{
						CDC_Transmit("\r\n8M ERASE OK!\r\n");	
					}
				else if (cmdbuff[5] == 0x0)
					{
						sprintf(displaybuff,"SECTORADD:0x%X ERASE OK!\r\n",address);
						CDC_Transmit(displaybuff);
					}
				else 
					{
						CDC_Transmit("\r\n512K ERASE OK!\r\n");	
					}
				write_mode();
				cmdclear();
				buffcnt = 0;
			}
    }
		
		if (cmdbuff[0] == 0x2E) {//MDSECTORERASE
			if((cmdbuff[1] == 0xAA)&&(cmdbuff[2] == 0x55)&&(cmdbuff[3] == 0xAA)&&(cmdbuff[4] == 0xBB)){
				write_mode();
				uint32_t address = cmdbuff[5];
				address = ( address << 8 ) + cmdbuff[6];
				address = ( address << 8 ) + cmdbuff[7];
				erase_sector(address); 			
				sprintf(displaybuff,"\r\nSECTORADD:0x%X ERASE OK!\r\n",address);
				CDC_Transmit(displaybuff);	
				cmdclear();
				buffcnt = 0;
			}
    }
		if (cmdbuff[0] == 0x0F) {//MDBUFFCLEAT
			if((cmdbuff[1] == 0xAA)&&(cmdbuff[2] == 0x55)&&(cmdbuff[3] == 0xAA)&&(cmdbuff[4] == 0xBB)){
			cmdclear();
			buffcnt = 0;
			bank = 0;
			HAL_Delay(100);
			CDC_Transmit("BUFF IS CLEAR\r\n");
			}
    }	
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

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
