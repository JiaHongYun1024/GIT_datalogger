/*********************ͷ�ļ�����************************************/
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


/*�޸�˵����      
           1�����뽻��ʽ����RTCʱ�䣬�û���ͨ��
					 2�����������
*/
/*#define*/
#define RX_BUF_MAX_SIZE     2048        //
/*�����ⲿ����*/
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
extern u16 USART_RX_STA;       //����״̬���	
extern struct xl_ringbuffer uart1RxRingBuf;
extern u8 DMAITFlag;
/*ȫ�ֱ�������*/
u8 RTCtime[5];
DWORD fre_clust, fre_sect, tot_sect;//�����������������������������
FATFS *ft;
DWORD TEMP;
uint16_t truecount;
u8 readfinish=0;

/*��������************************************************************************/
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
		             TIM2_Int_Init(20000-1,16800-1);	//4s��ʱ����
	               upload_space();//������⣬�ϱ�
                 LED_shansuo();
/*MAIN
	    */
while(1)
	{		
		if(DMAITFlag) //һ֡���ݽ������
		{     LED0=0;
				  TIM2->CNT=0;//����������
			    TIM_Cmd(TIM2,ENABLE); //ʹ�ܶ�ʱ��2 4s timeout �ж�
			    DMAITFlag = 0;
					MY_br = 2048;
					truecount =	xl_ringbuffer_get(&uart1RxRingBuf,buffer,MY_br);   
					writeSDCard();	
					LED0=1;
		}
	} //while��1��         /*    version:test  1.1     */
}//main



/*дSD������
��������writeSDCard
���������void
����ֵ��  void
���ߣ�    ������
*/
void writeSDCard(void)
{	
	FRESULT res=0;	
 				 /*дSD��*/ 
	    res = f_open(&fl,get_time_filename,FA_OPEN_EXISTING | FA_WRITE |FA_READ);
	    res = f_lseek(&fl,f_size(&fl));//��ָ���Ƶ��ļ�ĩβ
      res = f_write(&fl,buffer,MY_br,&MY_bw);
//             if (res || MY_bw < MY_br) break; // ������ߴ������� 			 
//				 count ++;
//	                    if(count == 10)	 
//				    {
//                        count = 0;
//												res = f_sync(&fl);//����ˢ�»�����
//	  TEMP = f_tell(&fl);//��ȡ��ǰ д ָ��
//	          f_getfree("0:",&fre_clust,&ft);//��ȡȫ���������Ϳ���������
//            tot_sect =(ft->n_fatent -2) * ft->csize; //total������
//            fre_sect = fre_clust * ft->csize;//free������
//					  } 
		  res = f_close(&fl);//�ر��ļ� 
//								       sdfinish = 0;     		//����д		 
}
/*����ʱ�亯��*************************************************************************
��������set_time
���������void
����ֵ��  void
���ߣ�    ������
**************************************************************************************/
u8 set_time(void)
{
  UINT Bbr;
	FRESULT res;
	u32 read_boud;
	char buf[15];
	ErrorStatus sta;
 res = f_open(&fl,"time.txt",FA_OPEN_EXISTING | FA_WRITE |FA_READ);
if(!res) //�򿪳ɹ�
  {
  	res = f_read(&fl,buf,sizeof(buf),&Bbr); 
		  if(res) 
      {
	       printf("��ȡ������ʧ��");
	       return 1;
			}		
		RTCtime[0] = atoi(buf);
		RTCtime[1] = atoi(&buf[3]);
		RTCtime[2] = atoi(&buf[6]);
		RTCtime[3] = atoi(&buf[9]);
		RTCtime[4] = atoi(&buf[12]);		
		res = f_close(&fl);//�ر��ļ�
		sta = RTC_Set_Date(RTCtime[0],RTCtime[1],RTCtime[2],1);
		sta = RTC_Set_Time(RTCtime[3],RTCtime[4],0,RTC_H12_AM);
		printf("����ʱ��ɹ�");
		f_unlink("time.txt"); //���ú�ɾ�������ļ�
		return 0;
  }
	else
  {
		printf("��ʱ���ļ�ʧ��");
		return 1;
	}
}
/*�ϱ�����
��������upload_space
���������void
����ֵ��  void
���ߣ�    ������
*/
void upload_space(void)
{
 printf("��������%d",(int)tot_sect);
 printf("��ǰʣ��������%d",(int)fre_sect);
if(fre_sect<=tot_sect/10)//ʣ��ռ�С�ڰٷ�֮10
 printf("ʣ��ռ�С�ڰٷ�֮10");
while(1)
 {
  LED_shansuo();
	delay_ms(2000);
	LED0 = ~LED0;
 }
}
/*LED
��������LED_shansuo
���������void
����ֵ��  void
���ߣ�    ������
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
