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
extern DWORD fre_clust, fre_sect, tot_sect;//�����������������������������
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
/*�ṹ�嶨��*/
RTC_TimeTypeDef RTC_TimeStruct;
RTC_DateTypeDef RTC_DateStruct;
time get_time;//����ʱ��Ľṹ�壬ÿ���ϵ��ȡʱ���ȶ�RTC��Ȼ�󸳸�time�ṹ�壬ȡ��ʱֱ��ȡtime��Ľṹ��Ա


/*******************************************************************************/

/*******************************************************************************/

/*************************************************************************/
void USART1_Init(u32 boud)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//ʹ��USART1ʱ��
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
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//������ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//��ʼ��������
	xl_ringbuffer_init(&uart1RxRingBuf, uart1Rx_buf, 4096);
	
}
/****************************************************************************/

/*************************************************************************/
void MY_init(void)
{
NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
MYDMA_Init(DMA2_Stream5,DMA_Channel_4);
//MYDMA_Enable(DMA2_Stream5,2048);
	
	USART1_Init(921600);
	
	delay_init(168);  //��ʼ����ʱ����
	
	LED_Init();					//��ʼ��LED 
//	W25QXX_Init();				//��ʼ��W25Q128
	my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ�� 
	my_mem_init(SRAMCCM);		//��ʼ��CCM�ڴ��
	My_RTC_Init();
}
/******************************************************************************/

/******************************************************************************/
void check_SD(void)
{	
while(SD_Init())//��ⲻ��SD��
	{
		delay_ms(2000);                  //����2����ⲻ��SD��
		LED0=!LED0;//DS0��˸
	}
}
/*****************************************************************************/

/*******************************************************************************/

/*************************************************************************/
void mount_fatfs(void)
{
         exfuns_init();							//Ϊfatfs��ر��������ڴ�				 
         f_mount(fs[0],"0:",1); 					//����SD�� 
}
/************************************************************************/

/************************************************************************/
void create_a_file(void)
{

	//��ȡRTCʱ��
			RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);	
			RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);


	/**********����ʱ���ã�����ɾ��*****************/
	get_time.year = RTC_DateStruct.RTC_Year;
	get_time.month = RTC_DateStruct.RTC_Month;
	get_time.day = RTC_DateStruct.RTC_Date;
	get_time.hours =RTC_TimeStruct.RTC_Hours;
	get_time.min = RTC_TimeStruct.RTC_Minutes;
	/***********************************************/
	sprintf(get_time_filename,"0:%d-%d-%d-%d-%d.bin",get_time.year,get_time.month,get_time.day,get_time.hours,get_time.min);
	//�˴�д��ȡʱ�䣬��ʱ����Ϊ�ļ�����ÿ���ϵ��½�һ���ļ�������ѯ��Ŀ¼���ļ���
	res = f_open(&fl,get_time_filename,FA_CREATE_NEW|FA_WRITE);//��/�����ļ� ��������0 xf.txt
	res = f_close(&fl);
	          f_getfree("0:",&fre_clust,&ft);//��ȡȫ���������Ϳ���������
            tot_sect =(ft->n_fatent -2) * ft->csize; //total������
            fre_sect = fre_clust * ft->csize;//free������
//	          if(fre_sect<0x40000000) 
//						{
//						while(1)//���ʣ���ڴ�С��1G������Ϊ�ڴ治�㣬����LED��˸
//						 {
//						LED0 = !LED0;
//						LED1 = !LED1;               //����1��SD���ڴ治�㱨��
//							 delay_ms(300);
//						 }
//			     }
}














