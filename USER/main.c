#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "beep.h"
#include "key.h"
#include "string.h"
#include "highpower.h"

//ALIENTEK ̽����STM32F407������ ʵ��4
//����ͨ��ʵ�� -�⺯���汾
//����֧�֣�www.openedv.com
//�Ա����̣�http://eboard.taobao.com
//������������ӿƼ����޹�˾  
//���ߣ�����ԭ�� @ALIENTEK
/*��������*/
//u16 find_datapackhead(u8 l);
//void get_data(u8 *destiny,u8 *src);
/*buffer����*/
extern u8 powerdata[256];
extern u8 dianliang[2];
/*�ⲿ����*/
extern u8 dataget;
int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);		//��ʱ��ʼ�� 
	uart_init(115200);	//���ڳ�ʼ��������Ϊ115200
	LED_Init();		  		//��ʼ����LED���ӵ�Ӳ���ӿ�  
	while(1)
	{
		power_API();
  }
}

/*data analize*/
//u16 find_datapackhead(u8 l)
//{
//u16 i;
//for(i=0;i<l;i++)
//	{
//	 if(USART_RX_BUF[i] == 0x06&&USART_RX_BUF[i-1] == 0x82) 
//	 return (i+1);
//	}
//}



/*get data*/ 
/*
powerdata[]       0~3         4~7     8~9��u16��    9~12
            ������ܵ���     �ܵ�ѹ   ����        ��һ�ڵ�ѹ  ... ... n = 54

*/
//void get_data(u8 *destiny,u8 *src)
//{
//u8 *pdptr,*rxptr;
//float fpower[60],f = 0.3;
//	pdptr=destiny;
//	rxptr = src;
//  memcpy(pdptr,rxptr,4);//������ܵ���
//	fpower[0] = *(float *)pdptr;
////	printf("������ܵ���Ϊ��%6fA",fpower[0]);
//	pdptr = pdptr+4;
//	rxptr = rxptr+4;
//	memcpy(pdptr,rxptr,4);//������ܵ�ѹ
//	fpower[1] = *(float *)pdptr;
//	pdptr = pdptr+4;
//	rxptr = rxptr+6;
//	memcpy(pdptr,rxptr,4); //��һ�ڵ�ص�ѹ
//	/*
//	.
//	. 
//	.
//	*/
//}


