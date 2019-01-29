/*********************头文件包含************************************/
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "stmflash.h" 
#include "key.h"  
#include "sram.h"   
#include "malloc.h" 
#include "sdio_sdcard.h"    
#include "malloc.h" 
#include "w25qxx.h"    
#include "ff.h"  
#include "exfuns.h"  
#include "stdio.h"
#include "stddef.h"
#include "init_xiaojia.h"
#include "ringbuffer.h"
#include <typedef.h>
#include "timer.h"
#include "rtc.h"
#include "stdlib.h"


/*修改说明：      
           1、加入交互式设置RTC时间，用户可通过
					 2、整理代码格局
*/
/*#define*/
#define RX_BUF_MAX_SIZE     2048        //
/*引用外部变量*/
extern u8 ReceiveBuff[RECEIVE_BUF_SIZE];
extern u8 RemainDataCount;
extern u8 ReceivedData[RECEIVE_BUF_SIZE];
extern u8 RemainDataCount;
extern unsigned char buffer[2048];
extern char get_time_filename[20];
extern UINT MY_br,MY_bw;
extern FIL fl;
extern unsigned char err;
extern FILINFO flinfo;
extern DIR dy; 
extern u8 dataget;
extern unsigned int truedata;
extern u16 USART_RX_STA;       //接收状态标记	
extern struct xl_ringbuffer uart1RxRingBuf;
extern u8 DMAITFlag;
/*全局变量定义*/
u8 RTCtime[5];
DWORD fre_clust, fre_sect, tot_sect;//空余簇数，空余扇区数，总扇区数
FATFS *ft;
DWORD TEMP;
uint16_t truecount;
u8 readfinish=0;

/*函数声明************************************************************************/
void writeSDCard(void);
u8 set_time(void);
void upload_space(void);
void LED_shansuo(void);
/********************************************************************************/


int main(void)
{        
	UINT i;
	FRESULT res=0;	 
	
     	/* init  */
	
	               MY_init();  
		             check_SD();
                 mount_fatfs();	
	               set_time();
                 create_a_file();
		             TIM2_Int_Init(20000-1,16800-1);	//4s定时周期
	               upload_space();//容量检测，上报
                 LED_shansuo();
/*MAIN
	    */
while(1)
	{		
		if(DMAITFlag) //一帧数据接收完毕
		{     LED0=0;
				  TIM2->CNT=0;//计数器清零
			    TIM_Cmd(TIM2,ENABLE); //使能定时器2 4s timeout 判断
			    DMAITFlag = 0;
					MY_br = 2048;
					truecount =	xl_ringbuffer_get(&uart1RxRingBuf,buffer,MY_br);   
					writeSDCard();	
					LED0=1;
		}
	} //while（1）         /*    version:test  1.1     */
}//main



/*写SD卡函数
函数名：writeSDCard
传入参数：void
返回值：  void
作者：    贾泓运
*/
void writeSDCard(void)
{	
	FRESULT res=0;	
 				 /*写SD卡*/ 
	    res = f_open(&fl,get_time_filename,FA_OPEN_EXISTING | FA_WRITE |FA_READ);
	    res = f_lseek(&fl,f_size(&fl));//将指针移到文件末尾
      res = f_write(&fl,buffer,MY_br,&MY_bw);
//             if (res || MY_bw < MY_br) break; // 错误或者磁盘已满 			 
//				 count ++;
//	                    if(count == 10)	 
//				    {
//                        count = 0;
//												res = f_sync(&fl);//定期刷新缓冲区
//	  TEMP = f_tell(&fl);//获取当前 写 指针
//	          f_getfree("0:",&fre_clust,&ft);//获取全部扇区数和空余扇区数
//            tot_sect =(ft->n_fatent -2) * ft->csize; //total扇区数
//            fre_sect = fre_clust * ft->csize;//free扇区数
//					  } 
		  res = f_close(&fl);//关闭文件 
//								       sdfinish = 0;     		//正在写		 
}
/*设置时间函数*************************************************************************
函数名：set_time
传入参数：void
返回值：  void
作者：    贾泓运
**************************************************************************************/
u8 set_time(void)
{
  UINT Bbr;
	FRESULT res;
	u32 read_boud;
	char buf[15];
	ErrorStatus sta;
 res = f_open(&fl,"time.txt",FA_OPEN_EXISTING | FA_WRITE |FA_READ);
if(!res) //打开成功
  {
  	res = f_read(&fl,buf,sizeof(buf),&Bbr); 
		  if(res) 
      {
	       printf("读取过程中失败");
	       return 1;
			}		
		RTCtime[0] = atoi(buf);
		RTCtime[1] = atoi(&buf[3]);
		RTCtime[2] = atoi(&buf[6]);
		RTCtime[3] = atoi(&buf[9]);
		RTCtime[4] = atoi(&buf[12]);		
		res = f_close(&fl);//关闭文件
		sta = RTC_Set_Date(RTCtime[0],RTCtime[1],RTCtime[2],1);
		sta = RTC_Set_Time(RTCtime[3],RTCtime[4],0,RTC_H12_AM);
		printf("设置时间成功");
		f_unlink("time.txt"); //设置后，删除设置文件
		return 0;
  }
	else
  {
		printf("打开时间文件失败");
		return 1;
	}
}
/*上报容量
函数名：upload_space
传入参数：void
返回值：  void
作者：    贾泓运
*/
void upload_space(void)
{
 printf("总容量：%d",(int)tot_sect);
 printf("当前剩余容量：%d",(int)fre_sect);
if(fre_sect<=tot_sect/10)//剩余空间小于百分之10
 printf("剩余空间小于百分之10");
while(1)
 {
  LED_shansuo();
	delay_ms(2000);
	LED0 = ~LED0;
 }
}
/*LED
函数名：LED_shansuo
传入参数：void
返回值：  void
作者：    贾泓运
*/
void LED_shansuo(void)
{
  LED0 = 1;
	delay_ms(100);
	LED0 = 0;
	delay_ms(100);
  LED0 = 1;
	delay_ms(100);
	LED0 = 0;
}
