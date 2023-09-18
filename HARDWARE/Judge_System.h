#ifndef __Judge_System_H
#define __Judge_System_H
#include <stm32f4xx.h>
#include <stdio.h>



/*********����ϵͳ�Զ������ݷ���   ���CRCУ��   **********/
typedef __packed struct  
{
  uint16_t Content_ID;
	uint16_t Robot_ID;
	uint16_t Client_ID;
		float data1; 
		float data2; 
		float data3; 
    uint8_t masks; 
}client_custom_data_t;    //�û��Զ������� 

typedef __packed struct
{
	uint8_t SOF;          //������ʼ�ֽڣ��̶�Ϊ0xA5          
	uint16_t data_length;  //���ݳ���
	uint8_t Seq;          //�����
	uint8_t CRC8;         //֡ͷCRCУ��
}ext_FrameHeader;//֡ͷ

typedef __packed struct
{
	ext_FrameHeader    FrameHeader;
	uint16_t          CmdID;
	client_custom_data_t client_custom_data; //�Զ������� 	
	uint16_t        CRC16;         //֮ǰ��������CRCУ��   ע������ݺ�֮ǰ�����ݿ��ܲ����������Բ�Ҫֱ��ʹ�ã�����Ҫֱ��ʹ�ã������ڴ˸�ֵ
}tFrame;  //����֡


typedef enum 
{
	game_state=0x0001,
	game_result=0x0002,
	game_robot_survivors=0x0003,
	event_data=0x0101,
	supply_projectile_action=0x0102,
	supply_projectile_booking=0x0103,
	game_robot_state=0x0201,
	power_heat_data=0x0202,
	game_robot_pos=0x0203,
	buff_musk=0x0204,
	aerial_robot_energy=0x0205,
	robot_hurt=0x0206,
	shoot_data=0x0207,
	
}tCmdID;


typedef __packed struct
{
  uint8_t game_type : 4;   
  uint8_t game_progress : 4;   
  uint16_t stage_remain_time; 
}ext_game_state_t;             //����״̬����

typedef __packed struct 
{
  uint8_t winner;
}ext_game_result_t;        //�����������

typedef __packed struct 
{
  uint16_t robot_legion; 
}ext_game_robot_survivors_t;     //�����˴������

typedef __packed struct 
{
  uint32_t event_type; 
}ext_event_data_t;          //�����¼�����

typedef __packed struct 
{ 
  uint8_t supply_projectile_id; 
  uint8_t supply_robot_id; 
  uint8_t supply_projectile_step;  
  uint8_t supply_projectile_num; 
}ext_supply_projectile_action_t;   //����վ������ʶ

typedef __packed struct 
{
  uint8_t supply_projectile_id;
  uint8_t supply_robot_id;
  uint8_t supply_num;  
}ext_supply_projectile_booking_t;   //���󲹸�վ�����ӵ�

typedef __packed struct 
{   
	uint8_t robot_id; //������ID  
	uint8_t robot_level;   //�����˵ȼ�
	uint16_t remain_HP;   //������ʣ��Ѫ��
	uint16_t max_HP;   //  ����������Ѫ��
	uint16_t shooter_heat0_cooling_rate;  // 17mmǹ��ÿ����ȴֵ
	uint16_t shooter_heat0_cooling_limit; //  17mmǹ����������
	uint16_t shooter_heat1_cooling_rate;  //   42mmǹ��ÿ����ȴֵ
	uint16_t shooter_heat1_cooling_limit;   //  42mmǹ����������
	uint8_t mains_power_gimbal_output : 1;  // ���ص�Դ������
	uint8_t mains_power_chassis_output : 1; //  
	uint8_t mains_power_shooter_output : 1;//
}ext_game_robot_state_t;            //

typedef __packed struct 
{   
	uint16_t chassis_volt;   // ���������ѹ
	uint16_t chassis_current;    //�����������
	float chassis_power;    //����
	uint16_t chassis_power_buffer; // ���ʻ���  ����
	uint16_t shooter_heat0;    //17mmǹ������
	uint16_t shooter_heat1;  //42mmǹ������
}ext_power_heat_data_t;               //ʵʱ������������

typedef __packed struct 
{
  float x;
  float y;  
  float z;  
  float yaw;
}ext_game_robot_pos_t;       //������λ��

typedef __packed struct 
{
  uint8_t power_rune_buff;
}ext_buff_musk_t;          //����������

typedef __packed struct 
{
  uint8_t energy_point;
  uint8_t attack_time; 
}aerial_robot_energy_t;     //���л���������״̬

typedef __packed struct 
{ 
  uint8_t armor_id : 4;   
  uint8_t hurt_type : 4;
}ext_robot_hurt_t;              //�˺�״̬

typedef __packed struct
{  
uint8_t bullet_type;   
uint8_t bullet_freq;   
float bullet_speed; 
}ext_shoot_data_t;      //ʵʱ�����Ϣ






//typedef union
//{
//	float fdata;
//	unsigned long ldata;
//}FloatLongType;



extern u8 Judge_Data[100];
extern ext_game_robot_state_t ext_game_robot_state;
extern ext_power_heat_data_t ext_power_heat_data;
extern client_custom_data_t client_custom_data;
extern uint16_t heat_42mm[2];

void DATA_0003(u8 Res);
void Judge_System(void);
void DATA_0201(u8 Res);
void DATA_0202(u8 Res);
void DATA_0001(u8 Res);
void DATA_0002(u8 Res);
void Get_Data(void);
void DATA_0101(u8 Res);
void DATA_0102(u8 Res);
void DATA_0103(u8 Res);
void DATA_0203(u8 Res);
void DATA_0204(u8 Res);
void DATA_0205(u8 Res);
void DATA_0206(u8 Res);
void DATA_0207(u8 Res);

int16_t Heat_42mm(void);
int16_t Heat_17mm(void);
int16_t Heat_17mm_rest(void);
float chasis_power(void);
void judge_system_transmit(float data1,float data2,float data3,uint8_t data_led,uint8_t Robot_ID,uint16_t client_ID);
void Append_CRC16_Check_Sum(uint8_t * pchMessage,uint32_t dwLength);
void Append_CRC8_Check_Sum(unsigned char *pchMessage, unsigned int dwLength);
void Send_FrameData(uint16_t cmdid,client_custom_data_t client_custom_data,uint8_t dwLength);
uint8_t led_control(uint8_t led_5,uint8_t led_4,uint8_t led_3,uint8_t led_2,uint8_t led_1,uint8_t led_0);
uint16_t client_ID_match(uint8_t robot_id);





#endif




