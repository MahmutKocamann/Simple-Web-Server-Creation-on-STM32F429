#include "stm32f4xx.h"                  
#include "stdio.h"
#include "string.h"
#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_lcd.h"
#include <stdio.h>
#include "a111.h"
#include "stm32f429i_discovery_sdram.h"
//#include "a111.h"
//#include "OYUN.h"

#define IS42S16400J_SIZE             0x150000

#define BOYUT 180000
//#define BOYUT2 80000

#define FLASH_USER_START_ADDR   ADDR_FLASH_SECTOR_4   /* Start @ of user Flash area */
#define FLASH_USER_END_ADDR     ADDR_FLASH_SECTOR_23   /* End @ of user Flash area */

/* Base address of the Flash sectors */ 
#define ADDR_FLASH_SECTOR_0      ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbytes   */
#define ADDR_FLASH_SECTOR_1      ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbytes   */
#define ADDR_FLASH_SECTOR_2      ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbytes   */
#define ADDR_FLASH_SECTOR_3      ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbytes   */
#define ADDR_FLASH_SECTOR_4      ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbytes   */
#define ADDR_FLASH_SECTOR_5      ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbytes  */
#define ADDR_FLASH_SECTOR_6      ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbytes  */
#define ADDR_FLASH_SECTOR_7      ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbytes  */
#define ADDR_FLASH_SECTOR_8      ((uint32_t)0x08080000) /* Base @ of Sector 8, 128 Kbytes  */
#define ADDR_FLASH_SECTOR_9      ((uint32_t)0x080A0000) /* Base @ of Sector 9, 128 Kbytes  */
#define ADDR_FLASH_SECTOR_10     ((uint32_t)0x080C0000) /* Base @ of Sector 10, 128 Kbytes */
#define ADDR_FLASH_SECTOR_11     ((uint32_t)0x080E0000) /* Base @ of Sector 11, 128 Kbytes */
#define ADDR_FLASH_SECTOR_12     ((uint32_t)0x08100000) /* Base @ of Sector 12, 16 Kbytes  */
#define ADDR_FLASH_SECTOR_13     ((uint32_t)0x08104000) /* Base @ of Sector 13, 16 Kbytes  */
#define ADDR_FLASH_SECTOR_14     ((uint32_t)0x08108000) /* Base @ of Sector 14, 16 Kbytes  */
#define ADDR_FLASH_SECTOR_15     ((uint32_t)0x0810C000) /* Base @ of Sector 15, 16 Kbytes  */
#define ADDR_FLASH_SECTOR_16     ((uint32_t)0x08110000) /* Base @ of Sector 16, 64 Kbytes  */
#define ADDR_FLASH_SECTOR_17     ((uint32_t)0x08120000) /* Base @ of Sector 17, 128 Kbytes */
#define ADDR_FLASH_SECTOR_18     ((uint32_t)0x08140000) /* Base @ of Sector 18, 128 Kbytes */
#define ADDR_FLASH_SECTOR_19     ((uint32_t)0x08160000) /* Base @ of Sector 19, 128 Kbytes */
#define ADDR_FLASH_SECTOR_20     ((uint32_t)0x08180000) /* Base @ of Sector 20, 128 Kbytes */
#define ADDR_FLASH_SECTOR_21     ((uint32_t)0x081A0000) /* Base @ of Sector 21, 128 Kbytes */
#define ADDR_FLASH_SECTOR_22     ((uint32_t)0x081C0000) /* Base @ of Sector 22, 128 Kbytes */
#define ADDR_FLASH_SECTOR_23     ((uint32_t)0x081E0000) /* Base @ of Sector 23, 128 Kbytes */


void USART_Initialize(void);
void USART1_IRQHandler(void);
void USART_Puts(USART_TypeDef* USARTx, volatile char *s);
void LED_Initialize(void);
static void ESP8266_Init(void);
static void Clear_ESPBuffer(void);
static void RenkPalet (void);
void delay(uint32_t x);
void ResetPin_ESP8266(void);


char ESP8266Buf[BOYUT];
//unsigned char ESP8266Buf2[BOYUT2];
static uint16_t ESPWriteIndex = 0;
static uint16_t receive_data;
char htmlcode[5000];
char str[5000];
int uzunluk;
uint32_t m = 0;
uint32_t n = 0;
//static uint32_t sayac =0;


uint32_t uwStartSector = 0;
uint32_t uwEndSector = 0;
uint32_t uwAddress = 0;
uint32_t uwSectorCounter = 0;

__IO uint32_t uwData32 = 0;
__IO uint32_t uwMemoryProgramStatus = 0;
  

static uint32_t GetSector(uint32_t Address);


int main()
{
         
  LCD_Init(); 
  
  LCD_LayerInit();
 
  /* Reload configuration of Layer1 */
  //LTDC_ReloadConfig(LTDC_IMReload);
  
  LTDC_Cmd(ENABLE);  
  
  LCD_SetLayer(LCD_FOREGROUND_LAYER); 
  
  LCD_DisplayOn();
  
  LCD_Clear(0xFFFF);
  
  LTDC_ReloadConfig(LTDC_IMReload);
  
  LCD_SetFont(&Font8x8);
  
    /* SDRAM Initialization */  
  SDRAM_Init();
  
  /* FMC SDRAM GPIOs Configuration */
  SDRAM_GPIOConfig();
  
  /* Disable write protection */
 
  
	USART_Initialize();
	LED_Initialize();
	ResetPin_ESP8266();
	Clear_ESPBuffer();
        
        
                
        FLASH_Unlock();
    
  /* Erase the user Flash area ************************************************/
  /* area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR */

  /* Clear pending flags (if any) */  
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR); 

  /* Get the number of the start and end sectors */
  uwStartSector = GetSector(FLASH_USER_START_ADDR);
  uwEndSector = GetSector(FLASH_USER_END_ADDR);

  /* Strat the erase operation */
  uwSectorCounter = uwStartSector;
        
	
	while(1)
	{
        
		ESP8266_Init();
	}
  
  
}


static void ESP8266_Init(void)
{
	
	static uint8_t ESPInitCase = 0;
	
	switch(ESPInitCase)
	{
		case 0 :
			
		   USART_Puts(USART1, "AT\r\n");
		   delay(1500000);
		   ESPInitCase = 1;
		   break;
		
		case 1 :
			
		   if(strstr(ESP8266Buf,"OK") != NULL)
			 {
				 Clear_ESPBuffer();
				 USART_Puts(USART3, "Module Erisildi\n");
                                 LCD_SetTextColor(LCD_COLOR_BLACK); 
                                 LCD_DisplayStringLine(LCD_LINE_2, (uint8_t*)" Module Erisildi ");
                               //  TM_ILI9341_Puts(5, 20, "Module Erisildi", &TM_Font_7x10, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
				 ESPInitCase = 2;
			 }
			 else
			 {
				 Clear_ESPBuffer();
				// USART_Puts(USART3, "Module Erisilemedi, Tekrar Deneniyor\n");
                                 LCD_SetTextColor(LCD_COLOR_BLACK); 
                                 LCD_DisplayStringLine(LCD_LINE_2, (uint8_t*)" Module Erisilemedi, Tekrar Deneniyor ");
				 ESPInitCase = 0;
			 }
			 break;
			 
		case 2 :
			
		   USART_Puts(USART1, "AT+CWMODE?\r\n");
		   delay(1500000);
	     ESPInitCase = 3;
		   break;
		
		case 3 :
			
		   if(strstr(ESP8266Buf, "+CWMODE:1") != NULL)
			 {
				 Clear_ESPBuffer();
				 USART_Puts(USART3, "STA Modunda, Ayar Dogru\n");
                                 LCD_SetTextColor(LCD_COLOR_BLACK); 
                                 LCD_DisplayStringLine(LCD_LINE_8, (uint8_t*)" STA Modunda, Ayar Dogru ");
                              //   TM_ILI9341_Puts(5, 80, "STA Modunda, Ayar Dogru", &TM_Font_7x10, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
				 ESPInitCase = 4;
			 }
			 else
			 {
				 USART_Puts(USART1, "AT+CWMODE=1\r\n");
				 delay(1500000);
				 Clear_ESPBuffer();
				 USART_Puts(USART3, "Mode Degistirildi\n");
                                 LCD_SetTextColor(LCD_COLOR_BLACK); 
                                 LCD_DisplayStringLine(LCD_LINE_8, (uint8_t*)" Mode Degistirildi ");
				 ESPInitCase = 2;
			 }
			 break;
			 
		case 4 :
		   
	           USART_Puts(USART1, "AT+CWJAP=\"OFLU\",\"termo2015\"\r\n");
		   delay(6000000);
		   ESPInitCase = 5;
		   break;
		
		case 5 :
			
		   if(strstr(ESP8266Buf, "OK") != NULL)
			 {
				 Clear_ESPBuffer();
				 USART_Puts(USART3, "Modeme Baglanildi\n");
                                 LCD_SetTextColor(LCD_COLOR_BLACK); 
                                 LCD_DisplayStringLine(LCD_LINE_14, (uint8_t*)" Modeme Baglanildi ");
                             //    TM_ILI9341_Puts(5, 140, "Modeme Baglanildi", &TM_Font_7x10, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
				 ESPInitCase = 6;
			 }
			 else
			 {
				 USART_Puts(USART3, "Modeme Baglanti Bekleniyor...\n");
                                 LCD_SetTextColor(LCD_COLOR_BLACK); 
                                 LCD_DisplayStringLine(LCD_LINE_14, (uint8_t*)" Modeme Baglanti Bekleniyor ");
				 delay(15000000); 
			 } 
			 break;
			 
		case 6 :
			  
		   USART_Puts(USART1, "AT+CIPMUX=1\r\n");
		   delay(1500000);
                   USART_Puts(USART1,"AT+CIFSR\r\n");
            // 1 saniye gecikme koyuyoruz.
            delay(1500000);
            
		   ESPInitCase = 7;
		   break;
		
		case 7 :
			
		   if(strstr(ESP8266Buf, "OK") != NULL)
			 {
                                 
				 Clear_ESPBuffer();
				 USART_Puts(USART3, "CIPMUX=1 Yapildi\n");
                                 LCD_SetTextColor(LCD_COLOR_BLACK); 
                                 LCD_DisplayStringLine(LCD_LINE_20, (uint8_t*)" CIPMUX=1 Yapildi ");
                                 LCD_DisplayStringLine(LCD_LINE_26, (uint8_t*)"Sunucu 80 Portunda Acildi");
                             //    TM_ILI9341_Puts(5, 200, "CIPMUX=1 Yapildi", &TM_Font_7x10, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
				 ESPInitCase = 8;
			 }
			 else
			 {
				 USART_Puts(USART3, "CIPMUX Ayari Yapilamadi, Tekrar Deneniyor...\n");
                                 LCD_SetTextColor(LCD_COLOR_BLACK); 
                                 LCD_DisplayStringLine(LCD_LINE_20, (uint8_t*)"CIPMUX Ayari Yapilamadi, Tekrar Deneniyor...");
				 delay(6000000);
			 }
			 break;
			 
		case 8 :
			
		   USART_Puts(USART1, "AT+CIPSERVER=1,80\r\n");
		   delay(1500000);
		   ESPInitCase = 9;
		   break;
		
		case 9 :
			
		   if(strstr(ESP8266Buf, "OK") != NULL)
			 {
				 Clear_ESPBuffer();
				 USART_Puts(USART3, "Sunucu 80 Portunda Acildi\n");
                                 LCD_SetTextColor(LCD_COLOR_BLACK); 
                              //   LCD_DisplayStringLine(LCD_LINE_26, (uint8_t*)"Sunucu 80 Portunda Acildi");
                              //   TM_ILI9341_Puts(5, 260, "Sunucu 80 Portunda Acildi", &TM_Font_7x10, ILI9341_COLOR_BLACK, ILI9341_COLOR_BLUE2);
				 ESPInitCase = 10;
			 }
			 else
			 {
				 USART_Puts(USART3, "Sunucu Acilamadi, Tekrar Deneniyor...\n");
                                 LCD_SetTextColor(LCD_COLOR_BLACK); 
                                 //LCD_DisplayStringLine(LCD_LINE_28, (uint8_t*)"Sunucu Acilamadi, Tekrar Deneniyor...");
				 delay(3000000);
			 }
			 break;
			 
		case 10 :
			
		  if(*ESP8266Buf != 0)
			{
                          if(strstr(ESP8266Buf, "+IPD,") != NULL)
			 {
				 strcpy(htmlcode, "<head><h2> MAHMUT KOCAMAN 140208033</h2> </head><br><head> LED1 </head><a href=\" ?pin=on\"><button type='button' >ON</button></a> <a href=\" ?pin=off\"><button type='button'>OFF</button></a><br><br><head> LED2 </head><a href=\" ?pin2=on\"><button type='button' >ON</button></a> <a href=\" ?pin2=off\"><button type='button'>OFF</button></a><br><br><head> FLASH MEMORY TEMIZLE </head><a href=\" ?pin=reset\"><button type='button' >TEMIZLE</button></a><br><br> <!DOCTYPE html><html><body><form action='/action_page.php'>Renk Giriniz:<br><input type='text' name='Color'><br><input type='submit'></form></body></html><!DOCTYPE html><html> <body><form action='upload.php' method='post' enctype='multipart/form-data'>Yüklenecek Resmi Seçiniz:<input type='file' name='fileToUpload' id='fileToUpload'><input type='submit' value='Upload Image' name='submit'></form></body></html><br><head> RESMI GÖRÜNTÜLE </head><a href=\" ?pin=goster\"><button type='button' >GÖRÜNTÜLE</button></a><br><br>");
				 uzunluk = strlen(htmlcode);
				 uzunluk += 2;
				 sprintf(str, "AT+CIPSEND=0,%d\r\n", uzunluk);
				 USART_Puts(USART1, str);
				 delay(6000000);
				 USART_Puts(USART1, htmlcode);
				 delay(6000000);

				 if(strstr(ESP8266Buf, "?pin=on") != NULL)
				 {
					 GPIO_SetBits(GPIOG, GPIO_Pin_13);                                          
				 }
				 if(strstr(ESP8266Buf, "?pin=off") != NULL)
				 {
					 GPIO_ResetBits(GPIOG, GPIO_Pin_13);
				 }
                                 if(strstr(ESP8266Buf, "?pin2=on") != NULL)
				 {
					 GPIO_SetBits(GPIOG, GPIO_Pin_14);
				 }
				 if(strstr(ESP8266Buf, "?pin2=off") != NULL)
				 {
					 GPIO_ResetBits(GPIOG, GPIO_Pin_14);
				 }
                                 
                                 if(strstr(ESP8266Buf, "?pin=reset") != NULL)
                                  {
                                    FMC_SDRAMWriteProtectionConfig(FMC_Bank2_SDRAM,DISABLE); 
                                    FMC_SDRAMWriteProtectionConfig(FMC_Bank1_SDRAM,DISABLE);
                                    
                                    for (int counter = 0; counter < IS42S16400J_SIZE; counter++)
                                         {
                                                 *(__IO uint16_t*) (SDRAM_BANK_ADDR + 2*counter) = (uint16_t)0x0000;
                                         }
                                    
                                         uwSectorCounter = uwStartSector;
                                    while (uwSectorCounter <= uwEndSector) 
                                        {
    /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
       be done by word */ 
                                               if (FLASH_EraseSector(uwSectorCounter, VoltageRange_3) != FLASH_COMPLETE)
                                                    { 
      /* Error occurred while sector erase. 
         User can add here some code to deal with this error  */
                                                        while (1)
                                                          {
                                                          }
                                                     }
    /* jump to the next sector */
                                                if (uwSectorCounter == FLASH_Sector_11)
                                                    {
                                                         uwSectorCounter += 40;
                                                    } 
                                                else 
                                                    {
                                                         uwSectorCounter += 8;
                                                    }
                                       }     
                      
                                        Clear_ESPBuffer();
                                  }
                                 
				 if(strstr(ESP8266Buf, "?Color=Daire") != NULL)
				 {
                                   
                                 //  TM_ILI9341_Fill(ILI9341_COLOR_MAGENTA);
					  
                                   //TM_ILI9341_DrawFilledCircle(60, 60, 35, ILI9341_COLOR_RED);
                                   
                                   LCD_Clear(0x07E0);
                                          
				 }
                                 if(strstr(ESP8266Buf, "Color=Dikdortgen") != NULL)
				 {
                                  // TM_ILI9341_Fill(ILI9341_COLOR_MAGENTA);
					//  TM_ILI9341_DrawFilledRectangle(130, 30, 210, 90, ILI9341_COLOR_BLACK);
                                   
                                   LCD_Clear(0x001F);
				 }
                                 if(strstr(ESP8266Buf, "?Color=Bayrak") != NULL)
				 {

                                  
                                          
				 }
                                 
                                 
                                 if(strstr(ESP8266Buf, "?Color=Oyun") != NULL)
				 {  
                                   
                                          
     
				 }
                                 
                                 if(strstr(ESP8266Buf, "?pin=goster") != NULL)
                                 {
                                   FMC_SDRAMWriteProtectionConfig(FMC_Bank2_SDRAM,DISABLE); 
                                   
                                 
                                    for (int counter = 0; counter < IS42S16400J_SIZE; counter++)
                                         {
                                                  *(__IO uint16_t*) (SDRAM_BANK_ADDR +BUFFER_OFFSET+ counter) = (uint16_t)0x00;
                                         }
  
                                    //delay(1000);
                                       
                                    for (int counter = 0; counter < 170000; counter++)
                                         {
                                       *(__IO uint16_t*) (0xD0050000 + counter) = *(__IO uint16_t*) (0x0801002B + counter);
                                         }
                                   
                                   
                                        Clear_ESPBuffer();
                                 }
                                 
                                 
                                 
                                 if(strstr(ESP8266Buf, "WebKitFormBoundary") != NULL)
				 {
                                   
                                  //delay(100000);
                                  RenkPalet();
                                  
                                  Clear_ESPBuffer();
                                  
				 }
                                 
				 USART_Puts(USART1, "AT+CIPCLOSE=0\r\n");
				 Clear_ESPBuffer();
				 ESPInitCase = 8;
			 }
			 
			 break;
		 } 
	}
}

void USART_Initialize(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP; // AF --> UP Olmali
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP; // AF --> UP Olmali
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9,  GPIO_AF_USART1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10,  GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
	
	USART_InitStructure.USART_BaudRate            = 115200;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode                = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStructure.USART_Parity              = USART_Parity_No;
	USART_InitStructure.USART_StopBits            = USART_StopBits_1;
	USART_InitStructure.USART_WordLength          = USART_WordLength_8b; //Gönderilen Veri 8-Bitlik
	USART_Init(USART1, &USART_InitStructure);
	
	USART_Cmd(USART1, ENABLE);
	
	USART_InitStructure.USART_BaudRate            = 115200;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode                = USART_Mode_Tx;
	USART_InitStructure.USART_Parity              = USART_Parity_No;
	USART_InitStructure.USART_StopBits            = USART_StopBits_1;
	USART_InitStructure.USART_WordLength          = USART_WordLength_8b; //Gönderilen Veri 8-Bitlik
	USART_Init(USART3, &USART_InitStructure);
	
	USART_Cmd(USART3, ENABLE);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

  NVIC_InitStructure.NVIC_IRQChannel                   = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void USART_Puts(USART_TypeDef* USARTx, volatile char *s)
{
	while(*s)
	{
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
		USART_SendData(USARTx, *s);
		*s++;
	}
}

void USART1_IRQHandler(void)
{
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE))
	{
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		
		//uint8_t receive_data;
		
		receive_data = USART1 ->DR;
		USART3 ->DR  = receive_data;
                
		
		if(receive_data != 0)
		{
			ESP8266Buf[ESPWriteIndex] = receive_data;
			ESPWriteIndex++;	
		}			 
	}
}

void LED_Initialize(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13 | GPIO_Pin_14;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOG, &GPIO_InitStructure);	
}

void delay(uint32_t x)
{
	while(x--);
}

static void Clear_ESPBuffer(void)
{
  uint32_t i;
	
   for(i = 0; i < BOYUT; i++)
	 {
     ESP8266Buf[i] = 0;
	 }
		
   ESPWriteIndex = 0;
}

void ResetPin_ESP8266(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13; 
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
  GPIO_ResetBits(GPIOB, GPIO_Pin_13);
}

void RenkPalet (void)
{
  
  uwAddress = FLASH_USER_START_ADDR;
  
                                    
for (uint32_t j=0; j <= 180000 ; j++)
   {
     
      if(ESP8266Buf[j] == 0x30) // ilk 8 bit elemani 0 ise;
   {
      if(ESP8266Buf[j+1] == 0x30) // 2. eleman 0 ise
        {        
          FLASH_ProgramByte(uwAddress, 0x00);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
        }
      else if(ESP8266Buf[j+1] == 0x31) // 2. eleman 1 ise
            {
              FLASH_ProgramByte(uwAddress, 0x01);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x32) // 2. eleman 2 ise
            {
              FLASH_ProgramByte(uwAddress, 0x02);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x33) // 2. eleman 3 ise
            {
              FLASH_ProgramByte(uwAddress, 0x03);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x34) // 2. eleman 4 ise
            {
              FLASH_ProgramByte(uwAddress, 0x04);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x35) // 2. eleman 5 ise
            {
              FLASH_ProgramByte(uwAddress, 0x05);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x36) // 2. eleman 6 ise
            {
              FLASH_ProgramByte(uwAddress, 0x06);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x37) // 2. eleman 7 ise
            {
              FLASH_ProgramByte(uwAddress, 0x07);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x38) // 2. eleman 8 ise
            {
              FLASH_ProgramByte(uwAddress, 0x08);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x39) // 2. eleman 9 ise
            {
              FLASH_ProgramByte(uwAddress, 0x09);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x41) // 2. eleman A ise
            {
              FLASH_ProgramByte(uwAddress, 0x0A);
             uwAddress = uwAddress + 1;    
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x42) // 2. eleman B ise
            {
              FLASH_ProgramByte(uwAddress, 0x0B);
             uwAddress = uwAddress + 1;    
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x43) // 2. eleman C ise
            {
              FLASH_ProgramByte(uwAddress, 0x0C);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x44) // 2. eleman D ise
            {
              FLASH_ProgramByte(uwAddress, 0x0D);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x45) // 2. eleman E ise
            {
              FLASH_ProgramByte(uwAddress, 0x0E);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x46) // 2. eleman F ise
            {
              FLASH_ProgramByte(uwAddress, 0x0F);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
 
   }
   
      ////////////////////////////////////////////////////////////////////////////////////////
      //////////////////////////ILK ELEMAN 0 IÇIN TAMAMLANDI/////////////////////////////////
   
   
   else if(ESP8266Buf[j] == 0x31) // ilk 8 bit elemani 1 ise;
   {
      if(ESP8266Buf[j+1] == 0x30) // 2. eleman 0 ise
        {        
          FLASH_ProgramByte(uwAddress, 0x10);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
        }
      else if(ESP8266Buf[j+1] == 0x31) // 2. eleman 1 ise
            {
              FLASH_ProgramByte(uwAddress, 0x11);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x32) // 2. eleman 2 ise
            {
              FLASH_ProgramByte(uwAddress, 0x12);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x33) // 2. eleman 3 ise
            {
              FLASH_ProgramByte(uwAddress, 0x13);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x34) // 2. eleman 4 ise
            {
              FLASH_ProgramByte(uwAddress, 0x14);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x35) // 2. eleman 5 ise
            {
              FLASH_ProgramByte(uwAddress, 0x15);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x36) // 2. eleman 6 ise
            {
              FLASH_ProgramByte(uwAddress, 0x16);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x37) // 2. eleman 7 ise
            {
              FLASH_ProgramByte(uwAddress, 0x17);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x38) // 2. eleman 8 ise
            {
              FLASH_ProgramByte(uwAddress, 0x18);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x39) // 2. eleman 9 ise
            {
              FLASH_ProgramByte(uwAddress, 0x19);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }    
      else if(ESP8266Buf[j+1] == 0x41) // 2. eleman A ise
            {
              FLASH_ProgramByte(uwAddress, 0x1A);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x42) // 2. eleman B ise
            {
              FLASH_ProgramByte(uwAddress, 0x1B);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x43) // 2. eleman C ise
            {
              FLASH_ProgramByte(uwAddress, 0x1C);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x44) // 2. eleman D ise
            {
              FLASH_ProgramByte(uwAddress, 0x1D);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x45) // 2. eleman E ise
            {
              FLASH_ProgramByte(uwAddress, 0x1E);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x46) // 2. eleman F ise
            {
              FLASH_ProgramByte(uwAddress, 0x1F);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
 
   }
   
      ////////////////////////////////////////////////////////////////////////////////////////
      //////////////////////////ILK ELEMAN 1 IÇIN TAMAMLANDI/////////////////////////////////
   
   
   else if(ESP8266Buf[j] == 0x32) // ilk 8 bit elemani 2 ise;
   {
      if(ESP8266Buf[j+1] == 0x30) // 2. eleman 0 ise
        {        
          FLASH_ProgramByte(uwAddress, 0x20);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
        }
      else if(ESP8266Buf[j+1] == 0x31) // 2. eleman 1 ise
            {
              FLASH_ProgramByte(uwAddress, 0x21);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x32) // 2. eleman 2 ise
            {
              FLASH_ProgramByte(uwAddress, 0x22);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x33) // 2. eleman 3 ise
            {
              FLASH_ProgramByte(uwAddress, 0x23);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x34) // 2. eleman 4 ise
            {
              FLASH_ProgramByte(uwAddress, 0x24);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x35) // 2. eleman 5 ise
            {
              FLASH_ProgramByte(uwAddress, 0x25);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x36) // 2. eleman 6 ise
            {
              FLASH_ProgramByte(uwAddress, 0x26);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x37) // 2. eleman 7 ise
            {
              FLASH_ProgramByte(uwAddress, 0x27);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x38) // 2. eleman 8 ise
            {
              FLASH_ProgramByte(uwAddress, 0x28);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x39) // 2. eleman 9 ise
            {
              FLASH_ProgramByte(uwAddress, 0x29);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }    
      else if(ESP8266Buf[j+1] == 0x41) // 2. eleman A ise
            {
              FLASH_ProgramByte(uwAddress, 0x2A);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x42) // 2. eleman B ise
            {
              FLASH_ProgramByte(uwAddress, 0x2B);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x43) // 2. eleman C ise
            {
              FLASH_ProgramByte(uwAddress, 0x2C);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x44) // 2. eleman D ise
            {
              FLASH_ProgramByte(uwAddress, 0x2D);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x45) // 2. eleman E ise
            {
              FLASH_ProgramByte(uwAddress, 0x2E);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x46) // 2. eleman F ise
            {
              FLASH_ProgramByte(uwAddress, 0x2F);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
   }
   
      ////////////////////////////////////////////////////////////////////////////////////////
      //////////////////////////ILK ELEMAN 2 IÇIN TAMAMLANDI/////////////////////////////////
   
   else if(ESP8266Buf[j] == 0x33) // ilk 8 bit elemani 3 ise;
   {
      if(ESP8266Buf[j+1] == 0x30) // 2. eleman 0 ise
        {        
          FLASH_ProgramByte(uwAddress, 0x30);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
        }
      else if(ESP8266Buf[j+1] == 0x31) // 2. eleman 1 ise
            {
              FLASH_ProgramByte(uwAddress, 0x31);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x32) // 2. eleman 2 ise
            {
              FLASH_ProgramByte(uwAddress, 0x32);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x33) // 2. eleman 3 ise
            {
              FLASH_ProgramByte(uwAddress, 0x33);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x34) // 2. eleman 4 ise
            {
              FLASH_ProgramByte(uwAddress, 0x34);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x35) // 2. eleman 5 ise
            {
              FLASH_ProgramByte(uwAddress, 0x35);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x36) // 2. eleman 6 ise
            {
              FLASH_ProgramByte(uwAddress, 0x36);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x37) // 2. eleman 7 ise
            {
              FLASH_ProgramByte(uwAddress, 0x37);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x38) // 2. eleman 8 ise
            {
              FLASH_ProgramByte(uwAddress, 0x38);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x39) // 2. eleman 9 ise
            {
              FLASH_ProgramByte(uwAddress, 0x39);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }    
      else if(ESP8266Buf[j+1] == 0x41) // 2. eleman A ise
            {
              FLASH_ProgramByte(uwAddress, 0x3A);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x42) // 2. eleman B ise
            {
              FLASH_ProgramByte(uwAddress, 0x3B);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x43) // 2. eleman C ise
            {
              FLASH_ProgramByte(uwAddress, 0x3C);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x44) // 2. eleman D ise
            {
              FLASH_ProgramByte(uwAddress, 0x3D);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x45) // 2. eleman E ise
            {
              FLASH_ProgramByte(uwAddress, 0x3E);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x46) // 2. eleman F ise
            {
              FLASH_ProgramByte(uwAddress, 0x3F);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
 
   }
   
      ////////////////////////////////////////////////////////////////////////////////////////
      //////////////////////////ILK ELEMAN 3 IÇIN TAMAMLANDI/////////////////////////////////
   
   else if(ESP8266Buf[j] == 0x34) // ilk 8 bit elemani 4 ise;
   {
      if(ESP8266Buf[j+1] == 0x30) // 2. eleman 0 ise
        {        
          FLASH_ProgramByte(uwAddress, 0x40);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
        }
      else if(ESP8266Buf[j+1] == 0x31) // 2. eleman 1 ise
            {
              FLASH_ProgramByte(uwAddress, 0x41);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x32) // 2. eleman 2 ise
            {
              FLASH_ProgramByte(uwAddress, 0x42);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x33) // 2. eleman 3 ise
            {
              FLASH_ProgramByte(uwAddress, 0x43);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x34) // 2. eleman 4 ise
            {
              FLASH_ProgramByte(uwAddress, 0x44);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x35) // 2. eleman 5 ise
            {
              FLASH_ProgramByte(uwAddress, 0x45);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x36) // 2. eleman 6 ise
            {
              FLASH_ProgramByte(uwAddress, 0x46);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x37) // 2. eleman 7 ise
            {
              FLASH_ProgramByte(uwAddress, 0x47);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x38) // 2. eleman 8 ise
            {
              FLASH_ProgramByte(uwAddress, 0x48);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x39) // 2. eleman 9 ise
            {
              FLASH_ProgramByte(uwAddress, 0x49);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }    
      else if(ESP8266Buf[j+1] == 0x41) // 2. eleman A ise
            {
              FLASH_ProgramByte(uwAddress, 0x4A);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x42) // 2. eleman B ise
            {
              FLASH_ProgramByte(uwAddress, 0x4B);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x43) // 2. eleman C ise
            {
              FLASH_ProgramByte(uwAddress, 0x4C);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x44) // 2. eleman D ise
            {
              FLASH_ProgramByte(uwAddress, 0x4D);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x45) // 2. eleman E ise
            {
              FLASH_ProgramByte(uwAddress, 0x4E);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x46) // 2. eleman F ise
            {
              FLASH_ProgramByte(uwAddress, 0x4F);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
 
   }
   
      ////////////////////////////////////////////////////////////////////////////////////////
      //////////////////////////ILK ELEMAN 4 IÇIN TAMAMLANDI/////////////////////////////////
      
   else if(ESP8266Buf[j] == 0x35) // ilk 8 bit elemani 5 ise;
   {
      if(ESP8266Buf[j+1] == 0x30) // 2. eleman 0 ise
        {        
          FLASH_ProgramByte(uwAddress, 0x50);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
        }
      else if(ESP8266Buf[j+1] == 0x31) // 2. eleman 1 ise
            {
              FLASH_ProgramByte(uwAddress, 0x51);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x32) // 2. eleman 2 ise
            {
              FLASH_ProgramByte(uwAddress, 0x52);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x33) // 2. eleman 3 ise
            {
              FLASH_ProgramByte(uwAddress, 0x53);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x34) // 2. eleman 4 ise
            {
              FLASH_ProgramByte(uwAddress, 0x54);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x35) // 2. eleman 5 ise
            {
              FLASH_ProgramByte(uwAddress, 0x55);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x36) // 2. eleman 6 ise
            {
              FLASH_ProgramByte(uwAddress, 0x56);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x37) // 2. eleman 7 ise
            {
              FLASH_ProgramByte(uwAddress, 0x57);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x38) // 2. eleman 8 ise
            {
              FLASH_ProgramByte(uwAddress, 0x58);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x39) // 2. eleman 9 ise
            {
              FLASH_ProgramByte(uwAddress, 0x59);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }    
      else if(ESP8266Buf[j+1] == 0x41) // 2. eleman A ise
            {
              FLASH_ProgramByte(uwAddress, 0x5A);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x42) // 2. eleman B ise
            {
              FLASH_ProgramByte(uwAddress, 0x5B);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x43) // 2. eleman C ise
            {
              FLASH_ProgramByte(uwAddress, 0x5C);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x44) // 2. eleman D ise
            {
              FLASH_ProgramByte(uwAddress, 0x5D);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x45) // 2. eleman E ise
            {
              FLASH_ProgramByte(uwAddress, 0x5E);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x46) // 2. eleman F ise
            {
              FLASH_ProgramByte(uwAddress, 0x5F);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
 
   }
   
      ////////////////////////////////////////////////////////////////////////////////////////
      //////////////////////////ILK ELEMAN 5 IÇIN TAMAMLANDI/////////////////////////////////
   
   else if(ESP8266Buf[j] == 0x36) // ilk 8 bit elemani 6 ise;
   {
      if(ESP8266Buf[j+1] == 0x30) // 2. eleman 0 ise
        {        
          FLASH_ProgramByte(uwAddress, 0x60);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
        }
       if(ESP8266Buf[j+1] == 0x31) // 2. eleman 1 ise
            {
              FLASH_ProgramByte(uwAddress, 0x61);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x32) // 2. eleman 2 ise
            {
              FLASH_ProgramByte(uwAddress, 0x62);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x33) // 2. eleman 3 ise
            {
              FLASH_ProgramByte(uwAddress, 0x63);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x34) // 2. eleman 4 ise
            {
              FLASH_ProgramByte(uwAddress, 0x64);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x35) // 2. eleman 5 ise
            {
              FLASH_ProgramByte(uwAddress, 0x65);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x36) // 2. eleman 6 ise
            {
              FLASH_ProgramByte(uwAddress, 0x66);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x37) // 2. eleman 7 ise
            {
              FLASH_ProgramByte(uwAddress, 0x67);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x38) // 2. eleman 8 ise
            {
              FLASH_ProgramByte(uwAddress, 0x68);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x39) // 2. eleman 9 ise
            {
              FLASH_ProgramByte(uwAddress, 0x69);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }    
      else if(ESP8266Buf[j+1] == 0x41) // 2. eleman A ise
            {
              FLASH_ProgramByte(uwAddress, 0x6A);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x42) // 2. eleman B ise
            {
              FLASH_ProgramByte(uwAddress, 0x6B);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x43) // 2. eleman C ise
            {
              FLASH_ProgramByte(uwAddress, 0x6C);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x44) // 2. eleman D ise
            {
              FLASH_ProgramByte(uwAddress, 0x6D);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x45) // 2. eleman E ise
            {
              FLASH_ProgramByte(uwAddress, 0x6E);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x46) // 2. eleman F ise
            {
              FLASH_ProgramByte(uwAddress, 0x6F);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
 
   }
   
      ////////////////////////////////////////////////////////////////////////////////////////
      //////////////////////////ILK ELEMAN 6 IÇIN TAMAMLANDI/////////////////////////////////
   
   else if(ESP8266Buf[j] == 0x37) // ilk 8 bit elemani 7 ise;
   {
      if(ESP8266Buf[j+1] == 0x30) // 2. eleman 0 ise
        {        
          FLASH_ProgramByte(uwAddress, 0x70);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
        }
      else if(ESP8266Buf[j+1] == 0x31) // 2. eleman 1 ise
            {
              FLASH_ProgramByte(uwAddress, 0x71);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x32) // 2. eleman 2 ise
            {
              FLASH_ProgramByte(uwAddress, 0x72);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x33) // 2. eleman 3 ise
            {
              FLASH_ProgramByte(uwAddress, 0x73);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x34) // 2. eleman 4 ise
            {
              FLASH_ProgramByte(uwAddress, 0x74);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x35) // 2. eleman 5 ise
            {
              FLASH_ProgramByte(uwAddress, 0x75);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x36) // 2. eleman 6 ise
            {
              FLASH_ProgramByte(uwAddress, 0x76);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x37) // 2. eleman 7 ise
            {
              FLASH_ProgramByte(uwAddress, 0x77);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x38) // 2. eleman 8 ise
            {
              FLASH_ProgramByte(uwAddress, 0x78);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x39) // 2. eleman 9 ise
            {
              FLASH_ProgramByte(uwAddress, 0x79);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }    
      else if(ESP8266Buf[j+1] == 0x41) // 2. eleman A ise
            {
              FLASH_ProgramByte(uwAddress, 0x7A);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x42) // 2. eleman B ise
            {
              FLASH_ProgramByte(uwAddress, 0x7B);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x43) // 2. eleman C ise
            {
              FLASH_ProgramByte(uwAddress, 0x7C);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x44) // 2. eleman D ise
            {
              FLASH_ProgramByte(uwAddress, 0x7D);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x45) // 2. eleman E ise
            {
              FLASH_ProgramByte(uwAddress, 0x7E);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x46) // 2. eleman F ise
            {
              FLASH_ProgramByte(uwAddress, 0x7F);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
 
   }
   
      ////////////////////////////////////////////////////////////////////////////////////////
      //////////////////////////ILK ELEMAN 7 IÇIN TAMAMLANDI/////////////////////////////////
   
   else if(ESP8266Buf[j] == 0x38) // ilk 8 bit elemani 8 ise;
   {
      if(ESP8266Buf[j+1] == 0x30) // 2. eleman 0 ise
        {        
          FLASH_ProgramByte(uwAddress, 0x80);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
        }
      else if(ESP8266Buf[j+1] == 0x31) // 2. eleman 1 ise
            {
              FLASH_ProgramByte(uwAddress, 0x81);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x32) // 2. eleman 2 ise
            {
              FLASH_ProgramByte(uwAddress, 0x82);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x33) // 2. eleman 3 ise
            {
              FLASH_ProgramByte(uwAddress, 0x83);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x34) // 2. eleman 4 ise
            {
              FLASH_ProgramByte(uwAddress, 0x84);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x35) // 2. eleman 5 ise
            {
              FLASH_ProgramByte(uwAddress, 0x85);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x36) // 2. eleman 6 ise
            {
              FLASH_ProgramByte(uwAddress, 0x86);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x37) // 2. eleman 7 ise
            {
              FLASH_ProgramByte(uwAddress, 0x87);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x38) // 2. eleman 8 ise
            {
              FLASH_ProgramByte(uwAddress, 0x88);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x39) // 2. eleman 9 ise
            {
              FLASH_ProgramByte(uwAddress, 0x89);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }    
      else if(ESP8266Buf[j+1] == 0x41) // 2. eleman A ise
            {
              FLASH_ProgramByte(uwAddress, 0x8A);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x42) // 2. eleman B ise
            {
              FLASH_ProgramByte(uwAddress, 0x8B);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x43) // 2. eleman C ise
            {
              FLASH_ProgramByte(uwAddress, 0x8C);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x44) // 2. eleman D ise
            {
              FLASH_ProgramByte(uwAddress, 0x8D);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x45) // 2. eleman E ise
            {
              FLASH_ProgramByte(uwAddress, 0x8E);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x46) // 2. eleman F ise
            {
              FLASH_ProgramByte(uwAddress, 0x8F);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
 
   }
   
      ////////////////////////////////////////////////////////////////////////////////////////
      //////////////////////////ILK ELEMAN 8 IÇIN TAMAMLANDI/////////////////////////////////
   
   else if(ESP8266Buf[j] == 0x39) // ilk 8 bit elemani 9 ise;
   {
      if(ESP8266Buf[j+1] == 0x30) // 2. eleman 0 ise
        {        
          FLASH_ProgramByte(uwAddress, 0x90);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
        }
      else if(ESP8266Buf[j+1] == 0x31) // 2. eleman 1 ise
            {
              FLASH_ProgramByte(uwAddress, 0x91);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x32) // 2. eleman 2 ise
            {
              FLASH_ProgramByte(uwAddress, 0x92);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x33) // 2. eleman 3 ise
            {
              FLASH_ProgramByte(uwAddress, 0x93);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x34) // 2. eleman 4 ise
            {
              FLASH_ProgramByte(uwAddress, 0x94);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x35) // 2. eleman 5 ise
            {
              FLASH_ProgramByte(uwAddress, 0x95);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x36) // 2. eleman 6 ise
            {
              FLASH_ProgramByte(uwAddress, 0x96);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x37) // 2. eleman 7 ise
            {
              FLASH_ProgramByte(uwAddress, 0x97);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x38) // 2. eleman 8 ise
            {
              FLASH_ProgramByte(uwAddress, 0x98);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x39) // 2. eleman 9 ise
            {
              FLASH_ProgramByte(uwAddress, 0x99);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }    
      else if(ESP8266Buf[j+1] == 0x41) // 2. eleman A ise
            {
              FLASH_ProgramByte(uwAddress, 0x9A);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x42) // 2. eleman B ise
            {
              FLASH_ProgramByte(uwAddress, 0x9B);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x43) // 2. eleman C ise
            {
              FLASH_ProgramByte(uwAddress, 0x9C);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x44) // 2. eleman D ise
            {
              FLASH_ProgramByte(uwAddress, 0x9D);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x45) // 2. eleman E ise
            {
              FLASH_ProgramByte(uwAddress, 0x9E);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x46) // 2. eleman F ise
            {
              FLASH_ProgramByte(uwAddress, 0x9F);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
 
   }
   
      ////////////////////////////////////////////////////////////////////////////////////////
      //////////////////////////ILK ELEMAN 9 IÇIN TAMAMLANDI/////////////////////////////////
   
   else if(ESP8266Buf[j] == 0x41) // ilk 8 bit elemani A ise;
   {
      if(ESP8266Buf[j+1] == 0x30) // 2. eleman 0 ise
        {        
          FLASH_ProgramByte(uwAddress, 0xA0);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
        }
      else if(ESP8266Buf[j+1] == 0x31) // 2. eleman 1 ise
            {
              FLASH_ProgramByte(uwAddress, 0xA1);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x32) // 2. eleman 2 ise
            {
              FLASH_ProgramByte(uwAddress, 0xA2);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x33) // 2. eleman 3 ise
            {
              FLASH_ProgramByte(uwAddress, 0xA3);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x34) // 2. eleman 4 ise
            {
              FLASH_ProgramByte(uwAddress, 0xA4);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x35) // 2. eleman 5 ise
            {
              FLASH_ProgramByte(uwAddress, 0xA5);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x36) // 2. eleman 6 ise
            {
              FLASH_ProgramByte(uwAddress, 0xA6);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x37) // 2. eleman 7 ise
            {
              FLASH_ProgramByte(uwAddress, 0xA7);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x38) // 2. eleman 8 ise
            {
              FLASH_ProgramByte(uwAddress, 0xA8);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x39) // 2. eleman 9 ise
            {
              FLASH_ProgramByte(uwAddress, 0xA9);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }    
      else if(ESP8266Buf[j+1] == 0x41) // 2. eleman A ise
            {
              FLASH_ProgramByte(uwAddress, 0xAA);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x42) // 2. eleman B ise
            {
              FLASH_ProgramByte(uwAddress, 0xAB);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x43) // 2. eleman C ise
            {
              FLASH_ProgramByte(uwAddress, 0xAC);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x44) // 2. eleman D ise
            {
              FLASH_ProgramByte(uwAddress, 0xAD);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x45) // 2. eleman E ise
            {
              FLASH_ProgramByte(uwAddress, 0xAE);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x46) // 2. eleman F ise
            {
              FLASH_ProgramByte(uwAddress, 0xAF);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
 
   }
   
      ////////////////////////////////////////////////////////////////////////////////////////
      //////////////////////////ILK ELEMAN A IÇIN TAMAMLANDI/////////////////////////////////
   
   else if(ESP8266Buf[j] == 0x42) // ilk 8 bit elemani B ise;
   {
      if(ESP8266Buf[j+1] == 0x30) // 2. eleman 0 ise
        {        
          FLASH_ProgramByte(uwAddress, 0xB0);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
        }
      else if(ESP8266Buf[j+1] == 0x31) // 2. eleman 1 ise
            {
              FLASH_ProgramByte(uwAddress, 0xB1);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x32) // 2. eleman 2 ise
            {
              FLASH_ProgramByte(uwAddress, 0xB2);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x33) // 2. eleman 3 ise
            {
              FLASH_ProgramByte(uwAddress, 0xB3);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x34) // 2. eleman 4 ise
            {
              FLASH_ProgramByte(uwAddress, 0xB4);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x35) // 2. eleman 5 ise
            {
              FLASH_ProgramByte(uwAddress, 0xB5);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x36) // 2. eleman 6 ise
            {
              FLASH_ProgramByte(uwAddress, 0xB6);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x37) // 2. eleman 7 ise
            {
              FLASH_ProgramByte(uwAddress, 0xB7);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x38) // 2. eleman 8 ise
            {
              FLASH_ProgramByte(uwAddress, 0xB8);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x39) // 2. eleman 9 ise
            {
              FLASH_ProgramByte(uwAddress, 0xB9);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }    
      else if(ESP8266Buf[j+1] == 0x41) // 2. eleman A ise
            {
              FLASH_ProgramByte(uwAddress, 0xBA);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x42) // 2. eleman B ise
            {
              FLASH_ProgramByte(uwAddress, 0xBB);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x43) // 2. eleman C ise
            {
              FLASH_ProgramByte(uwAddress, 0xBC);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x44) // 2. eleman D ise
            {
              FLASH_ProgramByte(uwAddress, 0xBD);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x45) // 2. eleman E ise
            {
              FLASH_ProgramByte(uwAddress, 0xBE);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x46) // 2. eleman F ise
            {
              FLASH_ProgramByte(uwAddress, 0xBF);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
 
   }
   
      ////////////////////////////////////////////////////////////////////////////////////////
      //////////////////////////ILK ELEMAN B IÇIN TAMAMLANDI/////////////////////////////////
   
   else if(ESP8266Buf[j] == 0x43) // ilk 8 bit elemani C ise;
   {
      if(ESP8266Buf[j+1] == 0x30) // 2. eleman 0 ise
        {        
          FLASH_ProgramByte(uwAddress, 0xC0);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
        }
      else if(ESP8266Buf[j+1] == 0x31) // 2. eleman 1 ise
            {
              FLASH_ProgramByte(uwAddress, 0xC1);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x32) // 2. eleman 2 ise
            {
              FLASH_ProgramByte(uwAddress, 0xC2);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x33) // 2. eleman 3 ise
            {
              FLASH_ProgramByte(uwAddress, 0xC3);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x34) // 2. eleman 4 ise
            {
              FLASH_ProgramByte(uwAddress, 0xC4);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x35) // 2. eleman 5 ise
            {
              FLASH_ProgramByte(uwAddress, 0xC5);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x36) // 2. eleman 6 ise
            {
              FLASH_ProgramByte(uwAddress, 0xC6);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x37) // 2. eleman 7 ise
            {
              FLASH_ProgramByte(uwAddress, 0xC7);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x38) // 2. eleman 8 ise
            {
              FLASH_ProgramByte(uwAddress, 0xC8);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x39) // 2. eleman 9 ise
            {
              FLASH_ProgramByte(uwAddress, 0xC9);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }    
      else if(ESP8266Buf[j+1] == 0x41) // 2. eleman A ise
            {
              FLASH_ProgramByte(uwAddress, 0xCA);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x42) // 2. eleman B ise
            {
              FLASH_ProgramByte(uwAddress, 0xCB);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x43) // 2. eleman C ise
            {
              FLASH_ProgramByte(uwAddress, 0xCC);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x44) // 2. eleman D ise
            {
              FLASH_ProgramByte(uwAddress, 0xCD);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x45) // 2. eleman E ise
            {
              FLASH_ProgramByte(uwAddress, 0xCE);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x46) // 2. eleman F ise
            {
              FLASH_ProgramByte(uwAddress, 0xCF);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
 
   }
   
      ////////////////////////////////////////////////////////////////////////////////////////
      //////////////////////////ILK ELEMAN C IÇIN TAMAMLANDI/////////////////////////////////
   
   else if(ESP8266Buf[j] == 0x44) // ilk 8 bit elemani D ise;
   {
      if(ESP8266Buf[j+1] == 0x30) // 2. eleman 0 ise
        {        
          FLASH_ProgramByte(uwAddress, 0xD0);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
        }
      else if(ESP8266Buf[j+1] == 0x31) // 2. eleman 1 ise
            {
              FLASH_ProgramByte(uwAddress, 0xD1);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x32) // 2. eleman 2 ise
            {
              FLASH_ProgramByte(uwAddress, 0xD2);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x33) // 2. eleman 3 ise
            {
              FLASH_ProgramByte(uwAddress, 0xD3);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x34) // 2. eleman 4 ise
            {
              FLASH_ProgramByte(uwAddress, 0xD4);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x35) // 2. eleman 5 ise
            {
              FLASH_ProgramByte(uwAddress, 0xD5);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x36) // 2. eleman 6 ise
            {
              FLASH_ProgramByte(uwAddress, 0xD6);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x37) // 2. eleman 7 ise
            {
              FLASH_ProgramByte(uwAddress, 0xD7);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x38) // 2. eleman 8 ise
            {
              FLASH_ProgramByte(uwAddress, 0xD8);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x39) // 2. eleman 9 ise
            {
              FLASH_ProgramByte(uwAddress, 0xD9);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }    
      else if(ESP8266Buf[j+1] == 0x41) // 2. eleman A ise
            {
              FLASH_ProgramByte(uwAddress, 0xDA);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x42) // 2. eleman B ise
            {
              FLASH_ProgramByte(uwAddress, 0xDB);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x43) // 2. eleman C ise
            {
              FLASH_ProgramByte(uwAddress, 0xDC);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x44) // 2. eleman D ise
            {
              FLASH_ProgramByte(uwAddress, 0xDD);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x45) // 2. eleman E ise
            {
              FLASH_ProgramByte(uwAddress, 0xDE);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x46) // 2. eleman F ise
            {
              FLASH_ProgramByte(uwAddress, 0xDF);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
 
   }
   
      ////////////////////////////////////////////////////////////////////////////////////////
      //////////////////////////ILK ELEMAN D IÇIN TAMAMLANDI/////////////////////////////////
   
   else if(ESP8266Buf[j] == 0x45) // ilk 8 bit elemani 1 ise;
   {
      if(ESP8266Buf[j+1] == 0x30) // 2. eleman 0 ise
        {        
          FLASH_ProgramByte(uwAddress, 0xE0);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
        }
      else if(ESP8266Buf[j+1] == 0x31) // 2. eleman 1 ise
            {
              FLASH_ProgramByte(uwAddress, 0xE1);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x32) // 2. eleman 2 ise
            {
              FLASH_ProgramByte(uwAddress, 0xE2);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x33) // 2. eleman 3 ise
            {
              FLASH_ProgramByte(uwAddress, 0xE3);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x34) // 2. eleman 4 ise
            {
              FLASH_ProgramByte(uwAddress, 0xE4);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x35) // 2. eleman 5 ise
            {
              FLASH_ProgramByte(uwAddress, 0xE5);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x36) // 2. eleman 6 ise
            {
              FLASH_ProgramByte(uwAddress, 0xE6);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x37) // 2. eleman 7 ise
            {
              FLASH_ProgramByte(uwAddress, 0xE7);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x38) // 2. eleman 8 ise
            {
              FLASH_ProgramByte(uwAddress, 0xE8);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x39) // 2. eleman 9 ise
            {
              FLASH_ProgramByte(uwAddress, 0xE9);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }    
      else if(ESP8266Buf[j+1] == 0x41) // 2. eleman A ise
            {
              FLASH_ProgramByte(uwAddress, 0xEA);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x42) // 2. eleman B ise
            {
              FLASH_ProgramByte(uwAddress, 0xEB);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x43) // 2. eleman C ise
            {
              FLASH_ProgramByte(uwAddress, 0xEC);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x44) // 2. eleman D ise
            {
              FLASH_ProgramByte(uwAddress, 0xED);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x45) // 2. eleman E ise
            {
              FLASH_ProgramByte(uwAddress, 0xEE);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x46) // 2. eleman F ise
            {
              FLASH_ProgramByte(uwAddress, 0xEF);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
 
   }
   
      ////////////////////////////////////////////////////////////////////////////////////////
      //////////////////////////ILK ELEMAN E IÇIN TAMAMLANDI/////////////////////////////////
   
   else if(ESP8266Buf[j] == 0x46) // ilk 8 bit elemani F ise;
   {
      if(ESP8266Buf[j+1] == 0x30) // 2. eleman 0 ise
        {        
          FLASH_ProgramByte(uwAddress, 0xF0);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
        }
      else if(ESP8266Buf[j+1] == 0x31) // 2. eleman 1 ise
            {
              FLASH_ProgramByte(uwAddress, 0xF1);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x32) // 2. eleman 2 ise
            {
              FLASH_ProgramByte(uwAddress, 0xF2);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x33) // 2. eleman 3 ise
            {
              FLASH_ProgramByte(uwAddress, 0xF3);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x34) // 2. eleman 4 ise
            {
              FLASH_ProgramByte(uwAddress, 0xF4);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x35) // 2. eleman 5 ise
            {
              FLASH_ProgramByte(uwAddress, 0xF5);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x36) // 2. eleman 6 ise
            {
              FLASH_ProgramByte(uwAddress, 0xF6);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x37) // 2. eleman 7 ise
            {
              FLASH_ProgramByte(uwAddress, 0xF7);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x38) // 2. eleman 8 ise
            {
              FLASH_ProgramByte(uwAddress, 0xF8);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x39) // 2. eleman 9 ise
            {
              FLASH_ProgramByte(uwAddress, 0xF9);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }    
      else if(ESP8266Buf[j+1] == 0x41) // 2. eleman A ise
            {
              FLASH_ProgramByte(uwAddress, 0xFA);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x42) // 2. eleman B ise
            {
              FLASH_ProgramByte(uwAddress, 0xFB);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x43) // 2. eleman C ise
            {
              FLASH_ProgramByte(uwAddress, 0xFC);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x44) // 2. eleman D ise
            {
              FLASH_ProgramByte(uwAddress, 0xFD);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x45) // 2. eleman E ise
            {
              FLASH_ProgramByte(uwAddress, 0xFE);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
      else if(ESP8266Buf[j+1] == 0x46) // 2. eleman F ise
            {
              FLASH_ProgramByte(uwAddress, 0xFF);
             uwAddress = uwAddress + 1;  
           delay(12000);  j++;   
            
            }
 
   }
   
    ////////////////////////////////////////////////////////////////////////////////////////
      //////////////////////////ILK ELEMAN F IÇIN TAMAMLANDI/////////////////////////////////
   
   
   else if(ESP8266Buf[j] == 0x53)        // SEND = kismi atlandi
   {
     if(ESP8266Buf[j+1] == 0x45)
     {
       if(ESP8266Buf[j+2] == 0x4e)
       {
         if(ESP8266Buf[j+3] == 0x44)
         {
           if(ESP8266Buf[j+4] == 0x3d)
           {
         j=9+j;
           }
         }
       }
     }
   }
   
   
   else if(ESP8266Buf[j] == 0x2b)   // +IPD,1,14 kismi 
   {
     if(ESP8266Buf[j+1] == 0x49)
     {
       if(ESP8266Buf[j+2] == 0x50)
       {
         if (ESP8266Buf[j+3] == 0x44)
         {
           if (ESP8266Buf[j+4] == 0x2c)
           {
             if (ESP8266Buf[j+5] == 0x31)
             {
               if (ESP8266Buf[j+6] == 0x2c)
               {
                 if (ESP8266Buf[j+7] == 0x31)
                 {
                   if (ESP8266Buf[j+8] == 0x34)
                   {
                     j=j+9;
                   }                   
                 }
               }
             }
           }                     
         }
       }
     }
   }
   
   
   else if (ESP8266Buf[j] == 0x78)
   {
    if (ESP8266Buf[j+2] == 0x2b) 
    {
      if (ESP8266Buf[j+3] == 0x49)
      {
        if (ESP8266Buf[j+4] == 0x50)
        {
          if (ESP8266Buf[j+5] == 0x44)
          {
            if (ESP8266Buf[j+6] == 0x2c)
            {
              if (ESP8266Buf[j+7] == 0x31)
              {
                if (ESP8266Buf[j+8] == 0x2c)
                {
                  if (ESP8266Buf[j+9] == 0x31)
                  {
                    if (ESP8266Buf[j+10] == 0x3a)
                    {
        FLASH_ProgramByte(uwAddress, 0x00);
             uwAddress = uwAddress + 1;   
           delay(12000); j=j+3;
                    }
                  }
                }
              }
            }
          }
        }
      }
    }  
    else if (ESP8266Buf[j+4] == 0x2c)
    {
      FLASH_ProgramByte(uwAddress, 0x00);
             uwAddress = uwAddress + 1;   
           delay(12000); 
           
           FLASH_ProgramByte(uwAddress, 0x00);
             uwAddress = uwAddress + 1;   
           delay(12000); j=j+4; 
    }
   }
   
   
   
   
    
   
   
   
   
   else if(ESP8266Buf[j] == 0x3a)   // : den 2 ötedeyse 00 4 ötedeyse 0000
   {
     if(ESP8266Buf[j+2] == 0x2c)
     {
       FLASH_ProgramByte(uwAddress, 0x00);
             uwAddress = uwAddress + 1; 
           delay(12000);  j++;      
     }
     else if(ESP8266Buf[j+4] == 0x2c)
     {
       FLASH_ProgramByte(uwAddress, 0x00);
             uwAddress = uwAddress + 1;   
           delay(12000); 
           
           FLASH_ProgramByte(uwAddress, 0x00);
             uwAddress = uwAddress + 1;   
           delay(12000); j=j+3;      
           
     }
   }

   
   
   else if ((ESP8266Buf[j]==0x7d) && (ESP8266Buf[j+1]==0x3b))
                                     {
				       break;				
                                     } 
   
     
   if (j == 180000)
   {
     
     j=0;
   }

  
                            
                                           



   }
} 


static uint32_t GetSector(uint32_t Address)
{
  uint32_t sector = 0;
  
  if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
  {
    sector = FLASH_Sector_0;  
  }
  else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
  {
    sector = FLASH_Sector_1;  
  }
  else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
  {
    sector = FLASH_Sector_2;  
  }
  else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
  {
    sector = FLASH_Sector_3;  
  }
  else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
  {
    sector = FLASH_Sector_4;  
  }
  else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
  {
    sector = FLASH_Sector_5;  
  }
  else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
  {
    sector = FLASH_Sector_6;  
  }
  else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
  {
    sector = FLASH_Sector_7;  
  }
  else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
  {
    sector = FLASH_Sector_8;  
  }
  else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
  {
    sector = FLASH_Sector_9;  
  }
  else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
  {
    sector = FLASH_Sector_10;  
  }
  else if((Address < ADDR_FLASH_SECTOR_12) && (Address >= ADDR_FLASH_SECTOR_11))
  {
    sector = FLASH_Sector_11;  
  }

  else if((Address < ADDR_FLASH_SECTOR_13) && (Address >= ADDR_FLASH_SECTOR_12))
  {
    sector = FLASH_Sector_12;  
  }
  else if((Address < ADDR_FLASH_SECTOR_14) && (Address >= ADDR_FLASH_SECTOR_13))
  {
    sector = FLASH_Sector_13;  
  }
  else if((Address < ADDR_FLASH_SECTOR_15) && (Address >= ADDR_FLASH_SECTOR_14))
  {
    sector = FLASH_Sector_14;  
  }
  else if((Address < ADDR_FLASH_SECTOR_16) && (Address >= ADDR_FLASH_SECTOR_15))
  {
    sector = FLASH_Sector_15;  
  }
  else if((Address < ADDR_FLASH_SECTOR_17) && (Address >= ADDR_FLASH_SECTOR_16))
  {
    sector = FLASH_Sector_16;  
  }
  else if((Address < ADDR_FLASH_SECTOR_18) && (Address >= ADDR_FLASH_SECTOR_17))
  {
    sector = FLASH_Sector_17;  
  }
  else if((Address < ADDR_FLASH_SECTOR_19) && (Address >= ADDR_FLASH_SECTOR_18))
  {
    sector = FLASH_Sector_18;  
  }
  else if((Address < ADDR_FLASH_SECTOR_20) && (Address >= ADDR_FLASH_SECTOR_19))
  {
    sector = FLASH_Sector_19;  
  }
  else if((Address < ADDR_FLASH_SECTOR_21) && (Address >= ADDR_FLASH_SECTOR_20))
  {
    sector = FLASH_Sector_20;  
  } 
  else if((Address < ADDR_FLASH_SECTOR_22) && (Address >= ADDR_FLASH_SECTOR_21))
  {
    sector = FLASH_Sector_21;  
  }
  else if((Address < ADDR_FLASH_SECTOR_23) && (Address >= ADDR_FLASH_SECTOR_22))
  {
    sector = FLASH_Sector_22;  
  }
  else/*(Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_23))*/
  {
    sector = FLASH_Sector_23;  
  }
  return sector;
}
