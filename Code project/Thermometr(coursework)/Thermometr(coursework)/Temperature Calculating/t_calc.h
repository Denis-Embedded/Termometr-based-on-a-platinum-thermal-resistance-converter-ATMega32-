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

#define ADCfactorK_C 512500		//������������ ��� �������������� ���������� 
#define ADCfactorB_C -131072000	//�������� ����������� � �������� �������

#define ADCfactorK_F 922500		//������������ ��� �������������� ����������
#define ADCfactorB_F -214958080	//�������� ����������� � �������� ����������

union temperature_t
{
	float f_cell;			//����� �����������, � ������� ����� �������� 
							//������������ �������� ����������� ��� ��������
							//�� ��������� ModBus RTU
	uint16_t buff[2]; 		//��� ���� ����� ��� �������� ����� �������� � �������
							//������ USART
};

//������������� �������� ������ ��� �������� �����������
volatile union temperature_t temp_C = {0};
volatile union temperature_t temp_K = {0};
volatile union temperature_t temp_F = {0};

//���������� ��� ������ �� ����������� ���������
volatile int16_t C_temp_code = 0;
volatile int16_t K_temp_code = 0;
volatile int16_t F_temp_code = 0;

void get_temp(void); //�������� ������� ��������� �����������
#endif /* T_CALC_H_ */