#include "RingQueue.h"

#define RX_BUF_MAX_SIZE     2048        //
#define RECEIVE_BUF_SIZE    2048	
#define SDCARD_NOT_ENOUGH 0
#define UINT unsigned int 


typedef struct {
 unsigned char year;
 unsigned char month;
 unsigned char day;
 unsigned char hours;
 unsigned char	min;
}time;





/*****************º¯ÊýÉùÃ÷**********************************************************/
void MY_init(void);
void check_SD(void);
UINT MAX(void);
void mount_fatfs(void);
void create_a_file(void);
void USART1_Init(u32 boud);
