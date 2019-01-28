#ifndef __DMA_H
#define	__DMA_H	   
#include "sys.h"
 
 

void MYDMA_Init(DMA_Stream_TypeDef *DMA_Streamx,uint32_t chx);
void MYDMA_Enable(DMA_Stream_TypeDef *DMA_Streamx,uint32_t ndtr);   
#endif
