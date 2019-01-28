#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "beep.h"
#include "key.h"
#include "string.h"
#include "highpower.h"

//ALIENTEK 探索者STM32F407开发板 实验4
//串口通信实验 -库函数版本
//技术支持：www.openedv.com
//淘宝店铺：http://eboard.taobao.com
//广州市星翼电子科技有限公司  
//作者：正点原子 @ALIENTEK
/*声明函数*/
//u16 find_datapackhead(u8 l);
//void get_data(u8 *destiny,u8 *src);
/*buffer定义*/
extern u8 powerdata[256];
extern u8 dianliang[2];
/*外部引用*/
extern u8 dataget;
int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);		//延时初始化 
	uart_init(115200);	//串口初始化波特率为115200
	LED_Init();		  		//初始化与LED连接的硬件接口  
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
powerdata[]       0~3         4~7     8~9（u16）    9~12
            电池组总电流     总电压   电量        第一节电压  ... ... n = 54

*/
//void get_data(u8 *destiny,u8 *src)
//{
//u8 *pdptr,*rxptr;
//float fpower[60],f = 0.3;
//	pdptr=destiny;
//	rxptr = src;
//  memcpy(pdptr,rxptr,4);//电池组总电流
//	fpower[0] = *(float *)pdptr;
////	printf("电池组总电流为：%6fA",fpower[0]);
//	pdptr = pdptr+4;
//	rxptr = rxptr+4;
//	memcpy(pdptr,rxptr,4);//电池组总电压
//	fpower[1] = *(float *)pdptr;
//	pdptr = pdptr+4;
//	rxptr = rxptr+6;
//	memcpy(pdptr,rxptr,4); //第一节电池电压
//	/*
//	.
//	. 
//	.
//	*/
//}


