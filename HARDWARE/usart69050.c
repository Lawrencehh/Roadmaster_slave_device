#include "usart69050.h"	
#include "sys.h"	
#include "packet.h"
#include "imu_data_decode.h"

void USART6_IRQHandler(void)
{
	if(USART_GetITStatus(USART6, USART_IT_RXNE) != RESET)//���յ�����
	{	 
		uint8_t tmp = USART_ReceiveData(USART6);
		Packet_Decode(tmp);	
	}
	USART_ClearFlag(USART6, USART_FLAG_RXNE);  
	USART_ClearITPendingBit(USART6, USART_IT_RXNE);
}



void USART6_init(void)
	{
  //GPIO�˿�����
	
	USART_InitTypeDef USART_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);
	
	
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource9,GPIO_AF_USART6); 
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource14,GPIO_AF_USART6); 
	
	
	//USART2_TX   GPIOA2
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_14; //PA2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	//�����������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOG, &GPIO_InitStructure);


  //Usart2 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;    ////////////////
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		
	NVIC_Init(&NVIC_InitStructure);	

	//USART ��ʼ������
	USART_DeInit(USART6);
	USART_InitStructure.USART_BaudRate = 115200;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_Init(USART6,&USART_InitStructure);

  USART_Init(USART6, &USART_InitStructure);     //��ʼ������2
  USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART6, ENABLE);                    //ʹ�ܴ���2 
	
  imu_data_decode_init();
	
}






