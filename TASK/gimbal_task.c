#include "can1.h"
#include "can2.h"
#include "task.h"
#include "delay.h"
#include "pstwo.h"
#include "uart3.h"
#include "pwm.h"
#include "imu.h"
#include "gpio.h"
#include "includes.h"
#include "math.h"
#include <stdint.h>  // ����int16_t��ͷ�ļ�

int32_t GM6020_setpoint = 20000;  // �趨��
int32_t GM6020_output_limit = 30000; // �������ѹ��������

int32_t GM6020_rotation_count = 0;  // ��ת������ÿ���һȦ����1�����1��
int32_t GM6020_absolute_position = 0;  // ����λ��
int32_t error = 0; // ���ֵ
int32_t GM6020_position_difference = 0; // ����λ�ò�

// PID����
double GM6020_Kp = 10;  // ��������
double GM6020_Ki = 0.001;  // ���ֳ���
double GM6020_Kd = 0.001; // ΢�ֳ���
int32_t GM6020_output = 0;    // �����ѹ -30000 - 30000 ��ֹ���
int32_t prev_error = 0; // ��һ�ε����
int32_t integral = 0;   // ������
int32_t derivative; 	// ���΢��			

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

		
    error = GM6020_setpoint - GM6020_absolute_position;  // �������
    integral += error;                // ����������
    derivative = error - prev_error;  // �������΢��
    // ���������ѹ
		GM6020_output = GM6020_Kp * error + GM6020_Ki * integral + GM6020_Kd * derivative; // PID����ϵͳ�г�����ƫ�steady-state error��ʱ��ʹ��PID��
	
		// ����GM6020_output��-GM6020_output_limit��GM6020_output_limit֮��
    if (GM6020_output > GM6020_output_limit) {
        GM6020_output = GM6020_output_limit;
    } else if (GM6020_output < -GM6020_output_limit) {
        GM6020_output = -GM6020_output_limit;
    }
		
		// ���Ϳ���ָ��
		Can_Send_Msg(GM6020_output,0,0,0);
    prev_error = error;  // ������һ�ε����
		// ������һ�ε�ԭʼλ��
		GM6020_last_raw_position = GM6020_current_raw_position;
		// ���µ�ǰλ��
		GM6020_current_raw_position = GripperMotor_205_t.position; 
}

void Gimbal_task(void *p_arg)
{
	OS_ERR err;


	while(1)
	{  	

		
			GM6020_update_pid();  // ����GM6020_PID������
			OSTimeDly(10,OS_OPT_TIME_PERIODIC,&err); //��ʱ10ms
  } 
}



