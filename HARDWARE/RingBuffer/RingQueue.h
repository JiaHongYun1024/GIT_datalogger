#ifndef __RingQueue_H
#define	__RingQueue_H	   
#include "sys.h"
/************************************************************
文件名称：环形缓冲区.h
作者:贾泓运
************************************************************/

void MyRingBuff_init();
unsigned char writedata_RingBuff(unsigned int wanttowrite,unsigned char *buff);
unsigned char readdata_RingBuff(unsigned int wanttoread,unsigned char *buff);






#endif