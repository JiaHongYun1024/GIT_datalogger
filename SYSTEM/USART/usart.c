#include "stm32f4xx.h"
#include "usart.h"
#include "delay.h"
#include "dma.h"
#include "doubleRingBuffer_xiaojia.h"
#include "led.h"
#include "init_xiaojia.h"

#include <typedef.h>
#include "ringbuffer.h"



uint8_t uart1Rx_buf[4096];
struct xl_ringbuffer uart1RxRingBuf;


//u8 ReceiveBuff[2048];
//unsigned int  RemainDataCount;
unsigned int  INCNT = 0;;
//unsigned int RemainDataCount11 = 0;
//u8 ReceivedData[2048];
u8 dataget;
u16 USART_RX_STA=0;       //接收状态标记	

//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif

//ALIENTEK 探索者STM32F407开发板 实验0
//STM32F4工程模板-库函数版本
//技术支持：www.openedv.com
//淘宝店铺：http://eboard.taobao.com
//广州市星翼电子科技有限公司  
//作者：正点原子 @ALIENTEK

//void USART1_IRQHandler(void)
//{
//	//int i;	
//	uint8_t data;
//	
//	
//	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET) { //超时中断判断一帧数据是否接收完毕
//     USART1->SR;
//		 USART1->DR;
//		 dataget = 1;
////		 INCNT++;
//	                                                      }

//	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) { //接收中断
//		data = (USART1->DR & 0x1FF);
//		xl_ringbuffer_putchar(&uart1RxRingBuf, data);
//                                                         	}																														
//}


 
