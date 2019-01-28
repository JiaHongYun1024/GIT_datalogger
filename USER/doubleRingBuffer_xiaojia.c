#include "RingQueue.h"

 
 

/**********************************************************
文件名称：环形缓冲区.c
作者：贾泓运
***********************************************************/
  unsigned char RingBuff[5120];//10k的环形缓冲区
	unsigned char *Wprb,*Rprb;
	unsigned int truedata;
/****************************************************
函数名称：环形缓冲区初始化
返回值：无
入口参数：无
功能：初始化环形缓冲区
****************************************************/

void MyRingBuff_init(void)
{
	Rprb = RingBuff;
	Wprb = RingBuff;
	truedata = 0;
}

/*************************************************************
函数名称：写环形缓冲区
返回值:0 缓冲区已满 （检测到0后，将数据全部读出写进SD卡
入口参数：即将要写入缓冲区的数据量wanttowrite,数据源地址
*************************************************************/
unsigned char writedata_RingBuff(unsigned int wanttowrite,unsigned char *buff)
{
	unsigned int i;
  if(wanttowrite>(5120-truedata)) //即将写入的数据个数若大于缓冲区内空闲容量，返回0
	{
	return 0;    //缓冲区剩余空间不足
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
  return 1;//写入完成且无故障
	}
}

unsigned char readdata_RingBuff(unsigned int wanttoread,unsigned char *buff)
{
  unsigned int i;
  if(wanttoread>truedata)//即将读取的数据个数若大于缓冲区内有效数据个数，返回0
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