#include "RingQueue.h"

 
 

/**********************************************************
�ļ����ƣ����λ�����.c
���ߣ�������
***********************************************************/
  unsigned char RingBuff[5120];//10k�Ļ��λ�����
	unsigned char *Wprb,*Rprb;
	unsigned int truedata;
/****************************************************
�������ƣ����λ�������ʼ��
����ֵ����
��ڲ�������
���ܣ���ʼ�����λ�����
****************************************************/

void MyRingBuff_init(void)
{
	Rprb = RingBuff;
	Wprb = RingBuff;
	truedata = 0;
}

/*************************************************************
�������ƣ�д���λ�����
����ֵ:0 ���������� ����⵽0�󣬽�����ȫ������д��SD��
��ڲ���������Ҫд�뻺������������wanttowrite,����Դ��ַ
*************************************************************/
unsigned char writedata_RingBuff(unsigned int wanttowrite,unsigned char *buff)
{
	unsigned int i;
  if(wanttowrite>(5120-truedata)) //����д������ݸ��������ڻ������ڿ�������������0
	{
	return 0;    //������ʣ��ռ䲻��
	}
  else
  {
  for(i=0;i<wanttowrite;i++)
		{
		  *Wprb = buff[i];
			Wprb++;
			truedata++;
			if(Wprb == RingBuff+5119)
			{
			Wprb = RingBuff;
			}
		}
  return 1;//д��������޹���
	}
}

unsigned char readdata_RingBuff(unsigned int wanttoread,unsigned char *buff)
{
  unsigned int i;
  if(wanttoread>truedata)//������ȡ�����ݸ��������ڻ���������Ч���ݸ���������0
  {
    return 0;
	}
	else
	{
		  for(i=0;i<wanttoread;i++)
			{
		  buff[i] = *Rprb;
		  Rprb++;
		  truedata--;
			if(Rprb == RingBuff + 5119)
			   {
			     Rprb = RingBuff;
			   }
	     }
  }
}