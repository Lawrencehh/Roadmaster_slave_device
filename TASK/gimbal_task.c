#include "can1.h"
#include "can2.h"
#include "task.h"
#include "delay.h"
#include "pstwo.h"
#include "uart3.h"
#include "uart6.h"
#include "pwm.h"
#include "imu.h"
#include "gpio.h"
#include "includes.h"
#include "math.h"
#include <stdint.h>  // ����int16_t��ͷ�ļ�

#include "SCServo.h" // ���ش��ڶ��
#include "SMS_STS.h"


void USART6_Init(void);
/****************************************************************************
													GM6020�����PID����
****************************************************************************/
int32_t GM6020_setpoint = 1;  // �趨��
int32_t GM6020_output_limit = 30000; // �������ѹ��������
int32_t GM6020_rotation_count = 0;  // ��ת������ÿ���һȦ����1�����1��
int32_t GM6020_absolute_position = 0;  // ����λ��
int32_t GM6020_error = 0; // ���ֵ
int32_t GM6020_position_difference = 0; // ����λ�ò�
// PID����
double GM6020_Kp = 10;  // ��������
double GM6020_Ki = 0.001;  // ���ֳ���
double GM6020_Kd = 0.001; // ΢�ֳ���
int32_t GM6020_output = 0;    // �����ѹ -30000 - 30000 ��ֹ���
int32_t GM6020_prev_error = 0; // ��һ�ε����
int32_t GM6020_integral = 0;   // ������
int32_t GM6020_derivative; 	// ���΢��			
// ����GM6020_PID������
void GM6020_update_pid() {
		// ����λ�ò�
		GM6020_position_difference = GM6020_current_raw_position - GM6020_last_raw_position;
		// ����Ƿ���λ�ð���
		if (abs(GM6020_position_difference) > 4096) {  // 8191 / 2 = 4095.5
				if (GM6020_position_difference > 0) {
						--GM6020_rotation_count;
				} else {
						++GM6020_rotation_count;
				}
		}
		// �������λ��
		GM6020_absolute_position = GM6020_current_raw_position + GM6020_rotation_count * 8192;
    GM6020_error = GM6020_setpoint - GM6020_absolute_position;  // �������
    GM6020_integral += GM6020_error;                // ����������
    GM6020_derivative = GM6020_error - GM6020_prev_error;  // �������΢��
    // ���������ѹ
		GM6020_output = GM6020_Kp * GM6020_error + GM6020_Ki * GM6020_integral + GM6020_Kd * GM6020_derivative; // PID����ϵͳ�г�����ƫ�steady-state error��ʱ��ʹ��PID��
		// ����GM6020_output��-GM6020_output_limit��GM6020_output_limit֮��
    if (GM6020_output > GM6020_output_limit) {
        GM6020_output = GM6020_output_limit;
    } else if (GM6020_output < -GM6020_output_limit) {
        GM6020_output = -GM6020_output_limit;
    }
		// ���Ϳ���ָ��
		GM6020_Can_Send_Msg(GM6020_output,0,0,0);
    GM6020_prev_error = GM6020_error;  // ������һ�ε����
		// ������һ�ε�ԭʼλ��
		GM6020_last_raw_position = GM6020_current_raw_position;
		// ���µ�ǰλ��
		GM6020_current_raw_position = GripperMotor_205_t.position; 
}
/****************************************************************************/

/****************************************************************************
													C610�����PID����
****************************************************************************/
int32_t C610_setpoint = 4000;  // �趨��
int32_t C610_output_limit = 10000; // �����������������
int32_t C610_rotation_count = 0;  // ��ת������ÿ���һȦ����1�����1��
int32_t C610_absolute_position = 0;  // ����λ��
int32_t C610_error = 0; // ���ֵ
int32_t C610_position_difference = 0; // ����λ�ò�
// PID����
double C610_Kp = 0;  // ��������   ��Ҫ����
double C610_Ki = 0;  // ���ֳ���
double C610_Kd = 0; // ΢�ֳ���
int32_t C610_output = 0;    // �����ѹ -10000 - 10000 ��ֹ���
int32_t C610_prev_error = 0; // ��һ�ε����
int32_t C610_integral = 0;   // ������
int32_t C610_derivative; 	// ���΢��	
// ����C610_PID������
void C610_update_pid() {
		// ����λ�ò�
		C610_position_difference = C610_current_raw_position - C610_last_raw_position;
		// ����Ƿ���λ�ð���
		if (abs(C610_position_difference) > 4096) {  // 8191 / 2 = 4095.5
				if (C610_position_difference > 0) {
						--C610_rotation_count;
				} else {
						++C610_rotation_count;
				}
		}
		// �������λ��
		C610_absolute_position = C610_current_raw_position + C610_rotation_count * 8192;
    C610_error = C610_setpoint - C610_absolute_position;  // �������
    C610_integral += C610_error;                // ����������
    C610_derivative = C610_error - C610_prev_error;  // �������΢��
    // ���������ѹ
		C610_output = C610_Kp * C610_error + C610_Ki * C610_integral + C610_Kd * C610_derivative; // PID����ϵͳ�г�����ƫ�steady-state error��ʱ��ʹ��PID��
		// ����C610_output��-C610_output_limit��C610_output_limit֮��
    if (C610_output > C610_output_limit) {
        C610_output = C610_output_limit;
    } else if (C610_output < -C610_output_limit) {
        C610_output = -C610_output_limit;
    }
		// ���Ϳ���ָ��
		C610_Can_Send_Msg(C610_output,0,0,0);
    C610_prev_error = C610_error;  // ������һ�ε����
		// ������һ�ε�ԭʼλ��
		C610_last_raw_position = C610_current_raw_position;
		// ���µ�ǰλ��
		C610_current_raw_position = GripperMotor_201_t.position; 
}
/****************************************************************************/

/****************************************************************************
															STS3032�������
****************************************************************************/



void STS3032_ServoControl(void)
{
	WritePosEx(1, 2000, 2250, 50);//���(ID1),������ٶ�V=2250��/��,���ٶ�A=50(50*100��/��^2),������P1=4095
	// ��һ��������ID�ţ�Ĭ��Ϊ1
	// �ڶ���������λ�ã�һȦΪ4096
	// ����������������ٶ� ��pulse/s��
	// ���ĸ������Ǽ��ٶ� ��pulse/s^2��
	
}




/****************************************************************************/

void Gimbal_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;


	
	while(1)
	{  	
			GM6020_update_pid(); // ����GM6020_PID������
			C610_update_pid();  // ����C610_PID������

			STS3032_ServoControl();
			OSTimeDly(10,OS_OPT_TIME_PERIODIC,&err); //��ʱ10ms
  } 
}



