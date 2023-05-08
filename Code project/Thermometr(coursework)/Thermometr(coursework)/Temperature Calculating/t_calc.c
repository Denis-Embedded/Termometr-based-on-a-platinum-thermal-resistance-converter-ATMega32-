/*
 * t_calc.c
 *
 * Created: 06.05.2023 21:18:45
 *  Author: user4
 */ 


#include "t_calc.h"
#include "AVR_ModBus.h"
#include <string.h>

void get_temp(void)
{
	ADMUX=0b00000101;
	ADCSRA|=(1<<ADSC);		// запускаем АЦП ( аналогично ADCSRA=((ADCSRA|0b01000000));
	while ((ADCSRA&(1<<ADSC))!=0); //ожидаем конца преобразования АЦП
	C_temp_code = (ADCfactorK_C * ADC + ADCfactorB_C) >> 16;
	temp_C.f_cell = (float)C_temp_code * 0.1f;
	for (uint8_t i = 0; i < 2; ++i)
	{
		RegNum3x[i] = temp_C.buff[i];
	}
	
	
	K_temp_code = C_temp_code + 2731; //переменные X_temp_value хранят значение в 10 раз больше фактического
	temp_K.f_cell = (float)K_temp_code * 0.1f;
	for (uint8_t i = 0; i < 2; ++i)
	{
		RegNum3x[i+2] = temp_C.buff[i];
	}
	
	F_temp_code = (ADCfactorK_F * ADC + ADCfactorB_F) >> 16;
	temp_F.f_cell = (float)F_temp_code * 0.1f;
	
	for (uint8_t i = 0; i < 2; ++i)
	{
		RegNum3x[i + 4] = temp_C.buff[i];
	}
}