#include "can2.h"
#include "can1.h"
#include "task.h"
#include "delay.h"
#include "uart1.h"
#include "uart3.h"
#include "uart6.h"
#include "pwm.h"
#include "imu.h"
#include "gpio.h"
#include "includes.h"
#include "Judge_System.h"

#include "SCServo.h" // ���ش��ڶ��
#include "SMS_STS.h"

int continue_flag=0;
int Pos;
int Speed;
int Load;
int Voltage;
int Temper;
int Move;
int Current;

//������ƣ���д
void STS3032_ServoControl(void)
{
	WritePosEx(1, 2000, 2250, 50);//���(ID1),������ٶ�V=2250��/��,���ٶ�A=50(50*100��/��^2),������P1=4095
//	// ��һ��������ID�ţ�Ĭ��Ϊ1
//	// �ڶ���������λ�ã�һȦΪ4096
//	// ����������������ٶ� ��pulse/s��
//	// ���ĸ������Ǽ��ٶ� ��pulse/s^2��
}

void Gun_task(void *p_arg)
{
	OS_ERR err;
//	STS3032_ServoControl(); //STS3032�������
	while(1)
	{  
//		STS3032_ServoControl(); //STS3032�������

		
		if(FeedBack(1)!=-1){
			Pos = ReadPos(-1);
			Speed = ReadSpeed(-1);
			Load = ReadLoad(-1);
			Voltage = ReadVoltage(-1);
			Temper = ReadTemper(-1);
			Move = ReadMove(-1);
			Current = ReadCurrent(-1);
			//printf("Pos:%d\n", Pos);
			//printf("Speed:%d\n", Speed);
			//printf("Load:%d\n", Load);
			//printf("Voltage:%d\n", Voltage);
			//printf("Temper:%d\n", Temper);
			//printf("Move:%d\n", Move);
			//printf("Current:%d\n", Current);
			delay_us(10);
		}
		Pos = ReadPos(1);
		
		OSTimeDly(10,OS_OPT_TIME_PERIODIC,&err); //��ʱ10ms	
	}
} 
	




 











//��ʱ��1�Ļص�����
void tmr1_callback(void *p_tmr, void *p_arg)
{
	continue_flag=1;		//��ʱ��1ִ�д�����1
}

void tmr2_callback(void *p_tmr, void *p_arg)
{
//			flag_get_pos=0;  //û�����ݱ�0
//      again_spin=0;
//	    time_again_flag=1;  
}


