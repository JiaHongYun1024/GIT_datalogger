#ifndef __RingQueue_H
#define	__RingQueue_H	   
#include "sys.h"
/************************************************************
�ļ����ƣ����λ�����.h
����:������
************************************************************/

void MyRingBuff_init();
unsigned char writedata_RingBuff(unsigned int wanttowrite,unsigned char *buff);
unsigned char readdata_RingBuff(unsigned int wanttoread,unsigned char *buff);






#endif