#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "stmflash.h" 
#include "key.h"  
#include "sram.h"   
#include "malloc.h" 
#include "sdio_sdcard.h"    
#include "malloc.h" 
#include "w25qxx.h"    
#include "ff.h"  
#include "exfuns.h"  
#include "stdio.h"
#include "RingQueue.h" 
#include "stddef.h"
#include "init_xiaojia.h"
#include "doubleRingBuffer_xiaojia.h"
#include "rtc.h"
#include "stm32f4xx_rtc.h"
#include "dma.h"

#include <typedef.h>
#include "ringbuffer.h"
/*************************************************************************/

/*************************************************************************/
extern uint8_t uart1Rx_buf[4096];
extern struct xl_ringbuffer uart1RxRingBuf;


//extern u8 ReceiveBuff[];
//extern u8 RemainDataCount;
//extern u8 ReceivedData[];
//extern u8 RemainDataCount;
extern DWORD fre_clust, fre_sect, tot_sect;//空余簇数，空余扇区数，总扇区数
extern FATFS *ft;


static unsigned char RxBuffer[RX_BUF_MAX_SIZE];
unsigned char err;
UINT MY_br,MY_bw;
unsigned char buffer[2048] ;
char get_time_filename[20];
FIL fl;
DIR dy; 
FRESULT res;
FILINFO flinfo;
/*结构体定义*/
RTC_TimeTypeDef RTC_TimeStruct;
RTC_DateTypeDef RTC_DateStruct;
time get_time;//放置时间的结构体，每次上电获取时间先读RTC，然后赋给time结构体，取数时直接取time里的结构成员


/*******************************************************************************/

/*******************************************************************************/

/*************************************************************************/
void USART1_Init(u32 boud)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//使能USART1时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate=boud;
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStructure.USART_Parity=USART_Parity_No;
	USART_InitStructure.USART_StopBits=USART_StopBits_1;
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;
	USART_Init(USART1,&USART_InitStructure);
	
	
	USART_Cmd(USART1,ENABLE);
	USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);
//	USART_ITConfig(USART1,USART_IT_IDLE,ENABLE);
//	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	USART_ClearFlag(USART1, USART_FLAG_TC);

	NVIC_InitStructure.NVIC_IRQChannel=DMA2_Stream5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//最高优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//初始化缓存区
	xl_ringbuffer_init(&uart1RxRingBuf, uart1Rx_buf, 4096);
	
}
/****************************************************************************/

/*************************************************************************/
void MY_init(void)
{
NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
MYDMA_Init(DMA2_Stream5,DMA_Channel_4);
//MYDMA_Enable(DMA2_Stream5,2048);
	
	USART1_Init(921600);
	
	delay_init(168);  //初始化延时函数
	
	LED_Init();					//初始化LED 
//	W25QXX_Init();				//初始化W25Q128
	my_mem_init(SRAMIN);		//初始化内部内存池 
	my_mem_init(SRAMCCM);		//初始化CCM内存池
	My_RTC_Init();
}
/******************************************************************************/

/******************************************************************************/
void check_SD(void)
{	
while(SD_Init())//检测不到SD卡
	{
		delay_ms(2000);                  //报警2：检测不到SD卡
		LED0=!LED0;//DS0闪烁
	}
}
/*****************************************************************************/

/*******************************************************************************/

/*************************************************************************/
void mount_fatfs(void)
{
         exfuns_init();							//为fatfs相关变量申请内存				 
         f_mount(fs[0],"0:",1); 					//挂载SD卡 
}
/************************************************************************/

/************************************************************************/
void create_a_file(void)
{

	//获取RTC时间
			RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);	
			RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);


	/**********调试时候用，可以删除*****************/
	get_time.year = RTC_DateStruct.RTC_Year;
	get_time.month = RTC_DateStruct.RTC_Month;
	get_time.day = RTC_DateStruct.RTC_Date;
	get_time.hours =RTC_TimeStruct.RTC_Hours;
	get_time.min = RTC_TimeStruct.RTC_Minutes;
	/***********************************************/
	sprintf(get_time_filename,"0:%d-%d-%d-%d-%d.bin",get_time.year,get_time.month,get_time.day,get_time.hours,get_time.min);
	//此处写获取时间，以时间作为文件名，每次上电新建一个文件，并查询该目录下文件数
	res = f_open(&fl,get_time_filename,FA_CREATE_NEW|FA_WRITE);//打开/创建文件 在驱动器0 xf.txt
	res = f_close(&fl);
	          f_getfree("0:",&fre_clust,&ft);//获取全部扇区数和空余扇区数
            tot_sect =(ft->n_fatent -2) * ft->csize; //total扇区数
            fre_sect = fre_clust * ft->csize;//free扇区数
//	          if(fre_sect<0x40000000) 
//						{
//						while(1)//如果剩余内存小于1G，则视为内存不足，两个LED闪烁
//						 {
//						LED0 = !LED0;
//						LED1 = !LED1;               //报警1：SD卡内存不足报警
//							 delay_ms(300);
//						 }
//			     }
}














