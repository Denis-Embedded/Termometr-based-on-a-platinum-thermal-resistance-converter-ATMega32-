/*
 * t_calc.h
 *
 * Created: 06.05.2023 21:18:29
 *  Author: user4
 */ 


#ifndef T_CALC_H_
#define T_CALC_H_
#include <avr/io.h>
#include <avr/interrupt.h>

#define ADCfactorK_C 512500LL		//������������ ��� �������������� ���������� 
#define ADCfactorB_C -131072000LL	//�������� ����������� � �������� �������

#define ADCfactorK_F 922500LL		//������������ ��� �������������� ����������
#define ADCfactorB_F -214958080LL	//�������� ����������� � �������� ����������

union temperature_t
{
	float f_cell;			//����� �����������, � ������� ����� �������� 
					//������������ �������� ����������� ��� ��������
					//�� ��������� ModBus RTU
	uint16_t buff[2]; 		//��� ���� ����� ��� �������� ����� �������� � �������
					//����������� Modbus RTU
};

//������������� �������� ������ ��� �������� �����������
extern volatile union temperature_t temp_C;
extern volatile union temperature_t temp_K;
extern volatile union temperature_t temp_F;

//���������� ��� ������ �� ����������� ���������
extern volatile int16_t C_temp_code;
extern volatile int16_t K_temp_code;
extern volatile int16_t F_temp_code;

void get_temp(void); //�������� ������� ��������� �����������
#endif /* T_CALC_H_ */