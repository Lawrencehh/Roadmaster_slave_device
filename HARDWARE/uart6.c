#include "task.h"
#include "delay.h"
#include "uart6.h"
#include "gpio.h"
#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4xx_usart.h"
#include "SCServo.h" // ���ش��ڶ���ӿ�

//////////////////////////////////////////////////////////////////////////
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_UCOS
#include "includes.h"					//ucos ʹ��	  
#endif


//UART �����ݻ�����
__IO uint8_t uart6Buf[128];
__IO int head = 0;
__IO int tail  = 0;

static uint8_t rx_buffer[256]; // ���ջ�����
static uint16_t rx_index = 0;  // ���ջ���������
static uint8_t header_count = 0; // ���ڼ��0xFFFF��ͷ

// �й�STS3032�ķ�������
uint16_t STS3032_Pos;
int STS3032_Speed;
int STS3032_Load;
int STS3032_Voltage;
int STS3032_Temper;
int STS3032_Move;
int STS3032_Current;

void Uart6_Flush(void)
{
	head = tail = 0;
}

int16_t Uart6_Read(void)
{
	if(head!=tail){
		uint8_t Data = uart6Buf[head];
		head =  (head+1)%128;
		return Data;
	}else{
		return -1;
	}
}

void USART6_Init(void) 
{                
		USART_InitTypeDef usart;
    GPIO_InitTypeDef  gpio;
	  NVIC_InitTypeDef  nvic;
	
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);
 
		GPIO_PinAFConfig(GPIOG,GPIO_PinSource9,GPIO_AF_USART6); 
		GPIO_PinAFConfig(GPIOG,GPIO_PinSource14,GPIO_AF_USART6); 
	
		gpio.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_14; 
		gpio.GPIO_Mode = GPIO_Mode_AF;
		gpio.GPIO_Speed = GPIO_Speed_50MHz;	
		gpio.GPIO_OType = GPIO_OType_PP; 
		gpio.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(GPIOG,&gpio); 

		usart.USART_BaudRate = 1000000;//����������
		usart.USART_WordLength = USART_WordLength_8b;
		usart.USART_StopBits = USART_StopBits_1;
		usart.USART_Parity = USART_Parity_No;
		usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		usart.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;	
		USART_Init(USART6, &usart); 
		
		nvic.NVIC_IRQChannel = USART6_IRQn;
		nvic.NVIC_IRQChannelPreemptionPriority=2;
		nvic.NVIC_IRQChannelSubPriority =2;		
		nvic.NVIC_IRQChannelCmd = ENABLE;			
		NVIC_Init(&nvic);	 
		
		USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);
		USART_Cmd(USART6,ENABLE);
}


//u8 Receive_6;

void USART6_IRQHandler(void)
{  				
		if(USART_GetFlagStatus(USART6,USART_FLAG_ORE)!=RESET)
		{
		(void)USART6->SR;   
		(void)USART6->DR;
		return;
		}
		if (USART_GetFlagStatus(USART6, USART_FLAG_ORE) != RESET)//��ֹ��������̫�쵼�����
		{  	
		//����жϱ�־    		 
		(void)USART6->SR;   
		(void)USART6->DR;
		return;			
		}
    if (USART_GetITStatus(USART6, USART_IT_RXNE)) {
        uint8_t byte = USART_ReceiveData(USART6); // ��ȡ���յ����ֽ�

        // ������ݰ���ͷ0xFFFF
        if (byte == 0xFF && header_count == 0) {
            header_count++;
        } else if (byte == 0xFF && header_count == 1) {
            header_count = 0;
            rx_index = 0;
            rx_buffer[rx_index++] = 0xFF;
            rx_buffer[rx_index++] = 0xFF;
        } else if (rx_index > 0) {
            rx_buffer[rx_index++] = byte;

            // �����յ�Ԥ���������ֽں�
            if (rx_index == 8) { // sts3032����Ļش���Ϣ
                // ������յ������ݵ�checksumУ����
                uint8_t calculated_checksum = ~(rx_buffer[2]+rx_buffer[3]+rx_buffer[4]+rx_buffer[5]+rx_buffer[6]); // ������checksum��֡ͷ�ֶ�

                // ��ȡ���յ���checksumУ����
                uint8_t received_checksum = rx_buffer[rx_index - 1];

                // �Ա�У����
                if (calculated_checksum == received_checksum) {
										STS3032_Pos =  (rx_buffer[5] << 8) | rx_buffer[6];
										
                } else {
                    // У��ʧ�ܣ�������Ҫ������
                }

                // ���ý��ջ���������
                rx_index = 0;
            }
        }
    }

    // ��������жϱ�־
		USART_ClearFlag(USART6, USART_FLAG_RXNE);  
		USART_ClearITPendingBit(USART6, USART_IT_RXNE);   
//		OSIntExit(); 
}

//void USART6_Handler(void)
//{
//	
//	  USART_ClearFlag(USART6,USART_FLAG_TC);
//		USART_SendData(USART6,0x64);
//		while(USART_GetFlagStatus(USART6,USART_FLAG_TXE)==RESET);//�ж��Ƿ������

//}

void Uart6_Send(uint8_t *buf , uint8_t len)
{
	uint8_t i=0;
	for(i=0; i<len; i++){
		USART_SendData(USART6, buf[i]);
		while(USART_GetFlagStatus(USART6, USART_FLAG_TXE) == RESET);
	}
	while(USART_GetFlagStatus(USART6, USART_FLAG_TC) == RESET);
}









