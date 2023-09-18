#include "uart1.h"
#include "includes.h"
#include "can1.h"	
#include "task.h"	
#include "Judge_System.h"

u8 sbus_rx_buffer[2][20]={0};//˫��������
Tel_Ctrl_t TelCtrlData;
int16_t Vy,Vx,Vw,Vy_mid,Vx_mid;     //���ٶȻ���󷢸����    �����м�ֵ
float Vy_keyboard,Vx_keyboard;     //�����ٶ�
int16_t Vy_rocker,Vx_rocker;         //ң���ٶ�
int32_t keyboard;                    //����ֵ
float V_mousex=0,V_mousey=0;           //����ٶ�
int flag_bo=0;
float mouse_x[2],mouse_y[2];
float avemousex,avemousey; 
 
int16_t V_max=300,V_min=95,V_mid=180;   
float acceleration_min=0.75,acceleration_mid=1.5,acceleration_max=2;
float acceleration_slowdown=20;   //ɲ�����ٶ�

void TEL_USART1_Init(void)
{                	
	  USART_InitTypeDef usart;
    GPIO_InitTypeDef  gpio;
		NVIC_InitTypeDef  nvic;  
		DMA_InitTypeDef   dma;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_DMA2,ENABLE);//DMA˫AHB�����߹��ܣ�һ�����ڴ洢�����ʣ�һ�������������
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);

    GPIO_PinAFConfig(GPIOB,GPIO_PinSource7 ,GPIO_AF_USART1);//��������

    gpio.GPIO_Pin = GPIO_Pin_7 ;//��������
    gpio.GPIO_Mode = GPIO_Mode_AF;
		gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_Speed = GPIO_Speed_100MHz;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB,&gpio);
		
    USART_DeInit(USART1);
    usart.USART_BaudRate = 100000;                //SBUS 100K baudrate
    usart.USART_WordLength = USART_WordLength_8b;//�ֳ�8B
    usart.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
    usart.USART_Parity = USART_Parity_Even;//��żУ��λ
    usart.USART_Mode = USART_Mode_Rx;//����ģʽ  ����ģʽ
    usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART1,&usart);
		
		nvic.NVIC_IRQChannel = USART1_IRQn;
		nvic.NVIC_IRQChannelPreemptionPriority=0;
		nvic.NVIC_IRQChannelSubPriority =0;		
		nvic.NVIC_IRQChannelCmd = ENABLE;			
		NVIC_Init(&nvic);
	
    DMA_DeInit(DMA2_Stream2);
    dma.DMA_Channel= DMA_Channel_4;
    dma.DMA_PeripheralBaseAddr = (uint32_t)&(USART1->DR);//�������ݼĴ�����ַ
    dma.DMA_Memory0BaseAddr = (uint32_t)&sbus_rx_buffer[0][0];//�ڴ��ַ
    dma.DMA_DIR = DMA_DIR_PeripheralToMemory;//���ݴ��䷽��  ���赽�ڴ�
    dma.DMA_BufferSize =20;  //��������СΪ20�ֽ�
    dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//�����ַ������
    dma.DMA_MemoryInc = DMA_MemoryInc_Enable;//�ڴ�����ַ����
    dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    dma.DMA_Mode = DMA_Mode_Circular;//ѭ���ɼ�
    dma.DMA_Priority = DMA_Priority_VeryHigh;//DMAͨ�����ȼ�
    dma.DMA_FIFOMode = DMA_FIFOMode_Disable;//�����ȳ���FIFO��
    dma.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;//ѡ��FIFO��ֵ
    dma.DMA_MemoryBurst = DMA_MemoryBurst_Single;//�洢��ͻ����������
    dma.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//����ͻ����������
		
		DMA_DoubleBufferModeConfig(DMA2_Stream2,(uint32_t)&sbus_rx_buffer[1][0], DMA_Memory_0);   //����Memory_1����������ַ��������Memory_0Ϊ��ǰʹ�û�����
		DMA_DoubleBufferModeCmd(DMA2_Stream2, ENABLE);      //����DMA˫����ģʽ
    DMA_Init(DMA2_Stream2,&dma);
		
		USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);//��������DMA���͹���
		USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);//�������ڿ����ж�
		USART_Cmd(USART1,ENABLE);
		DMA_Cmd(DMA2_Stream2,ENABLE);
}


void USART1_IRQHandler(void)
{  	
	 u8 Current_NDTR=0;
	 OSIntEnter();
   if (USART_GetITStatus(USART1,USART_IT_IDLE) != RESET)  
   {  
		 //����жϱ�־    
		 (void)USART1->SR;   
		 (void)USART1->DR;
		 
		 if(DMA_GetCurrentMemoryTarget(DMA2_Stream2) == 0)        //��ǰ����ʹ��Memory_0������ 
		 { 
				 DMA_Cmd(DMA2_Stream2, DISABLE);                      //�ر�DMA  
				 Current_NDTR=DMA2_Stream2->NDTR;                     //��ȡʣ�໺����
				 DMA2_Stream2->NDTR=20;                               //����DMA����ֵ
				 DMA2_Stream2->CR |= (uint32_t)(DMA_SxCR_CT);         //�л���Memory_1������   
				 DMA_Cmd(DMA2_Stream2, ENABLE);                       //����DMA	
				 
				 if(Current_NDTR==0x02)       //��ʣ��2�������ֽڣ�˵����һ֡��������
				 {
						 RemoteDataProcess(sbus_rx_buffer[0]);					 
				 }			
		 }		 		 
		 else  //��ǰ����ʹ��Memory_1������
		 {    
				 DMA_Cmd(DMA2_Stream2, DISABLE); 
				 Current_NDTR =	DMA2_Stream2->NDTR;				 
				 DMA2_Stream2->NDTR = 20;     
				 DMA2_Stream2->CR &= ~(uint32_t)(DMA_SxCR_CT);   //�л���Memory_0������    
				 DMA_Cmd(DMA2_Stream2, ENABLE); 
			 
				 if(Current_NDTR == 0x02)      
				 {  
					 RemoteDataProcess(sbus_rx_buffer[1]);				 				 					 
				 }          
		 } 		 
	 }
	 OSIntExit(); 
}

u8 TeleCtrl_Rev_OK=10;
void RemoteDataProcess(uint8_t *sbus_rx_buffer) //���ݽ���
{     
	if(sbus_rx_buffer == NULL)     
	{         
		return;     
	}          
	TelCtrlData.right_x = ((int16_t)sbus_rx_buffer[0] | ((int16_t)sbus_rx_buffer[1] << 8)) & 0x07FF;   //��x  1684~1024`364   
	  TelCtrlData.right_x-=1024;
	  if(TelCtrlData.right_x>=665 || TelCtrlData.right_x<=-665)TelCtrlData.right_x=0;
	  if(TelCtrlData.right_x<=10 && TelCtrlData.right_x>=-10)TelCtrlData.right_x=0;  //�����
	TelCtrlData.right_y = (((int16_t)sbus_rx_buffer[1] >> 3) | ((int16_t)sbus_rx_buffer[2] << 5)) & 0x07FF; //��y    
	  TelCtrlData.right_y-=1024;
	  if(TelCtrlData.right_y>=665 || TelCtrlData.right_y<=-665)TelCtrlData.right_y=0;
	  if(TelCtrlData.right_y<=10 && TelCtrlData.right_y>=-10)TelCtrlData.right_y=0;
	TelCtrlData.left_x = (((int16_t)sbus_rx_buffer[2] >> 6) | ((int16_t)sbus_rx_buffer[3] << 2)|((int16_t)sbus_rx_buffer[4] << 10)) & 0x07FF;  //��x
    TelCtrlData.left_x-=1024;
	  if(TelCtrlData.left_x>=665 || TelCtrlData.left_x<=-665)TelCtrlData.left_x=0;
	TelCtrlData.left_y = (((int16_t)sbus_rx_buffer[4] >> 1) | ((int16_t)sbus_rx_buffer[5]<<7)) & 0x07FF;    //��y
	  TelCtrlData.left_y-=1024;
	  if(TelCtrlData.left_y>=665 || TelCtrlData.left_y<=-665)TelCtrlData.left_y=0;
	TelCtrlData.switch_l = ((sbus_rx_buffer[5] >> 4) & 0x000C) >> 2;     //�����������1~3~2
	TelCtrlData.switch_r = ((sbus_rx_buffer[5] >> 4) & 0x0003);          //�Ҳ���������
  TelCtrlData.mouse_x = ((int16_t)sbus_rx_buffer[6]) | ((int16_t)sbus_rx_buffer[7] << 8);  //���x�����ƶ��ٶ�    
	mouse_x[0]=mouse_x[1];
	mouse_x[1]=TelCtrlData.mouse_x;
	if(mouse_x[1]>32768)mouse_x[1]=mouse_x[1]-65535;
	if(mouse_x[1]>60)mouse_x[1]=60;
	else if(mouse_x[1]<-60)mouse_x[1]=-60;
	avemousex=(mouse_x[1]+mouse_x[0])/2.0f;
	TelCtrlData.mouse_y = ((int16_t)sbus_rx_buffer[8]) | ((int16_t)sbus_rx_buffer[9] << 8);  //���y�����ƶ��ٶ�   
	mouse_y[0]=mouse_y[1];
	mouse_y[1]=TelCtrlData.mouse_y;
	if(mouse_y[1]>32768)mouse_y[1]=mouse_y[1]-65535;
	if(mouse_y[1]>60)mouse_y[1]=60;
	else if(mouse_y[1]<-60)mouse_y[1]=-60;
	avemousey=(mouse_y[1]+mouse_y[0])/2.0f;
	TelCtrlData.mouse_z = ((int16_t)sbus_rx_buffer[10]) | ((int16_t)sbus_rx_buffer[11] << 8);  //���z�����ƶ��ٶ�(������z�����)    
  TelCtrlData.mouse_l = sbus_rx_buffer[12];     //������
	TelCtrlData.mouse_r = sbus_rx_buffer[13];     //����Ҽ�
	keyboard=TelCtrlData.key = ((int16_t)sbus_rx_buffer[14])|((int16_t)sbus_rx_buffer[15] << 8);    //���̰���  
	TeleCtrl_Rev_OK=0;	
}  
/*******************************************************************************/	
/*	HEX		      F				|		    F       |        F         |        F          */
/*                      |               |                  |                   */
/*  B IN		1		1		1		1	|	1		1		1		1	|	1		1		 1		1	 |	1		1		1		1    */
/*											|               |                  |                   */
/*  Key		B		V		C		X |	Z		G		F		R	|	E		Q	 Ctrl Shift|	D		A		S		W    */
/*******************************************************************************/
//u8 flag_V_shift=0,flag_V=0;
//u8 flag_E_right=0,flag_Q_left=0,flag_sway_ctrl=0,flag_Vw_C=0,flag_Vw_R,flag_Vw_F,flag_Vw_Q=0,flag_E=0;
//u8 flag_2006_start=0;
void Tel_Control(void)
{
	if(TelCtrlData.switch_l==1){TIM5->CCR2=165;TIM5->CCR3=165;}
	else{TIM5->CCR2=220;TIM5->CCR3=220;}
	if(TelCtrlData.switch_l==2){TIM5->CCR1=50;}
	else{TIM5->CCR1=200;}
}
//	if(switch_shift==8 && keyboard==Keyboard_Q)    //�е���״̬��������
//	{
//		V_max=250;
//		V_min=95;
//		V_mid=180;
//		acceleration_min=0.75;
//		acceleration_mid=1;
//		acceleration_max=1.5;
//	}

//	else if(switch_shift==8 && (keyboard==Keyboard_V || keyboard==Keyboard_F) && flag_buff_start!=1)    //�е���(����)
//	{
//		V_max=500;
//		V_min=300;
//		V_mid=400;
//		acceleration_min=1.5;
//		acceleration_mid=2;
//		acceleration_max=3;
//	}
//	else if(switch_shift==7)    //�޵���״̬
//	{
//		V_max=200;
//		
//		V_min=65;
//		V_mid=140;
//		acceleration_min=0.75;
//		acceleration_mid=1;
//		acceleration_max=1.5;
//	}
//	
//	if(keyboard==Keyboard_shift+Keyboard_R)            //�����ݻָ�Ĭ��
//	{
//		V_max=250;
//		V_min=95;
//		V_mid=180;
//		acceleration_min=0.75;
//		acceleration_mid=1;
//		acceleration_max=1.5;
//	}
//	 
//	V_mousex =avemousex*0.025;
//	V_mousey =-avemousey*0.015;
//	
//	if(TelCtrlData.mouse_r==1)
//	{
//		V_mousex =0.5*avemousex*0.015;
//	  V_mousey =-0.5*avemousey*0.0075;
//	}
//	
//	Vx_rocker=TelCtrlData.left_x*180/680;     //130
//	Vy_rocker=TelCtrlData.left_y*180/680;     //130
	
//	if(TelCtrlData.mouse_l==1 && flag_bo==0 && flag_friction_start==1)
//	{
//		target_anger_bodan_208 +=200; 
//		flag_bo=1; 
//		flag_2006_start=1;
//	}
//	if(TelCtrlData.mouse_l==0)
//	{
//		flag_bo=0;
//	}
	
	
//	if(TelCtrlData.switch_r==1 && flag_bo==0 )
//	{
//		target_anger_bodan_208 +=108; 
//		flag_bo=1; 
////		flag_2006_start=1;
//	}
//	if(TelCtrlData.switch_r==3)
//	{
//		flag_bo=0;
//	}
	
	
//	if(keyboard==Keyboard_shift && flag_V_shift==0)   // �����ٶȵ�
//	{
//		if(flag_V==0)
//		{
//			V_max=150;
//			flag_V=1;
//		}
//		else if(flag_V==1)
//		{
//			V_max=100;
//			flag_V=0;
//		}
//		flag_V_shift=1;
//	}
//	
//	if(keyboard!=Keyboard_shift)
//	{
//		flag_V_shift=0;
//	}
	
	
//	if(keyboard==Keyboard_Z)
//	{
//		count_bullet=0;
//	}
//	if(keyboard==Keyboard_X)
//	{
//		count_bullet=4;
//	}  
//	if(keyboard==Keyboard_Q)
//	{
//		flag_Vw_Q=1;
//	}
//	
//	if(keyboard==Keyboard_E )
//	{
//		flag_E_right=1;
////		flag_E=0;
//	}
//	if(keyboard==Keyboard_V )
//	{
//		flag_E_right=0;
//	}
//	if(keyboard!=Keyboard_E && flag_E==0 && flag_E_right==1)
//	{
//		flag_E=1;
//	}
//	if(keyboard==Keyboard_E && flag_E==1)
//	{
//		flag_E_right=0;
//		flag_E=1;
//	}
//	if(keyboard!=Keyboard_E && flag_E==1 && flag_E_right==0)
//	{
//		flag_E=0;
//	}
	
//	if(keyboard==Keyboard_ctrl)
//	{
//		flag_Q_left=0;
//		flag_E_right=0;
//		flag_sway_ctrl=1;
//		if(Vx_keyboard>0)Vx_keyboard-=5;
//		if(Vx_keyboard<0)Vx_keyboard+=5;
//		if(Vx_keyboard>-5&&Vx_keyboard<5)Vx_keyboard=0;
//		if(Vy_keyboard>0)Vy_keyboard-=5;
//		if(Vy_keyboard<0)Vy_keyboard+=5;
//		if(Vy_keyboard>-5&&Vy_keyboard<5)Vy_keyboard=0;
//	}
//  if(keyboard==Keyboard_ctrl+Keyboard_W)
//	{
//		Vy_keyboard+=0.5;
//		if(Vy_keyboard>=V_max)Vy_keyboard=V_max;
//		if(Vx_keyboard>0)Vx_keyboard-=5;
//		if(Vx_keyboard<0)Vx_keyboard+=5;
//		if(Vx_keyboard>-5&&Vx_keyboard<5)Vx_keyboard=0;
//	}
//  if(keyboard==Keyboard_ctrl+Keyboard_S)
//	{
//		Vy_keyboard-=0.5;
//		if(Vy_keyboard<=-V_max)Vy_keyboard=-V_max;
//		if(Vx_keyboard>0)Vx_keyboard-=5;
//		if(Vx_keyboard<0)Vx_keyboard+=5;
//		if(Vx_keyboard>-5&&Vx_keyboard<5)Vx_keyboard=0;
//	}
//  if(keyboard==Keyboard_ctrl+Keyboard_A)
//	{
//		Vx_keyboard-=0.5;
//		if(Vx_keyboard<=-V_max)Vx_keyboard=-V_max;
//		if(Vy_keyboard>0)Vy_keyboard-=5;
//		if(Vy_keyboard<0)Vy_keyboard+=5;
//		if(Vy_keyboard>-5&&Vy_keyboard<5)Vy_keyboard=0;
//	}
//  if(keyboard==Keyboard_ctrl+Keyboard_D)
//	{
//		Vx_keyboard+=0.5;
//		if(Vx_keyboard>=V_max)Vx_keyboard=V_max;
//		if(Vy_keyboard>0)Vy_keyboard-=5;
//		if(Vy_keyboard<0)Vy_keyboard+=5;
//		if(Vy_keyboard>-5&&Vy_keyboard<5)Vy_keyboard=0;
//	}
	
//	if(keyboard==Keyboard_C)
//	{
//		flag_Vw_C=1;
//	}
//	else flag_Vw_C=0;
//	
//	if(keyboard==Keyboard_R)
//	{
//		flag_Vw_R=1;
//	}
//	else flag_Vw_R=0;
//	
//	if(keyboard==Keyboard_F)
//	{
//		flag_Vw_F=1;
//	}
//	else flag_Vw_F=0;



//	if(keyboard==0)
//	{
//		if(Vx_keyboard>0)Vx_keyboard-=acceleration_slowdown;
//		if(Vx_keyboard<0)Vx_keyboard+=acceleration_slowdown;
//		if(Vx_keyboard>-acceleration_slowdown && Vx_keyboard<acceleration_slowdown)Vx_keyboard=0;
//		if(Vy_keyboard>0)Vy_keyboard-=acceleration_slowdown;
//		if(Vy_keyboard<0)Vy_keyboard+=acceleration_slowdown;
//		if(Vy_keyboard>-acceleration_slowdown && Vy_keyboard<acceleration_slowdown)Vy_keyboard=0;
//	}
//	
//	if(keyboard==Keyboard_W)
//	{
//		Vy_keyboard+=acceleration_mid;
//		if(Vy_keyboard>=V_mid)Vy_keyboard=V_mid;
//		if(Vx_keyboard>0)Vx_keyboard-=acceleration_slowdown;
//		if(Vx_keyboard<0)Vx_keyboard+=acceleration_slowdown;
//		if(Vx_keyboard>-acceleration_slowdown && Vx_keyboard<acceleration_slowdown)Vx_keyboard=0;
//	}
//	
//	if(keyboard==Keyboard_S)
//	{
//		Vy_keyboard-=acceleration_mid;
//		if(Vy_keyboard<=-V_mid)Vy_keyboard=-V_mid;
//		if(Vx_keyboard>0)Vx_keyboard-=acceleration_slowdown;
//		if(Vx_keyboard<0)Vx_keyboard+=acceleration_slowdown;
//		if(Vx_keyboard>-acceleration_slowdown && Vx_keyboard<acceleration_slowdown)Vx_keyboard=0;
//	}
//	
//	if(keyboard==Keyboard_A)
//	{
//		Vx_keyboard-=acceleration_mid;
//		if(Vx_keyboard<=-V_mid)Vx_keyboard=-V_mid;
//		if(Vy_keyboard>0)Vy_keyboard-=acceleration_slowdown;
//		if(Vy_keyboard<0)Vy_keyboard+=acceleration_slowdown;
//		if(Vy_keyboard>-acceleration_slowdown && Vy_keyboard<acceleration_slowdown)Vy_keyboard=0;
//	}
//	
//	if(keyboard==Keyboard_D)
//	{
//		Vx_keyboard+=acceleration_mid;
//		if(Vx_keyboard>=V_mid)Vx_keyboard=V_mid;
//		if(Vy_keyboard>0)Vy_keyboard-=acceleration_slowdown;
//		if(Vy_keyboard<0)Vy_keyboard+=acceleration_slowdown;
//		if(Vy_keyboard>-acceleration_slowdown && Vy_keyboard<acceleration_slowdown)Vy_keyboard=0;
//	}
//	
//	if(keyboard==Keyboard_D+Keyboard_W)
//	{
//		Vx_keyboard+=acceleration_mid;
//		Vy_keyboard+=acceleration_mid;
//		if(Vx_keyboard>=V_mid)Vx_keyboard=V_mid;
//		if(Vy_keyboard>=V_mid)Vy_keyboard=V_mid;
//	}
//	
//	if(keyboard==Keyboard_D+Keyboard_S)
//	{
//		Vx_keyboard+=acceleration_mid;
//		Vy_keyboard-=acceleration_mid;
//		if(Vx_keyboard>=V_mid)Vx_keyboard=V_mid;
//		if(Vy_keyboard<=-V_mid)Vy_keyboard=-V_mid;
//	}
//	
//	if(keyboard==Keyboard_A+Keyboard_S)
//	{
//		Vx_keyboard-=acceleration_mid;
//		Vy_keyboard-=acceleration_mid;
//		if(Vx_keyboard<=-V_mid)Vx_keyboard=-V_mid;
//		if(Vy_keyboard<=-V_mid)Vy_keyboard=-V_mid;
//	}
//	
//	if(keyboard==Keyboard_A+Keyboard_W)
//	{
//		Vx_keyboard-=acceleration_mid;
//		Vy_keyboard+=acceleration_mid;
//		if(Vx_keyboard<=-V_mid)Vx_keyboard=-V_mid;
//		if(Vy_keyboard>=V_mid)Vy_keyboard=V_mid;
//	}
//	
//	if(keyboard==Keyboard_W+Keyboard_shift)
//	{
//		Vy_keyboard+=acceleration_max;
//		if(Vy_keyboard>=V_max)Vy_keyboard=V_max;
//		if(Vx_keyboard>0)Vx_keyboard-=acceleration_slowdown;
//		if(Vx_keyboard<0)Vx_keyboard+=acceleration_slowdown;
//		if(Vx_keyboard>-acceleration_slowdown && Vx_keyboard<acceleration_slowdown)Vx_keyboard=0;
//	}
//	
//	if(keyboard==Keyboard_S+Keyboard_shift)
//	{
//		Vy_keyboard-=acceleration_max;
//		if(Vy_keyboard<=-V_max)Vy_keyboard=-V_max;
//		if(Vx_keyboard>0)Vx_keyboard-=acceleration_slowdown;
//		if(Vx_keyboard<0)Vx_keyboard+=acceleration_slowdown;
//		if(Vx_keyboard>-acceleration_slowdown && Vx_keyboard<acceleration_slowdown)Vx_keyboard=0;
//	}
//	
//	if(keyboard==Keyboard_A+Keyboard_shift)
//	{
//		Vx_keyboard-=acceleration_max;
//		if(Vx_keyboard<=-V_max)Vx_keyboard=-V_max;
//		if(Vy_keyboard>0)Vy_keyboard-=acceleration_slowdown;
//		if(Vy_keyboard<0)Vy_keyboard+=acceleration_slowdown;
//		if(Vy_keyboard>-acceleration_slowdown && Vy_keyboard<acceleration_slowdown)Vy_keyboard=0;
//	}
//	
//	if(keyboard==Keyboard_D+Keyboard_shift)
//	{
//		Vx_keyboard+=acceleration_max;
//		if(Vx_keyboard>=V_max)Vx_keyboard=V_max;
//		if(Vy_keyboard>0)Vy_keyboard-=acceleration_slowdown;
//		if(Vy_keyboard<0)Vy_keyboard+=acceleration_slowdown;
//		if(Vy_keyboard>-acceleration_slowdown && Vy_keyboard<acceleration_slowdown)Vy_keyboard=0;
//	}
//	
//	if(keyboard==Keyboard_D+Keyboard_W+Keyboard_shift)
//	{
//		Vx_keyboard+=acceleration_max;
//		Vy_keyboard+=acceleration_max;
//		if(Vx_keyboard>=V_max)Vx_keyboard=V_max;
//		if(Vy_keyboard>=V_max)Vy_keyboard=V_max;
//	}
//	
//	if(keyboard==Keyboard_D+Keyboard_S+Keyboard_shift)
//	{
//		Vx_keyboard+=acceleration_max;
//		Vy_keyboard-=acceleration_max;
//		if(Vx_keyboard>=V_max)Vx_keyboard=V_max;
//		if(Vy_keyboard<=-V_max)Vy_keyboard=-V_max;
//	}
//	
//	if(keyboard==Keyboard_A+Keyboard_S+Keyboard_shift)
//	{
//		Vx_keyboard-=acceleration_max;
//		Vy_keyboard-=acceleration_max;
//		if(Vx_keyboard<=-V_max)Vx_keyboard=-V_max;
//		if(Vy_keyboard<=-V_max)Vy_keyboard=-V_max;
//	}
//	
//	if(keyboard==Keyboard_A+Keyboard_W+Keyboard_shift)
//	{
//		Vx_keyboard-=acceleration_max;
//		Vy_keyboard+=acceleration_max;
//		if(Vx_keyboard<=-V_max)Vx_keyboard=-V_max;
//		if(Vy_keyboard>=V_max)Vy_keyboard=V_max;
//	}
//	
//	
//	if(keyboard==Keyboard_W+Keyboard_ctrl)
//	{
//		Vy_keyboard+=acceleration_min;
//		if(Vy_keyboard>=V_min)Vy_keyboard=V_min;
//		if(Vx_keyboard>0)Vx_keyboard-=acceleration_slowdown;
//		if(Vx_keyboard<0)Vx_keyboard+=acceleration_slowdown;
//		if(Vx_keyboard>-acceleration_slowdown && Vx_keyboard<acceleration_slowdown)Vx_keyboard=0;
//	}
//	
//	if(keyboard==Keyboard_S+Keyboard_ctrl)
//	{
//		Vy_keyboard-=acceleration_min;
//		if(Vy_keyboard<=-V_min)Vy_keyboard=-V_min;
//		if(Vx_keyboard>0)Vx_keyboard-=acceleration_slowdown;
//		if(Vx_keyboard<0)Vx_keyboard+=acceleration_slowdown;
//		if(Vx_keyboard>-acceleration_slowdown && Vx_keyboard<acceleration_slowdown)Vx_keyboard=0;
//	}
//	
//	if(keyboard==Keyboard_A+Keyboard_ctrl)
//	{
//		Vx_keyboard-=acceleration_min;
//		if(Vx_keyboard<=-V_min)Vx_keyboard=-V_min;
//		if(Vy_keyboard>0)Vy_keyboard-=acceleration_slowdown;
//		if(Vy_keyboard<0)Vy_keyboard+=acceleration_slowdown;
//		if(Vy_keyboard>-acceleration_slowdown && Vy_keyboard<acceleration_slowdown)Vy_keyboard=0;
//	}
//	
//	if(keyboard==Keyboard_D+Keyboard_ctrl)
//	{
//		Vx_keyboard+=acceleration_min;
//		if(Vx_keyboard>=V_min)Vx_keyboard=V_min;
//		if(Vy_keyboard>0)Vy_keyboard-=acceleration_slowdown;
//		if(Vy_keyboard<0)Vy_keyboard+=acceleration_slowdown;
//		if(Vy_keyboard>-acceleration_slowdown && Vy_keyboard<acceleration_slowdown)Vy_keyboard=0;
//	}
//	
//	if(keyboard==Keyboard_D+Keyboard_W+Keyboard_ctrl)
//	{
//		Vx_keyboard+=acceleration_min;
//		Vy_keyboard+=acceleration_min;
//		if(Vx_keyboard>=V_min)Vx_keyboard=V_min;
//		if(Vy_keyboard>=V_min)Vy_keyboard=V_min;
//	}
//	
//	if(keyboard==Keyboard_D+Keyboard_S+Keyboard_ctrl)
//	{
//		Vx_keyboard+=acceleration_min;
//		Vy_keyboard-=acceleration_min;
//		if(Vx_keyboard>=V_min)Vx_keyboard=V_min;
//		if(Vy_keyboard<=-V_min)Vy_keyboard=-V_min;
//	}
//	
//	if(keyboard==Keyboard_A+Keyboard_S+Keyboard_ctrl)
//	{
//		Vx_keyboard-=acceleration_min;
//		Vy_keyboard-=acceleration_min;
//		if(Vx_keyboard<=-V_min)Vx_keyboard=-V_min;
//		if(Vy_keyboard<=-V_min)Vy_keyboard=-V_min;
//	}
//	
//	if(keyboard==Keyboard_A+Keyboard_W+Keyboard_ctrl)
//	{
//		Vx_keyboard-=acceleration_min;
//		Vy_keyboard+=acceleration_min;
//		if(Vx_keyboard<=-V_min)Vx_keyboard=-V_min;
//		if(Vy_keyboard>=V_min)Vy_keyboard=V_min;
//	}
	
	
//	Vx_mid=(Vx_keyboard+Vx_rocker)*35;
//	Vy_mid=(Vy_keyboard+Vy_rocker)*40;
//	Vx=Vx_mid*cos((this_angle_Yaw-MID_angle)*2*3.1415926/360)-Vy_mid*sin((this_angle_Yaw-MID_angle)*2*3.1415926/360);
//	Vy=Vx_mid*sin((this_angle_Yaw-MID_angle)*2*3.1415926/360)+Vy_mid*cos((this_angle_Yaw-MID_angle)*2*3.1415926/360);
//	Vw=TelCtrlData.right_x*4500/680;
//}


