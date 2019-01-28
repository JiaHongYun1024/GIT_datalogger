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

/**********************************************************************/

/************ȫ�ֱ�������**********************************************/
#define RX_BUF_MAX_SIZE     2048        //

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

u8 RTCtime[5];



UINT *add;
DWORD fre_clust, fre_sect, tot_sect;//�����������������������������
FATFS *ft;
DWORD TEMP;
ptrdiff_t diff;
uint16_t truecount;
u8 readfinish=0;

/************************************************************************/
void writeSDCard(void);
u8 set_time(void);
u8 set_boud(void);


int main(void)
{        
	UINT i;
// 	u32 total,free;
//	u8 t=0;	
	FRESULT res=0;	

   
     	/* init  */
	               MY_init();
                 check_SD();	
                 mount_fatfs();
	               set_time();
	               //set_boud();
                 create_a_file();
		             TIM2_Int_Init(20000-1,16800-1);	//4s��ʱ����
			           LED1=0;//�̵�������ʼ�����


	  /*           */
while(1)
	{		
		if(DMAITFlag) //һ֡���ݽ������
		{     LED0 = 0;
				  TIM2->CNT=0;//����������
			    TIM_Cmd(TIM2,ENABLE); //ʹ�ܶ�ʱ��2 4s timeout �ж�
			    DMAITFlag = 0;
					MY_br = 2048;
					truecount =	xl_ringbuffer_get(&uart1RxRingBuf,buffer,MY_br);   
					writeSDCard();	
					LED0=1;
		}
	} //while��1��         /*    version:test  1.0     */
}//main









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
/*��ȡ����
��������read_setup*/
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

u8 set_boud(void)
{
	UINT Bbr;
	FRESULT res;
	u32 read_boud;
	char buf[6];

		res = f_open(&fl,"boud.txt",FA_OPEN_EXISTING | FA_WRITE |FA_READ);
  if(!res)    //���ļ��ɹ�
	 {
	res = f_read(&fl,&buf,sizeof(buf),&Bbr);
      if(res) 
      {
	printf("��ȡ������ʧ��");
	return 1;
      }
	 res = f_close(&fl);//�ر��ļ�
	 read_boud = atoi(buf);
	 printf("�������޸ĳɹ�");
	 f_unlink("boud.txt");
	 USART1_Init(read_boud);
	 return 0;
	 }
	 else
	 {
	 printf("�򿪲������ļ�ʧ��");
	return 1;
	 }
 }