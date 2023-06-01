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

#define ADCfactorK_C 512500LL		//коэффициенты для целочисленного вычисления 
#define ADCfactorB_C -131072000LL	//значения температуры в градусах цельсия

#define ADCfactorK_F 922500LL		//Коэффициенты для целочисленного вычисления
#define ADCfactorB_F -214958080LL	//значения температуры в градусах Фаренгейта

union temperature_t
{
	float f_cell;			//Часть объединения, в которое нужно записать 
					//вещественное значение температуры для передачи
					//по протоколу ModBus RTU
	uint16_t buff[2]; 		//Это поле нужно для передачи всего значения с помощью
					//обработчика Modbus RTU
};

//инициализация структур данных для хранения температуры
extern volatile union temperature_t temp_C;
extern volatile union temperature_t temp_K;
extern volatile union temperature_t temp_F;

//переменные для вывода на графический индикатор
extern volatile int16_t C_temp_code;
extern volatile int16_t K_temp_code;
extern volatile int16_t F_temp_code;

void get_temp(void); //Прототип функции измерения температуры
#endif /* T_CALC_H_ */