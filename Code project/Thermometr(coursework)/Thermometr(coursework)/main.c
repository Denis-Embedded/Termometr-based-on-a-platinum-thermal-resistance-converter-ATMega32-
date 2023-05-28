/*
 * Thermometr(coursework).c
 *
 * Created: 01.05.2023 20:41:29
 * Author : user4
 */ 

#define F_CPU 8000000UL


#include <avr/pgmspace.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "Modbus/AVR_ModBus.h"
#include "GLCD/glcd.h"
// #include "GLCD/KS0108.h"
// #include "GLCD/Tahoma11x13.h"
#include "Temperature Calculating/t_calc.h"


const unsigned char paw[] PROGMEM =
{
	0xff, 0xff, 0xf0, 0xff, 0xff, 0xf0, 0xff, 0xef, 0xf0, 0xff, 0xcf, 0xf0, 0xfe, 0x4f, 0xf0, 0xfe,
	0x4b, 0xf0, 0xfe, 0x43, 0xf0, 0xfe, 0x53, 0xf0, 0xfe, 0x63, 0xf0, 0xff, 0x23, 0xf0, 0xfe, 0x1f,
	0xf0, 0xfc, 0x13, 0xf0, 0xfc, 0x13, 0xf0, 0xfc, 0x03, 0xf0, 0xfe, 0x03, 0xf0, 0xff, 0x17, 0xf0,
	0xff, 0x9f, 0xf0, 0xff, 0xbf, 0xf0, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xf0
};



extern volatile int16_t C_temp_code;
extern volatile int16_t K_temp_code;
extern volatile int16_t F_temp_code;

#define base 20 //������ �� ���� �������

volatile uint8_t g_key = 0;			//����� ������� �������
volatile uint8_t g_key_status = 0;	//���� ��� 7 ����� 1, ������ ������� ������.
							//���� ��� 6 ����� 1, ������ ������� �� ��������
							
volatile uint8_t mode = 0; //����� ����������� �����������
/*
1. mode == 0 - ����������� � �������� �������
2. mode == 1 - ����������� � ���������
3. mode == 2 - ����������� � �������� ����������
*/

volatile uint8_t upd_flag = 0; //����, ��������������� �� ���������� ������. ����������� ������� ����

volatile uint8_t TCI_counts = 0; //���������� ���������� (����� ��� ������������ ������� ������ �������)                                                  


void glcd_init(void);
void draw_loading_screen();
void draw_main_screen(void);
void display_result(void);

void IO_init(void);
void timer_init(void);
void ADC_init(void);
void scan_key(void);

void draw_round_rectangle(uint8_t x1, uint8_t x2, uint8_t y1, uint8_t y2, uint8_t radius);
int main(void)
{
    asm("cli");
	/* Replace with your application code */
	//InitModBus();
	IO_init();
	timer_init();
	ADC_init();
	InitModBus();
	glcd_init();

	draw_main_screen();
	draw_loading_screen();
		
	asm("sei");

    while (1) 
    {
		if(upd_flag)
		{
			scan_key();
			get_temp();
			display_result();		
			TCCR2=0b00001110;
			upd_flag = 0;
		}
		
		if ((g_key_status &0b10000000) != 0)
		{
			g_key_status &= 0b01000000;
			switch (g_key)
			{
				case 0:
				{
					mode = 0;
					//glcd_putchar((176),base+8*10,1,2,1); //������ ��������
					glcd_puts("�C",base+8*9,1,0,1,1);
					break;
				}
				case 1:
				{
					mode = 1;
					glcd_puts(" K", base+8*9,1,0,1,1);
					//glcd_putchar(' ', base+8*11,1,1,1);
					break;
				}
				case 2:
				{
					mode = 2;
					//glcd_putchar((176),base+8*10,1,2,1); //������ ��������
					glcd_puts("�F",base+8*9,1,0,1,1);
					break;
				}
			}
		}
		CheckModBus();
		
    }
}

void IO_init(void)
{
	DDRA |=0b00001100;  //������������� ������
	PORTA |=0b00000100;	//��� ��������� ������ ����������
	
	DDRB |=0b11100011;  
	PORTB |=0b00000000;
	
	DDRD |=0b11000000;
	PORTD |=0b10000000;
	
	DDRC |=0b11111111;
}

void timer_init(void)
{
	//��������� �� ������������ �/�2 � ���������� 8 ��
	TCNT2=0b00000000;	//������� �/�2
	OCR2=249;			//������ ��������� 249 � ������� ��������� �/�2
	TCCR2=0b00001110;	//����� "����� ��� ����������" � �������� �� 256
	TIMSK|=0b10000000;  //���������� ���������� �� ���������� �� �/�2
	TIFR &=0b11000000;   //������� ������ ���������� �/�2
}

void ADC_init(void)
{
	ADMUX = 0b00000000; //������������ 2 ������ ��� - ADC0 � ADC5
	ADCSRA = 0b10000111; //ADEN | ������������ ��� 128
};

//�������, ����������� ��������������� ������������������ ������������� �������
void glcd_init(void)
{
	_delay_ms(100);
	glcd_off();			//��������� �������
	_delay_ms(100);
	glcd_on();			//�������� ����� ��������
	glcd_clear();		//������� �������
}

//������� ��������� ��������
void draw_main_screen()
{
	//rectangle(0,0,126,62,0,1);	//��������� �������� �������
	draw_round_rectangle(0,0,126,62, 8);
	// ����� ��������������� ����������� "������"
	draw_round_rectangle(5,35,40,59, 8);
	draw_round_rectangle(86,35,121,59,8);
	draw_round_rectangle(45,35,81,59,8);
	// ����� ��������� ��������
	
// 	point_at(15, 5, 1);
// 	point_at(15, 17, 1);
// 	point_at(92+21, 5, 1);
// 	point_at((92+21), 17, 1);
	
	draw_round_rectangle(15,5,113,17, 6);
	glcd_puts("T=",base,1,0,1,1);
	glcd_puts("�C", base + 8 *9, 1, 0, 1, 1);
	// ��������� ����� � ����� ����������� "������"
	glcd_puts("C",16,5,0,2,1);
	glcd_puts("K",57,5,0,2,1);
	glcd_puts("F",97,5,0,2,1);
}

void display_result(void)
{
		int16_t code_temp = 0;

		switch(mode)
		{
			case 0:
			{
				code_temp = C_temp_code;
				break;
			}
			case 1: 
			{
				code_temp = K_temp_code;
				break;				
			}
			case 2:
			{
				code_temp = F_temp_code;
				break;
			}
		}
		
		if (code_temp < 0)
		{
			glcd_putchar('-', base+8*3, 1, 1, 1);
			code_temp = -code_temp;
		}
		else
		{
			glcd_putchar(' ', base+8*3, 1, 1, 1);
		}
		
		uint8_t nums[4] = {0,};
		for (int8_t i = 3; i > -1; --i) //����������� ����� � ������. � ������ [0] ����� ������� ������
		{
			nums[i] = code_temp % 10 + 48;
			code_temp = code_temp / 10;
		}
		
		uint8_t cursor = 4;//������� ���������� ������������ �������
		uint8_t count = 0; //���������� ���������� ��������
		while(count < 2)
		{
			if (nums[count]== 48)
			{
				++count;				//����������� ������� �� ������� ���������� �������� ��� �� ����
			}
			else
			{
				break; 
			}
		}
		
		for (uint8_t i = count; i < 3; ++i)
		{
			glcd_putchar(nums[i], base+8*(cursor++), 1, 1, 1);
		}
// 		for (uint8_t i = 0; i < 3; ++i)
// 		{
// 			glcd_putchar(nums[i], base + 8 *(4 + i), 1,0,1);
// 			
// 		}
		//glcd_putchar('.', base + 8*(cursor++), 1, 1, 1);   //�������������� �����
/*		glcd_putchar('.', base + 8*7, 1,0,1);*/
//		glcd_putchar(nums[3], base + 8 * 8, 1, 0, 1);
		glcd_putchar('.', base + 8 * (cursor++), 1,0,1);
		glcd_putchar(nums[3], base + 8*(cursor++), 1, 0, 1); 
		for (uint8_t i = 0; i < count; ++i)
		{
			glcd_putchar(' ', base + 8*(cursor++), 1,0,1); //��������� ��������� ���������� �������, ���� ��� ����
		}
		
}


void scan_key(void)
{
	PORTA=0b00000100;       //
	_delay_ms(2);			// �������� !!! (���������� ������� ������ ����������)
	ADMUX=0b00000000;		// �������� ������� ����� ���
	ADCSRA|=(1<<ADSC);		// ��������� ��� ( ���������� ADCSRA=((ADCSRA|0b01000000));
	while ((ADCSRA&(1<<ADSC))!=0); //������� ����� �������������� ���
	// ���������� ���������� �� ����������� �������
	volatile uint16_t x_coordinate=ADC;
	PORTA=0b00001000;
	_delay_ms(2);
	ADMUX=0b00000001;
	ADCSRA|=(1<<ADSC);		// ��������� ��� ( ���������� ADCSRA=((ADCSRA|0b01000000));
	while ((ADCSRA&(1<<ADSC))!=0); //������� ����� �������������� ���
	// ���������� ���������� �� ����������� �������
	volatile uint16_t y_coordinate=ADC;
	if (((y_coordinate>190)&&(y_coordinate<370))&&((x_coordinate>80)&&(x_coordinate<260))&&(g_key_status==0)){
		g_key_status=0b11000000;
		g_key=0;
	}
	if (((y_coordinate>190)&&(y_coordinate<370))&&((x_coordinate>330)&&(x_coordinate<490))&&(g_key_status==0)){
		g_key_status=0b11000000;
		g_key=1;
	}
	if (((y_coordinate>190)&&(y_coordinate<370))&&((x_coordinate>565)&&(x_coordinate<730))&&(g_key_status==0)){
		g_key_status=0b11000000;
		g_key=2;
	}
	if (((y_coordinate<20)&&(x_coordinate<20))&&(g_key_status&0b01000000)!=0){
		g_key_status=g_key_status&0b10000000;
	}

}

ISR(TIMER2_COMP_vect)
{
	++TCI_counts;
	
	if(TCI_counts == 10)
	{
		TCI_counts = 0;
		TCCR2=0b00000000;

		upd_flag = 1;
	}
}

void draw_loading_screen()
{
	glcd_puts("", )
}

void draw_round_rectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t radius)
{
	int16_t tSwitch = 3 - 2 * radius;
	uint8_t width, height, x, y;
	width = x2-x1;
	height = y2-y1;
	x = 0;
	y = radius;
	
	h_line(x1 + radius, y1, width - 2 * radius, 0, 1);
	h_line(x1 + radius, y2, width - 2 * radius, 0, 1);
	v_line(x1, y1 + radius, height - 2 * radius, 0, 1);
	v_line(x2, y1 + radius, height - 2 * radius, 0, 1);
	 while (x <= y)
	 {
		 point_at(x1 + radius - x, y1 + radius - y, 1);
		 point_at (x1 + radius - y, y1 + radius - x, 1);
		 
		 point_at(x1 + width - radius + x, y1 + radius - y, 1);
		 point_at (x1 + width - radius + y, y1 + radius - x, 1);
		 
		 point_at(x1 + radius - x, y1 + height - radius + y, 1);
		 point_at (x1 + radius - y, y1 + height - radius + x, 1);
		 
		 point_at(x1 + width - radius + x, y1 + height - radius + y, 1);
		 point_at (x1 + width - radius + y, y1 + height - radius + x, 1);	
		 
		if (tSwitch < 0)
		{
			tSwitch += 4 * x + 6;
		}
		else
		{
			tSwitch += 4 * (x-y) + 10;
			--y;
		}
		++x;
	 }
	 

}