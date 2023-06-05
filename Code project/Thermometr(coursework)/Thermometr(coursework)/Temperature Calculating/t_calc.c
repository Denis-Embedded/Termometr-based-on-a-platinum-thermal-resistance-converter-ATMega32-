/*
 * t_calc.c
 *
 * Created: 06.05.2023 21:18:45
 *  Author: user4
 */ 


#include "t_calc.h"
#include "AVR_ModBus.h"
//#include <string.h>

//инициализация структур данных для хранения температуры
volatile union temperature_t temp_C = {0};
volatile union temperature_t temp_K = {0};
volatile union temperature_t temp_F = {0};

//переменные для вывода на графический индикатор
volatile int16_t C_temp_code = 0;
volatile int16_t K_temp_code = 0;
volatile int16_t F_temp_code = 0;

void get_temp(void)
{
	ADMUX=0b00000101;
	ADCSRA|=(1<<ADSC);		// запуск АЦП;
	while ((ADCSRA&(1<<ADSC))!=0); //ожидание конца преобразования АЦП
	C_temp_code = ((int32_t)ADCfactorK_C * ADC + (int32_t)ADCfactorB_C) >> 16;
	temp_C.f_cell = (float)C_temp_code * 0.1f;
	for (uint8_t i = 0; i < 2; ++i)
	{//байты хранятся в памяти мк в порядке "от младшего к старшему"
		RegNum3x[i] = temp_C.buff[1 - i];
	}
	
	
	K_temp_code = C_temp_code + 2731; //переменные X_temp_value хранят значение в 10 раз больше фактического
	temp_K.f_cell = (float)K_temp_code * 0.1f;
	for (uint8_t i = 0; i < 2; ++i)
	{
		RegNum3x[i+2] = temp_K.buff[1 - i];
	}
	
	F_temp_code = ((int32_t)ADCfactorK_F * ADC + (int32_t)ADCfactorB_F) >> 16;
	temp_F.f_cell = (float)F_temp_code * 0.1f;
	
	for (uint8_t i = 0; i < 2; ++i)
	{
		RegNum3x[i + 4] = temp_F.buff[1 - i];
	}
}