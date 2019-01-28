#include "sys.h"
#include "string.h"
#include "usart.h"



extern u8 dataget;

u8 t;
u8 len;	
u8 powerdata[256];
u8 dianliang[2];
/**********************************************************************************
*	������void array_copy(char *array_a,char *array_b,char len)
*
*	���ܣ�������B��Ԫ�ظ�ֵ��A,˳����
*
*	������@array_a������a
*		  @array_b������b
*		  @len����
*
*	���أ�void
*********************************************************************************/
static void array_copy(u8 *array_a,u8 *array_b,u8 len)
{
	u8 i;
	u8 j=len-1;
	for(i=0;i<len;i++)
	{
		array_a[i]=array_b[j-i];
	}
}

/*get data*/ 
/*
powerdata[]       0~3         4~7     8~9��u16��    9~12
            ������ܵ���     �ܵ�ѹ   ����        ��һ�ڵ�ѹ  ... ... n = 54

*/
void get_data(u8 *destiny,u8 *src)
{
u8 *pdptr,*rxptr;
u8 buf[4];
float fpower[8];
u16 free_power;
	pdptr=destiny;
	rxptr = src;
  memcpy(pdptr,rxptr,4);//������ܵ���
	fpower[0] = *(float *)pdptr;
	printf("������ܵ���Ϊ��%f A",fpower[0]);
	pdptr = pdptr+4;
	rxptr = rxptr+4;
	array_copy(pdptr,rxptr,4);//������ܵ�ѹ
	fpower[1] = *(float *)pdptr;
	printf("������ܵ�ѹΪ��%f V",fpower[1]);
	pdptr = pdptr+4;
	rxptr = rxptr+4;
	memcpy(buf,rxptr,2);//ʣ���ܵ���
	printf("ʣ�������%d%\r\n",buf[0]*256+buf[1]);
//	pdptr = pdptr+4;
//	rxptr = rxptr+6;
//	memcpy(pdptr,rxptr,4); //��һ�ڵ�ص�ѹ
	/*
	.
	. 
	.
	*/
}


/*data analize*/
u16 find_datapackhead(u8 l)
{
u16 i;
for(i=0;i<l;i++)
	{
	 if(USART_RX_BUF[i] == 0x06&&USART_RX_BUF[i-1] == 0x82) 
	 return (i+1);
	}
}

void power_API(void)
{
		if(dataget) // ��̨��һ֡���ݽ������
		{
		dataget = 0;
		len = USART_RX_STA; //��ȡ���ճ���
		USART_RX_STA = 0;
		t = find_datapackhead(len);
		get_data(powerdata,&USART_RX_BUF[t]);
		}
}
