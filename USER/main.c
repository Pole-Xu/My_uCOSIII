#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "includes.h"
#include "os_app_hooks.h"



#define START_TASK_PRIO		3                   //任务优先级
#define START_STK_SIZE 		512                 //任务堆栈大小
OS_TCB StartTaskTCB;                            //任务控制块
CPU_STK START_TASK_STK[START_STK_SIZE];         //任务堆栈
void start_task(void *p_arg);                   //任务函数


#define LED0_TASK_PRIO		4
#define LED0_STK_SIZE 		128
OS_TCB Led0TaskTCB;
CPU_STK LED0_TASK_STK[LED0_STK_SIZE];
void led0_task(void *p_arg);


#define LED1_TASK_PRIO		5
#define LED1_STK_SIZE 		128
OS_TCB Led1TaskTCB;
CPU_STK LED1_TASK_STK[LED1_STK_SIZE];
void led1_task(void *p_arg);


int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	
	delay_init();       //延时初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //中断分组配置
	uart_init(115200);    //串口波特率设置
	LED_Init();         //LED初始化
	
	OSInit(&err);		//初始化UCOSIII
	OS_CRITICAL_ENTER();//进入临界区
	//创建开始任务
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//任务控制块
				 (CPU_CHAR	* )"start task", 		//任务名字
                 (OS_TASK_PTR )start_task, 			//任务函数
                 (void		* )0,					//传递给任务函数的参数
                 (OS_PRIO	  )START_TASK_PRIO,     //任务优先级
                 (CPU_STK   * )&START_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//任务堆栈深度限位
                 (CPU_STK_SIZE)START_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	  )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	* )0,					//用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //任务选项
                 (OS_ERR 	* )&err);				//存放该函数错误时的返回值
	OS_CRITICAL_EXIT();	//退出临界区	 
	OSStart(&err);  //开启UCOSIII
	while(1);
}

//开始任务函数
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//统计任务                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//如果使能了测量中断关闭时间
    CPU_IntDisMeasMaxCurReset();	
#endif
	
#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //当使用时间片轮转的时候
	 //使能时间片轮转调度功能,时间片长度为1个系统时钟节拍，既1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		

#if OS_CFG_APP_HOOKS_EN				//使用钩子函数
	App_OS_SetAllHooks();			
#endif

	OS_CRITICAL_ENTER();	//进入临界区
	//创建LED0任务
	OSTaskCreate((OS_TCB 	* )&Led0TaskTCB,		
				 (CPU_CHAR	* )"led0 task", 		
                 (OS_TASK_PTR )led0_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )LED0_TASK_PRIO,     
                 (CPU_STK   * )&LED0_TASK_STK[0],	
                 (CPU_STK_SIZE)LED0_STK_SIZE/10,	
                 (CPU_STK_SIZE)LED0_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);				
				 
	//创建LED1任务
	OSTaskCreate((OS_TCB 	* )&Led1TaskTCB,		
				 (CPU_CHAR	* )"led1 task", 		
                 (OS_TASK_PTR )led1_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )LED1_TASK_PRIO,     	
                 (CPU_STK   * )&LED1_TASK_STK[0],	
                 (CPU_STK_SIZE)LED1_STK_SIZE/10,	
                 (CPU_STK_SIZE)LED1_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);	
			 								 
	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//挂起开始任务			 
	OS_CRITICAL_EXIT();	//进入临界区
}

//led0任务函数
void led0_task(void *p_arg)
{
    unsigned int cnt = 0;
	OS_ERR err;
	p_arg = p_arg;
	
	while(1)
	{
	    
		LED0=0;
		OSTimeDlyHMSM(0,0,0,300,OS_OPT_TIME_HMSM_STRICT,&err); //延时300ms
		LED0=1;
		OSTimeDlyHMSM(0,0,0,700,OS_OPT_TIME_HMSM_STRICT,&err); //延时700ms
		if(cnt == 20){
		    OSTaskSuspend(&Led1TaskTCB, &err);
		    printf(" LED1 Task has been pend!\r\n");
		    cnt++;
		}else{
		    if(cnt == 40){
                OSTaskResume(&Led1TaskTCB, &err);
                //printf(" LED1 Task has been Resume!\r\n");
		    }
            cnt++;
		}
		printf("This is led0 task, cnt = %d\r\n",cnt);
	}
}

//led1任务函数
void led1_task(void *p_arg)
{
    unsigned int cnt = 0;
	OS_ERR err;
	p_arg = p_arg;
	
	while(1)
	{   cnt++;
		//LED1=~LED1;
		printf("This is led1 task, cnt = %d\r\n",cnt);
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时1s
	}
}
