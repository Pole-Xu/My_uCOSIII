#include "gpio.h"
#include "delay.h"
// IO��ʼ��
void Usr_Gpio_Init(void)
{ 

    // RED
    GPIO_InitTypeDef GPIO_InitStructure;
 	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 // ʹ��PB�˿�ʱ��
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;				 // RED PB9 �˿�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 // �������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 // IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);					 
    GPIO_ResetBits(GPIOB,GPIO_Pin_9);						 // ���ó�ʼ��ƽΪ��

    // KEY
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				 // KEY PB8 �˿�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		     // ��������
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}


u8 Key_Scan(u8 mode)
{	 
	static u8 key_up=1;             //�������ɿ���־
	if(mode)
		key_up=1;                   //֧������
	
	if(key_up && KEY0==0){
		delay_ms(10);//ȥ���� 
		key_up=0;
		if(KEY0==0)
		    return 1;
    }else if(KEY0 == 1){
        key_up=1;
    }
 	return 0;// �ް�������
}

