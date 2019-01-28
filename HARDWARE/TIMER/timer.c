#include "timer.h"
#include "ringbuffer.h"
#include "ff.h" 
#include "dma.h"																	   	  
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz
extern unsigned char buffer[2048];
extern struct xl_ringbuffer uart1RxRingBuf;
extern UINT MY_br;
extern void writeSDCard(void);


//TIM2��ʼ������******************************************************************************************************************************************************
void TIM2_Int_Init(u32 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);  ///ʹ��TIM2ʱ��,84MHz
	
	TIM_TimeBaseInitStructure.TIM_Period = arr; 	//�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);//��ʼ��TIM2
	TIM2->CNT=0;
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);  //����жϱ�־λ
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE); //����ʱ��2�����ж�
	TIM2->CNT=0;
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);  //����жϱ�־λ
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn; //��ʱ��2�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x03; //��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}
//*******************************************************************************************************************************************************************




//��ʱ��2�жϷ�������10s����һ���ж�**************************************************************************************************************************************************
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET) //����ж�
	{
		     MY_br = 4096-DMA_GetCurrDataCounter(DMA2_Stream1);//�Ѿ�д����ֽ���
		if(MY_br<=2048)//û�д�������һ���ж�
		{
				 xl_ringbuffer_movewriteindex(&uart1RxRingBuf,MY_br);
			   xl_ringbuffer_get(&uart1RxRingBuf,buffer,MY_br);   
				 writeSDCard();	                                  //�����λ�������ʣ������һ�ֲ���д��SD��
		}
		else          //�Ѿ������˴���һ���ж�
		{
		MY_br = MY_br-2048;
				 xl_ringbuffer_movewriteindex(&uart1RxRingBuf,MY_br);//
			   xl_ringbuffer_get(&uart1RxRingBuf,buffer,MY_br);   
				 writeSDCard();	 			//�����λ�������ʣ������һ�ֲ���д��SD��
		}
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);  //����жϱ�־λ
    GPIO_ResetBits(GPIOB,GPIO_Pin_0);//  �ɻ����������̵Ƴ���
while(1);
	}
}
//*******************************************************************************************************************************************************************

