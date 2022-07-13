#include "gpio.h"
#include "delay.h"
// IO初始化
void Usr_Gpio_Init(void)
{ 

    // RED
    GPIO_InitTypeDef GPIO_InitStructure;
 	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 // 使能PB端口时钟
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;				 // RED PB9 端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 // IO口速度为50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);					 
    GPIO_ResetBits(GPIOB,GPIO_Pin_9);						 // 设置初始电平为低

    // KEY
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				 // KEY PB8 端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		     // 上拉输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}


u8 Key_Scan(u8 mode)
{	 
	static u8 key_up=1;             //按键按松开标志
	if(mode)
		key_up=1;                   //支持连按
	
	if(key_up && KEY0==0){
		delay_ms(10);//去抖动 
		key_up=0;
		if(KEY0==0)
		    return 1;
    }else if(KEY0 == 1){
        key_up=1;
    }
 	return 0;// 无按键按下
}

