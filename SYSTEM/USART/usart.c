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
u16 USART_RX_STA=0;       //����״̬���	

//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif

//ALIENTEK ̽����STM32F407������ ʵ��0
//STM32F4����ģ��-�⺯���汾
//����֧�֣�www.openedv.com
//�Ա����̣�http://eboard.taobao.com
//������������ӿƼ����޹�˾  
//���ߣ�����ԭ�� @ALIENTEK

//void USART1_IRQHandler(void)
//{
//	//int i;	
//	uint8_t data;
//	
//	
//	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET) { //��ʱ�ж��ж�һ֡�����Ƿ�������
//     USART1->SR;
//		 USART1->DR;
//		 dataget = 1;
////		 INCNT++;
//	                                                      }

//	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) { //�����ж�
//		data = (USART1->DR & 0x1FF);
//		xl_ringbuffer_putchar(&uart1RxRingBuf, data);
//                                                         	}																														
//}


 
