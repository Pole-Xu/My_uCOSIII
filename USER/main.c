#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "includes.h"
#include "os_app_hooks.h"
#include <string.h>
#include "gpio.h"


#define START_TASK_PRIO		3                   //�������ȼ�
#define START_STK_SIZE 		512                 //�����ջ��С
OS_TCB StartTaskTCB;                            //������ƿ�
CPU_STK START_TASK_STK[START_STK_SIZE];         //�����ջ
void start_task(void *p_arg);                   //������


#define HIGH_TASK_PRIO		4
#define HIGH_STK_SIZE 		128
OS_TCB HighTaskTCB;
CPU_STK HIGH_TASK_STK[HIGH_STK_SIZE];
void high_task(void *p_arg);

#define LOW_TASK_PRIO		5
#define LOW_STK_SIZE 		128
OS_TCB LowTaskTCB;
CPU_STK LOW_TASK_STK[LOW_STK_SIZE];
void low_task(void *p_arg);

#define LED_TASK_PRIO		6
#define LED_STK_SIZE 		128
OS_TCB LedTaskTCB;
CPU_STK LED_TASK_STK[LED_STK_SIZE];
void led_task(void *p_arg);


OS_SEM gSEM;
OS_TMR gTMR;

void usr_my_init(void){
    OS_ERR err;
    
    delay_init();       // ��ʱ��ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // �жϷ�������
	uart_init(115200);    // ���ڲ���������
	LED_Init();         // LED��ʼ��
	Usr_Gpio_Init();    // Usr Init
	OSInit(&err);		// ��ʼ��UCOSIII
}

int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	
    usr_my_init();   // usr init 
	
	OS_CRITICAL_ENTER();//�����ٽ���
	//������ʼ����
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//������ƿ�
				 (CPU_CHAR	* )"start task", 		//��������
                 (OS_TASK_PTR )start_task, 			//������
                 (void		* )0,					//���ݸ��������Ĳ���
                 (OS_PRIO	  )START_TASK_PRIO,     //�������ȼ�
                 (CPU_STK   * )&START_TASK_STK[0],	//�����ջ����ַ
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//�����ջ�����λ
                 (CPU_STK_SIZE)START_STK_SIZE,		//�����ջ��С
                 (OS_MSG_QTY  )0,					//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK	  )0,					//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                 (void   	* )0,					//�û�����Ĵ洢��
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //����ѡ��
                 (OS_ERR 	* )&err);				//��Ÿú�������ʱ�ķ���ֵ
	OS_CRITICAL_EXIT();	//�˳��ٽ���	 
	OSStart(&err);  //����UCOSIII
	while(1);
}

// Tmr CallBack
void gTMR_callback(void *p_tmr, void *p_arg)
{
	static int cnt=0;
	cnt++;		//��ʱ��1ִ�д�����1
	printf("10s to go! cnt = %d.\r\n",cnt);
}

// Start Func
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//ͳ������                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//���ʹ���˲����жϹر�ʱ��
    CPU_IntDisMeasMaxCurReset();	
#endif
	
#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //��ʹ��ʱ��Ƭ��ת��ʱ��
	//ʹ��ʱ��Ƭ��ת���ȹ���,ʱ��Ƭ����Ϊ1��ϵͳʱ�ӽ��ģ���1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		

#if OS_CFG_APP_HOOKS_EN				//ʹ�ù��Ӻ���
	//App_OS_SetAllHooks();			
#endif

   //�����ź���
    OSSemCreate ((OS_SEM   *)&gSEM,
                 (CPU_CHAR *)"gSEM",
                 (OS_SEM_CTR)0,
                 (OS_ERR   *)&err);
                 
	//������ʱ��1
	OSTmrCreate((OS_TMR		*)&gTMR,		//��ʱ��1
                (CPU_CHAR	*)"TMR",		//��ʱ������
                (OS_TICK	 )0,			
                (OS_TICK	 )1000,          //100*10=1000ms
                (OS_OPT		 )OS_OPT_TMR_PERIODIC, //����ģʽ
                (OS_TMR_CALLBACK_PTR)gTMR_callback,//�ص�����
                (void	    *)0,			//����Ϊ0
                (OS_ERR	    *)&err);		//���صĴ�����

	OS_CRITICAL_ENTER();	//�����ٽ���
	
	//����LED����
	OSTaskCreate((OS_TCB 	* )&LedTaskTCB,		
				 (CPU_CHAR	* )"Led Task", 		
                 (OS_TASK_PTR )led_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )LED_TASK_PRIO,     
                 (CPU_STK   * )&LED_TASK_STK[0],	
                 (CPU_STK_SIZE)LED_STK_SIZE/10,	
                 (CPU_STK_SIZE)LED_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);				
				 
	//����High����
	OSTaskCreate((OS_TCB 	* )&HighTaskTCB,		
				 (CPU_CHAR	* )"High Task", 		
                 (OS_TASK_PTR )high_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )HIGH_TASK_PRIO,     	
                 (CPU_STK   * )&HIGH_TASK_STK[0],	
                 (CPU_STK_SIZE)HIGH_STK_SIZE/10,	
                 (CPU_STK_SIZE)HIGH_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);	

    //����Low����
	OSTaskCreate((OS_TCB 	* )&LowTaskTCB,		
				 (CPU_CHAR	* )"Low Task", 		
                 (OS_TASK_PTR )low_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )LOW_TASK_PRIO,     	
                 (CPU_STK   * )&LOW_TASK_STK[0],	
                 (CPU_STK_SIZE)LOW_STK_SIZE/10,	
                 (CPU_STK_SIZE)LOW_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);
			 								 
	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//����ʼ����			 
	OS_CRITICAL_EXIT();	//�����ٽ���
}


// LED Func
void led_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;

	while(1)
	{   
	    printf("This is LED TASK!\r\n");
        LED0 = ~LED0;
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ1s
	}
}

// High Func
void high_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;

	while(1)
	{
	    printf("This is HIGH TASK!\r\n");
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ1s
	}
}

// Low Func
void low_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
    OSTmrStart(&gTMR,&err);
	while(1)
	{
	    
	    if(Key_Scan(0) == 1)
	        RED = ~RED;
	    printf("This is Low TASK!\r\n");
		//OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ1s
	}
}

