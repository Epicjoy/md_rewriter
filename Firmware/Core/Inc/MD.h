#ifndef __MD_H
#define __MD_H

#include "main.h"
#include <stdint.h>

#define GPIO_WriteLow(GPIOx,a)    GPIOx->BSRR=(((uint32_t)(uint8_t)~(a))<<16)|((uint32_t)(uint8_t)(a))
#define GPIO_WriteHigh(GPIOx,a)    GPIOx->BSRR=(((uint8_t)(uint8_t)~(a))<<24)|(((uint32_t)(uint8_t)(a))<<8)
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 

#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C 
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C 
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C 
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C 
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x40011A0C    
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C    

#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08 
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008 
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408 
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40011808 
#define GPIOF_IDR_Addr    (GPIOF_BASE+8) //0x40011A08 
#define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40011E08 
 
 
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)   
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)   

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)   
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)   

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  														
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  	 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)   
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)   

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)   
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)   

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)   
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  

#define MD_WR 			PBout(14)
#define MD_RD  			PBout(4)
#define MD_CS 			PBout(6)
#define MD_RESET 		PBout(15)
#define MD_TIME 		PBout(12)
#define MD_M3 			PBout(13)


void Delay_nop(uint32_t nop);
void read_mode(void);
void write_mode(void);
void enableSram_MD(uint8_t enableSram);
void softsystemreset(void);
void read_mode(void);
void write_mode(void);
uint8_t getByte(uint32_t addr);
void setAddress(uint32_t addr);
void write_data_bus(uint16_t data);
void setByte(uint32_t address, uint16_t dat);
void eraseFLASH(void);
void checkid(void);
void checkheadermenu(void);
uint8_t erase_sector(uint32_t sector_addr);

		
#endif




