
#include "includes.h"
#include "mpu6050.h"
#include "can2.h"
#include "task.h"
#include "uart1.h"
#include "uart3.h"
#include "math.h"


float target_angle_Yaw2=270.0f,target_angle_Pitch2=150.0f;//Сǹ������
int16_t output_207=0,output_209=0;
float this_angle_Pitch_min,this_angle_Yaw_min;
u8 flag_Vshift=0;
double Camera_Inc_Pitch_min,Camera_Inc_Yaw_min,X_distance,Y_distance,Z_distance,distance_min;
static float inc_205_min[2]={0.0f,0.0f},error205_v_min[2]={0.0f,0.0f};
static float inc_206_min[2]={0.0f,0.0f},error206_v_min[2]={0.0f,0.0f};
double_t limit_k_min,limit_kk_min;
int16_t count_n1_min=0,flag_count_start_min=0;
float Output_Camera_Inc_Yaw_min=0,Output_Camera_Inc_Pitch_min=0,yaw_mid=0.0f;	



void Gimbal_min_task(void *p_arg)    //ԭ��ΪС��̨�Ŀ��ƣ��������Ѳ���
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
//		MPU6050_42mm_ReadData();
//		
//		
//		if(newdata_flag)
//		{
//			newdata_flag=0;
//			X_distance=Camera_Distance*cos(Camera_Inc_Pitch*3.1415926/180)*sin(Camera_Inc_Yaw*3.1415926/180)-195.3;   //���Ǻ�������ϵ�任
//			Y_distance=Camera_Distance*cos(Camera_Inc_Pitch*3.1415926/180)*cos(Camera_Inc_Yaw*3.1415926/180)-53.6;
//			Z_distance=Camera_Distance*sin(Camera_Inc_Pitch*3.1415926/180)-124.7;
//			distance_min=sqrt(X_distance*X_distance+Y_distance*Y_distance+Z_distance*Z_distance);
//			Camera_Inc_Yaw_min=atan(X_distance/Y_distance)*180/3.1415926;
//			Camera_Inc_Pitch_min=atan(Z_distance/sqrt(X_distance*X_distance+Y_distance*Y_distance))*180/3.1415926;			
//			
//			
//			  flag_count_start_min=1;
//				count_n1_min=0;
//			 	limit_k_min=4*(fabs(Camera_Inc_Yaw_min)/28.0f)*(fabs(Camera_Inc_Yaw_min)/28.0f)*(fabs(Camera_Inc_Yaw_min)/28.0f)-6*(fabs(Camera_Inc_Yaw_min)/28.0f)*(fabs(Camera_Inc_Yaw_min)/28.0f)+3*(fabs(Camera_Inc_Yaw_min)/28.0f);
//				limit_kk_min=4*(fabs(Camera_Inc_Pitch_min)/28.0f)*(fabs(Camera_Inc_Pitch_min)/28.0f)*(fabs(Camera_Inc_Pitch_min)/28.0f)-6*(fabs(Camera_Inc_Pitch_min)/28.0f)*(fabs(Camera_Inc_Pitch_min)/28.0f)+3*(fabs(Camera_Inc_Pitch_min)/28.0f);
//	 			Output_Camera_Inc_Yaw_min=limit_k_min*Camera_PID_205_Position_min(Camera_Inc_Yaw_min);
//				Output_Camera_Inc_Pitch_min=limit_kk_min*Camera_PID_206_Position_min(Camera_Inc_Pitch_min);		
//		}
//		  if(flag_count_start_min==1)  
//	  	  {
//	  	  	count_n1_min++;
//	  	  }
//		
//		  if(count_n1_min>=5)
//		  	{
//		  		Output_Camera_Inc_Yaw_min=0;
//		  		Output_Camera_Inc_Pitch_min=0;
//		  	}
////			target_angle_Yaw2 -=0.8*PID_camera_yaw_min(0,Output_Camera_Inc_Yaw_min);   //ϵ���Ǹ���֡�ʼ������
////			target_angle_Pitch2 -=0.8*PID_camera_pitch_min(0,Output_Camera_Inc_Pitch_min);
//				yaw_mid-=0.8*PID_camera_yaw_min(0,Output_Camera_Inc_Yaw_min);
//				if(yaw_mid<-Output_Camera_Inc_Yaw_min )yaw_mid=-Output_Camera_Inc_Yaw_min;
//			
//			
//		if (target_angle_Pitch2>165)	          target_angle_Pitch2 = 165;
//		else if (target_angle_Pitch2<140)	    target_angle_Pitch2 = 140;	
//		if (target_angle_Yaw2>330)	            target_angle_Yaw2 = 330;
//		else if (target_angle_Yaw2<230)	      target_angle_Yaw2 = 230;	
//			
//		output_207 = PID_207_Velocity(PID_207_Position(target_angle_Yaw2,this_angle_Yaw_min),MPU6050_42mm_Real_Data.Gyro_Z);
//		output_209 = PID_209_Velocity(PID_209_Position(target_angle_Pitch2,this_angle_Pitch_min),MPU6050_42mm_Real_Data.Gyro_X);
//		
//		Gimbal_ESC_Send_Can2(output_209,0,0,0);
//		Gimbal_ESC_Send_Can2_yaw(0,output_207,0,0);
			
			
		
		 
		
		
//		if(TelCtrlData.switch_r==3 && speed_ready==0 && flag_Vshift==0)   //����Ħ����ʱ��ֹ������
//		{
//			Chasis_ESC_Send_Can2(0,0,PID_203_Velocity_bodan(0,Bodan_203_t.velocity),0);
//		}
//		else if(speed_ready==1 && TIM2->CCR1<1600)
//		{
//			Chasis_ESC_Send_Can2(0,0,PID_203_Velocity_bodan(0,Bodan_203_t.velocity),0);
//		}
		
		OSTimeDly(4,OS_OPT_TIME_PERIODIC,&err);
	}
}



int16_t PID_207_Velocity(int16_t target_velo,int16_t current_velo)  //yaw���ٶȻ�
{
	const float Kp =400.0f;  //27
	const float Ki=0.15f;   //0.05
	const float Kd =0.0f;
    
	static int32_t error_v[2] = {0.0,0.0};
	static int32_t error_sum=0;
	static int32_t error =0;
			
	error_v[0] = error_v[1];
	error_v[1] = -target_velo+current_velo;	
	error_sum += error_v[1];

	if(error_sum > 20000)  error_sum =  20000;      //���ƻ��ֱ���
	else if(error_sum < -20000) error_sum = -20000;
	error = error_v[1]  * Kp
				 +  error_sum * Ki 
				 + (error_v[1] - error_v[0]) * Kd;  
	
	if(error > 28000)  error = 28000;
	else if(error < -28000) error = -28000;
	return error;
}

int16_t PID_207_Position(float target_pos,float current_pos)  //yaw��λ�û�
{
	const float Kp =16.0f;   //15
	const float Ki=0.0f;
	const float Kd =0.0f;
    
	static float error_v[2] = {0.0,0.0};
	static float error_sum=0;
	static float error =0;
	
//	if(target_pos<=105)target_pos=105;
//	if(target_pos>=150)target_pos=150;
			
	error_v[0] = error_v[1];
	error_v[1] = -target_pos+current_pos;	
	error_sum += error_v[1];

	if(error_sum > 8000)  error_sum =  8000;      //���ƻ��ֱ���
	else if(error_sum < -8000) error_sum = -8000;
	error = error_v[1]  * Kp
				 +  error_sum * Ki 
				 + (error_v[1] - error_v[0]) * Kd; 
	
	if(error > 20000)  error = 20000;
	else if(error < -20000) error = -20000;
	return error;
}



int16_t PID_209_Velocity(int16_t target_velo,int16_t current_velo)  //Pitch���ٶȻ�
{
	const float Kp =27.0f;  //27
	const float Ki=0.05f;   //0.15
	const float Kd =0.0f;
    
	static int32_t error_v[2] = {0.0,0.0};
	static int32_t error_sum=0;
	static int32_t error =0;
			
	error_v[0] = error_v[1];
	error_v[1] = target_velo-current_velo;	
	error_sum += error_v[1];

	if(error_sum > 8000)  error_sum =  8000;      //���ƻ��ֱ���
	else if(error_sum < -8000) error_sum = -8000;
	error = error_v[1]  * Kp
				 +  error_sum * Ki 
				 + (error_v[1] - error_v[0]) * Kd; 
	
	if(error > 5000)  error = 5000;
	else if(error < -5000) error = -5000;
	return error;
}

int16_t PID_209_Position(float target_pos,float current_pos)  //Pitch��λ�û�
{
	const float Kp =25.0f;   //15
	const float Ki=0.0f;
	const float Kd =0.0f;
    
	static float error_v[2] = {0.0,0.0};
	static float error_sum=0;
	static float error =0;
//	
//	if(target_pos<=105)target_pos=105;
//	if(target_pos>=150)target_pos=150;
			
	error_v[0] = error_v[1];
	error_v[1] = -target_pos+current_pos;	
	error_sum += error_v[1];

	if(error_sum > 8000)  error_sum =  8000;      //���ƻ��ֱ���
	else if(error_sum < -8000) error_sum = -8000;
	error = error_v[1]  * Kp
				 +  error_sum * Ki 
				 + (error_v[1] - error_v[0]) * Kd; 
	
	if(error > 8000)  error = 8000;
	else if(error < -8000) error = -8000;
	return error;
}




#define FPS 200          //����ͷ֡��
#define T_Finish  0.002f  //��̨����ʱ��
float Camera_PID_205_Position_min(float new_inc)
{
	static float error =0;
		
	/*��������*/	
	inc_205_min[1]=new_inc;
	error205_v_min[0]=error205_v_min[1];
	error205_v_min[1]=inc_205_min[1]-inc_205_min[0];
	inc_205_min[0]=inc_205_min[1];
	 
	/*�������*/
	error = inc_205_min[1]																			 //���
					+ error205_v_min[1]*T_Finish*FPS;             									 //һ��
					+ (error205_v_min[1] - error205_v_min[0])*0.5f*T_Finish*T_Finish*FPS;    //����
	
	return error;
}

float Camera_PID_206_Position_min(float new_inc)
{
	static float error =0;
	
	/*��������*/	
	inc_206_min[1]=new_inc;
	error206_v_min[0]=error206_v_min[1];
	error206_v_min[1]=inc_206_min[1]-inc_206_min[0];
	inc_206_min[0]=inc_206_min[1];
	
	/*�������*/
	error = inc_206_min[1] 																							 //���
					+ error206_v_min[1]*T_Finish*FPS;             									 //һ��
					+ (error206_v_min[1] - error206_v_min[0])*0.5f*T_Finish*T_Finish*FPS;    //����
	
	return error;
}
float PID_camera_yaw_min(float target_pos,float current_pos)  
{
	float k; 
	k=(1-fabs(target_pos-current_pos)/30);
	const float Kp =0.01;          //0.35
	const float Ki=0.0f;          //0.001
	const float Kd =0.0f;          //       ͻȻ���ַ�ֹ����
    
	static float error_v[2] = {0.0,0.0};
	static float error_sum=0;
	static float error =0;
			
	error_v[0] = error_v[1];
	error_v[1] = -target_pos+current_pos;	
	error_sum += error_v[1];

	if(error_sum > 2000)  error_sum =  2000;      //���ƻ��ֱ���
	else if(error_sum < -2000) error_sum = -2000;
	error = error_v[1]  * Kp *k
				 +  error_sum * Ki 
				 + (error_v[1] - error_v[0]) * Kd; 
	
	if(error > 5000)  error = 5000;
	else if(error < -5000) error = -5000;
	return error;
}

float PID_camera_pitch_min(float target_pos,float current_pos)  
{
	float k;
	k=(1-fabs(target_pos-current_pos)/30);
	const float Kp =0.08;          //18.0f;
	const float Ki=0.0f;
	const float Kd =0.0f;
    
	static float error_v[2] = {0.0,0.0};
	static float error_sum=0;
	static float error =0;
			
	error_v[0] = error_v[1];
	error_v[1] = -target_pos+current_pos;	
	error_sum += error_v[1];

	if(error_sum > 2000)  error_sum =  2000;      //���ƻ��ֱ���
	else if(error_sum < -2000) error_sum = -2000;
	error = error_v[1]  * Kp * k
				 +  error_sum * Ki 
				 + (error_v[1] - error_v[0]) * Kd; 
	
	if(error > 5000)  error = 5000;
	else if(error < -5000) error = -5000;
	return error;
}






