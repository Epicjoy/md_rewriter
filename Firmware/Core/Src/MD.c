#include "main.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "usbd_cdc_if.h"
#include "MD.h"


	char disbuff[30];
	
	
	void Delay_nop(uint32_t nop)
		{
			while(nop--)
			{
				__ASM volatile("nop");
			}
		}
				
	void softsystemreset(void)
			{
			__ASM volatile ("cpsid i");
			HAL_NVIC_SystemReset();	
			}


	void read_mode(void)
			{
				GPIO_InitTypeDef GPIO_InitStruct;
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5
														|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9
														|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13
														|GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_0|GPIO_PIN_1, GPIO_PIN_RESET);			
				GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5
														|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9
														|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13
														|GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_0|GPIO_PIN_1;		
				GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
				GPIO_InitStruct.Pull = GPIO_PULLUP;
				HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
			}
			
	void write_mode(void)
			{
				GPIO_InitTypeDef GPIO_InitStruct;
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9
                          |GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13
                          |GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_0|GPIO_PIN_1, GPIO_PIN_RESET);			
				GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9
                          |GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13
                          |GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_0|GPIO_PIN_1;		
				GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
				GPIO_InitStruct.Pull = GPIO_PULLUP;
				GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
				HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
			}
			
	void enableSram_MD(uint8_t enableSram) {
				MD_RESET = 0;//RESET 0	
				Delay_nop(100);
				MD_RESET = 1;	//RESET = 1
				MD_TIME = 1;//TIME = 1	
				Delay_nop(100);
				write_mode();
			  // Set D0 to either 1(enable SRAM) or 0(enable ROM)
				if(enableSram == 1)
					{
						PEout(0) = 1;//D0 = 1	
					}
				else
					{
						PEout(0) = 0; //D0 = 0	
					}
				  // Strobe TIME(PJ0) LOW to latch the data
				MD_TIME = 0;//TIME = 0	
				Delay_nop(100);
				// Set TIME(PJ0) HIGH
				MD_TIME = 1;//TIME = 1	
				Delay_nop(100);
				read_mode();
			}		
	
		void setAddress(uint32_t addr) 
			{
				GPIO_WriteLow(GPIOA,(addr>>16)&0xff);
				GPIO_WriteHigh(GPIOD,(addr>>8)&0xff);
				GPIO_WriteLow(GPIOD,addr&0xff);
			}
						
	void setByte(uint32_t address, uint16_t dat) 
			{
				write_data_bus(dat);
				setAddress(address);		
				MD_CS= 0;
				MD_WR= 0;
				Delay_nop(30);
				MD_WR= 1;
				MD_CS= 1;
				Delay_nop(60);
			}
						
	uint8_t read_data_busNGP(void) 
			{
				return (GPIOE->IDR) & 0xff;
			}
			
	void write_data_bus(uint16_t data)
			{
				GPIO_WriteLow(GPIOE, data & 0xff);
			}
			
	uint8_t getByte(uint32_t addr) 
			{
				setAddress(addr);
				MD_WR= 1;
				MD_RD= 0;
				MD_CS= 0;
				Delay_nop(30);
				uint8_t data = (GPIOE->IDR) & 0xff;
				MD_WR= 1;
				MD_RD= 1;
				MD_CS= 1;
				Delay_nop(50);
				return data;
			}
			
	uint8_t erase_sector(uint32_t sector_addr) 
		{
			write_mode();
			setByte(0x555, 0xaa);
			setByte(0x2aa, 0x55);
			setByte(0x555, 0x80);
			setByte(0x555, 0xaa);
			setByte(0x2aa, 0x55);
			setByte(sector_addr, 0x30);
			while (1) 
				{
					read_mode();
					uint8_t status1 = getByte(sector_addr);
					uint8_t status2 = getByte(sector_addr);
					if ((status1 & 0x40) == (status2 & 0x40)) 
						{
							if (status2 & 0x80) 
								{
									return 1; 
								}
						}
					if (status2 & 0x20) 
						{
							setByte(0x555, 0xaa);
							setByte(0x2aa, 0x55);
							setByte(0x555, 0x80);
							setByte(0x555, 0xaa);
							setByte(0x2aa, 0x55);
							setByte(sector_addr, 0x30);
						}
			}
		}
				
	void eraseFLASH(void)
			{
				CDC_Transmit("-- MD CART ERASE --\r\n");	
				CDC_Transmit("FLASH ERASE START\r\n");		
				write_mode();
				HAL_Delay(10);
				MD_RD= 1;
				MD_CS= 1;
				MD_WR= 1;
				HAL_Delay(1);
				setByte(0x555, 0xaa);
				setByte(0x2aa, 0x55);
				setByte(0x555, 0x80);
				setByte(0x555, 0xaa);
				setByte(0x2aa, 0x55);
				setByte(0x555, 0x10);
				MD_RD= 1;
				MD_CS= 1;
				MD_WR= 1;
				HAL_Delay(10);
				read_mode();
				uint8_t eraseflag = 0;
				uint8_t data[8];
				uint32_t time = 0;
				
				//char chartime[15];
				while(eraseflag == 0)
				{
					data[0] = getByte(0x0);
					data[1] = getByte(0x1);
					data[2] = getByte(0x2);
					data[3] = getByte(0x3);
					data[4] = getByte(0x4);
					data[5] = getByte(0x5);
					data[6] = getByte(0x6);
					data[7] = getByte(0x7);
					HAL_Delay(1000);
					HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_13);
					if(data[0] == 0xff) eraseflag = 1;
					else eraseflag = 0;
					if((data[1] == 0xff) & (eraseflag == 1)) eraseflag = 1;
					else eraseflag = 0;
					if((data[2] == 0xff) & (eraseflag == 1)) eraseflag = 1;
					else eraseflag = 0;
					if((data[3] == 0xff) & (eraseflag == 1)) eraseflag = 1;
					else eraseflag = 0;
					if((data[4] == 0xff) & (eraseflag == 1)) eraseflag = 1;
					else eraseflag = 0;
					if((data[5] == 0xff) & (eraseflag == 1)) eraseflag = 1;
					else eraseflag = 0;
					if((data[6] == 0xff) & (eraseflag == 1)) eraseflag = 1;
					else eraseflag = 0;
					if((data[7] == 0xff) & (eraseflag == 1)) 
						{
							eraseflag = 1;
						}
					sprintf(disbuff,"USE TIME %d s\r\n",time);
					CDC_Transmit(disbuff);
					time = time + 1;	
				}
				write_mode();
				CDC_Transmit("FLASH ERASE FINISH!!!\r\n");
				HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_RESET);
			}
			
			
	void checkid(void)
			{
				CDC_Transmit("-- MD CART ID --\r\n");		
				char chipid[2];
				char disbuff[50];
				uint8_t s1,s2;
				write_mode();
				HAL_Delay(10);
				MD_RD= 1;
				MD_CS= 1;
				MD_WR= 1;
				HAL_Delay(1);
				setByte(0x555, 0xaa);
				setByte(0x2aa, 0x55);
				setByte(0x555, 0x90);			
				MD_RD= 1;
				MD_CS= 1;
				MD_WR= 1;
				read_mode();		
				s1 = getByte(0x0);
				s2 = getByte(0x1);
				write_mode();
				HAL_Delay(10);
				MD_RD= 1;
				MD_CS= 1;
				MD_WR= 1;
				HAL_Delay(1);
				setByte(0x555, 0xaa);
				setByte(0x2aa, 0x55);
				setByte(0x555, 0xf0);	
				MD_RD= 1;
				MD_CS= 1;
				MD_WR= 1;
				read_mode();
				HAL_Delay(100);
				write_mode();
				HAL_Delay(10);
				MD_RD= 1;
				MD_CS= 1;
				MD_WR= 1;
				HAL_Delay(1);
				setByte(0x555, 0xaa);
				setByte(0x2aa, 0x55);
				setByte(0x555, 0x90);			
				MD_RD= 1;
				MD_CS= 1;
				MD_WR= 1;
				read_mode();		
				s1 = getByte(0x0);
				s2 = getByte(0x1);
				sprintf(chipid,"%X%X",s1,s2);
				if((s1==0xC2)&(s2==0XCB))
					{
						sprintf(disbuff,"FLASHID:%X%X\r\n",s1,s2);
						CDC_Transmit(disbuff);
						CDC_Transmit("MX29LV640EB MD FLASH CART\r\n");				
					}
				else 
					{
						sprintf(disbuff,"FLASHID:%X%X\r\n",s1,s2);
						CDC_Transmit(disbuff);
						CDC_Transmit("NO FIND NGP CARD\r\n");
					}
				write_mode();
				HAL_Delay(10);
				MD_RD= 1;
				MD_CS= 1;
				MD_WR= 1;
				HAL_Delay(1);
				setByte(0x555, 0xaa);
				setByte(0x2aa, 0x55);
				setByte(0x555, 0xf0);				
				MD_RD= 1;
				MD_CS= 1;
				MD_WR= 1;
			}
			