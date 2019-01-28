///*
//*********************************************************************************************************
//*
//*
//*                                      RingQueueStruct
//*                                        ??????
//*
//* File : RingQueue.c
//* By   : Lin Shijun(http://blog.csdn.net/lin_strong)
//* Date : 2017/10/16
//* version: V1.1 
//* NOTE(s): 
//*
//* History : 2017/04/25   the original version of RingQueueStruct.
//*           2017/10/16   put functions used frequently,RingQueueIn and RingQueueOut, in non-banked address;
//*                        modify single line function RingQueueIsEmpty and RingQueueIsFull to marco function;
//*                        to get better efficiency.  
//*********************************************************************************************************
//*/

///*
//*********************************************************************************************************
//*                                     INCLUDES
//*********************************************************************************************************
//*/
//#include "RingQueue.h"

///*
//*********************************************************************************************************
//*                                   CONSTANTS  ??
//*********************************************************************************************************
//*/


///*
//*********************************************************************************************************
//*                                LOCAL FUNCTION DECLARE
//*********************************************************************************************************
//*/
////#pragma CODE_SEG __NEAR_SEG NON_BANKED
////// ????,????????????????????(???????)
////static void forwardPointer(RING_QUEUE *pQueue,pRQTYPE* pPointer);
////#pragma CODE_SEG DEFAULT
///*
///*
//*********************************************************************************************************
//*                                        RingQueueIn()
//*
//* Description : Enqueue an element.    ??????
//*
//* Arguments   : pQueue   pointer to the ring queue control block;    ????????????
//*               data     the data to enqueue;                        ??????
//*               option   option when queue is full ,you can choose:  ??????????,?????:
//*                            RQ_OPTION_WHEN_FULL_DISCARD_FIRST          ???????????????
//*                            RQ_OPTION_WHEN_FULL_DONT_IN                ????????
//*               perr     a pointer to a variable containing an error message which will be set by this
//*                          function to either:
//*
//*                             RQ_ERR_NONE                            if no err happen
//*                             RQ_ERR_POINTER_NULL                    if pointer is 0x00
//*                             RQ_ERR_BUFFER_FULL                     if buffer is full
//*
//* Return       : the Elements Count after enqueue the element
//*                    ?????????????
//*Note(s)       :
//*********************************************************************************************************
//*/
///*
//*********************************************************************************************************
//*                                        RingQueueOut()
//*
//* Description : Dequeue an element.       ??????
//*
//* Arguments   : pQueue   pointer to the ring queue control block;     ????????????
//*               perr     a pointer to a variable containing an error message which will be set by this
//*                          function to either:
//*
//*                              RQ_ERR_NONE                            if no err happen
//*                              RQ_ERR_POINTER_NULL                    if pointer is 0x00
//*                              RQ_ERR_BUFFER_EMPTY                    if buffer is empty
//*
//* Return      : the data 
//*               0                 if any error;
//*Note(s):
//*********************************************************************************************************
//*/
//RQTYPE RingQueueOut(RING_QUEUE *pQueue,unsigned char *perr){
//     RQTYPE data;
//     if(pQueue == 0x00) {
//          *perr = RQ_ERR_POINTER_NULL;          
//          return 0;
//     }
//     if(pQueue->RingBufCtr == 0){
//          *perr = RQ_ERR_BUFFER_EMPTY;        
//          return 0;  
//     }
//     pQueue->RingBufCtr--;      		 /*  decrement character count           */  
//    
//		 data = *pQueue->RingBufOutPtr;                      /* Get character from buffer                */  
//     forwardPointer(pQueue,&pQueue->RingBufOutPtr);        /* Wrap OUT pointer                          */  
//     *perr = RQ_ERR_NONE;
//     return data;
//}

//#pragma CODE_SEG __NEAR_SEG NON_BANKED
//unsigned short RingQueueIn(RING_QUEUE *pQueue,RQTYPE data,unsigned char option,unsigned char *perr){
//     if(pQueue == 0x00){
//          *perr = RQ_ERR_POINTER_NULL;         
//          return 0; 
//     }
//     if(pQueue->RingBufCtr >= pQueue->RingBufSize){
//           *perr = RQ_ERR_BUFFER_FULL;     
//           if(option == RQ_OPTION_WHEN_FULL_DISCARD_FIRST){
//               forwardPointer(pQueue,&pQueue->RingBufOutPtr); /* Wrap OUT pointer                          */  
//           }else{                                                  // option == RQ_OPTION_WHEN_FULL_DONT_IN
//               return pQueue->RingBufCtr;
//           }
//     }else{
//           pQueue->RingBufCtr++;                                           /* No, increment character count            */      
//           *perr = RQ_ERR_NONE;
//     }
//      *pQueue->RingBufInPtr = data;                                /* Put character into buffer                */  
//      forwardPointer(pQueue,&pQueue->RingBufInPtr);   /* Wrap IN pointer                          */  
//     return pQueue->RingBufCtr;
//}

//#pragma CODE_SEG DEFAULT



