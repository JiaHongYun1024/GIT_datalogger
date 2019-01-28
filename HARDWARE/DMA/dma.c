#include "dma.h"																	   	  
#include "delay.h"	
#include "ringbuffer.h"  
/************************************************************************************/
#define USART1MAXSIZE 4096;
/************************************************************************************/
extern uint8_t uart1Rx_buf[4096];
extern struct xl_ringbuffer uart1RxRingBuf;
extern uint16_t write_index;
u8 DMAITFlag = 0;
void MYDMA_Init(DMA_Stream_TypeDef *DMA_Streamx,uint32_t chx)
{ 
	DMA_InitTypeDef  DMA_InitStructure;
	
	//if((u32)DMA_Streamx>(u32)DMA2)
	//{
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE); 
		
	//}else 
	//{
	//  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);
	//}
  DMA_DeInit(DMA_Streamx);
	
	while (DMA_GetCmdStatus(DMA_Streamx) != DISABLE){}
	
  /* DMA Stream */
  DMA_InitStructure.DMA_Channel = chx;  
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)uart1Rx_buf;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_BufferSize = (uint32_t)USART1MAXSIZE;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize =DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_ITConfig(DMA2_Stream5,DMA_IT_TC,ENABLE);
	DMA_ITConfig(DMA2_Stream5,DMA_IT_HT,ENABLE);
  DMA_Init(DMA_Streamx, &DMA_InitStructure);
	DMA_Cmd(DMA_Streamx, ENABLE);//开始DMA循环模式
	
 
} 

 
void DMA2_Stream5_IRQHandler(void)
{
	if(DMA_GetFlagStatus(DMA2_Stream5,DMA_FLAG_HTIF5|DMA_FLAG_TCIF5)!=RESET) { //一半中断
		       DMA_ClearFlag(DMA2_Stream5,DMA_FLAG_HTIF5|DMA_FLAG_TCIF5);
				 	 xl_ringbuffer_movewriteindex(&uart1RxRingBuf,2048);
		       DMAITFlag=1;
	}
}




