#include "timer.h"
#include "ringbuffer.h"
#include "ff.h" 
#include "dma.h"																	   	  
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
extern unsigned char buffer[2048];
extern struct xl_ringbuffer uart1RxRingBuf;
extern UINT MY_br;
extern void writeSDCard(void);


//TIM2初始化函数******************************************************************************************************************************************************
void TIM2_Int_Init(u32 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);  ///使能TIM2时钟,84MHz
	
	TIM_TimeBaseInitStructure.TIM_Period = arr; 	//自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);//初始化TIM2
	TIM2->CNT=0;
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);  //清除中断标志位
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE); //允许定时器2更新中断
	TIM2->CNT=0;
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);  //清除中断标志位
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn; //定时器2中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x03; //抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}
//*******************************************************************************************************************************************************************




//定时器2中断服务函数，10s进入一次中断**************************************************************************************************************************************************
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET) //溢出中断
	{
		     MY_br = 4096-DMA_GetCurrDataCounter(DMA2_Stream1);//已经写入的字节数
		if(MY_br<=2048)//没有触发传输一半中断
		{
				 xl_ringbuffer_movewriteindex(&uart1RxRingBuf,MY_br);
			   xl_ringbuffer_get(&uart1RxRingBuf,buffer,MY_br);   
				 writeSDCard();	                                  //将环形缓冲区内剩余数据一字不落写进SD卡
		}
		else          //已经触发了传输一半中断
		{
		MY_br = MY_br-2048;
				 xl_ringbuffer_movewriteindex(&uart1RxRingBuf,MY_br);//
			   xl_ringbuffer_get(&uart1RxRingBuf,buffer,MY_br);   
				 writeSDCard();	 			//将环形缓冲区内剩余数据一字不落写进SD卡
		}
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);  //清除中断标志位
    GPIO_ResetBits(GPIOB,GPIO_Pin_0);//  飞机上锁，红绿灯常亮
while(1);
	}
}
//*******************************************************************************************************************************************************************

