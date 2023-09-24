/*Include the needed libraries*/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


/* Global variables for timekeeping */
unsigned char SEC1 = 0;
unsigned char SEC2 = 0;
unsigned char MIN1 = 0;
unsigned char MIN2 = 0;
unsigned char HOUR1 = 0;
unsigned char HOUR2 = 0;

volatile uint8_t updateDisplayFlag = 0;
volatile uint8_t currentDisplay = 0;

unsigned char SECT1 = 0;
unsigned char SECT2 = 0;
unsigned char MINT1 = 0;
unsigned char MINT2 = 0;

/* External INT0 Interrupt Service Routine */
ISR(INT0_vect)
{
	currentDisplay = 1;

	SECT1 = SEC1;
	SECT2 = SEC2;
	MINT1 = MIN1;
	MINT2= MIN2;

	SEC1 =   0;
	SEC2 =   0;
	MIN1 =   0;
	MIN2 =   0;
	HOUR1=   0;
	HOUR2=   0;

}
/* External INT1 Interrupt Service Routine */


ISR(INT1_vect)
{
	TCCR1B &= (~((1<<CS12)|(1<<CS11)|(1<<CS10))) ;
}
/* External INT2 Interrupt Service Routine */


ISR(INT2_vect)
{
	TCCR1B = (1<<WGM12) | (1<<CS12) | (1<<CS10);
}


/* Interrupt Service Routine for Timer1 compare mode */
ISR(TIMER1_COMPA_vect)
{
	SEC1++;
	if (SEC1 > 9)
	{
		SEC1 = 0;
		SEC2++;
		if (SEC2 > 5)
		{
			SEC2 = 0;
			MIN1++;
			if (MIN1 > 9)
			{
				MIN1 = 0;
				MIN2++;
				if (MIN2 > 5)
				{
					MIN2 = 0;
					HOUR1++;
					if (HOUR1 > 9)
					{
						HOUR1 = 0;
						HOUR2++;
						if (HOUR2 > 9)
						{

							HOUR2 = 0;
						}
					}
				}
			}
		}
	}

	updateDisplayFlag = 1; // Set the flag to update the display
}

/* External INT0 enable and configuration function */
void INT0_Init(void)
{
	DDRD  &= ~(1 << PD2);                 // Configure INT0/PD2 as input pin
	PORTD |= (1<<PD2);					 //  Connect the push button with the internal pull-up resistor.
	MCUCR |= (1 << ISC01);				// Trigger INT0 with the Falling edge
	GICR  |= (1 << INT0);              // Enable external interrupt pin INT0

}
/* External INT1 enable and configuration function */



void INT1_Init(void)
{
	DDRD &= ~(1 << PD3);                 				  // Configure INT1/PD3 as input pin
	MCUCR |= (1 << ISC11) | (1 << ISC10);				 // Trigger INT1 with the Rising edge
	GICR |= (1 << INT1);                				// Enable external interrupt pin INT1

}
/* External INT2 enable and configuration function */



void INT2_Init(void)
{
	DDRB &= ~(1 << PB2);                  // Configure INT2/PB2 as input pin
	PORTB |= (1<<PB2);					 //  Connect the push button with the internal pull-up resistor.
	MCUCR |= (1 << ISC01);				// Trigger INT0 with the Falling edge
	GICR |= (1 << INT2);               // Enable external interrupt pin INT2

}
/* Timer1 enable and configuration function */



void Timer1_Init_CTC_Mode(void)
{
	TCNT1 = 0;    // Set Timer initial value to 0
	OCR1A  = 644; // Set Compare Value
	TIMSK |= (1<<OCIE1A); /* Enable Timer1 Compare A Interrupt */

	/* Configure Timer1 control register:
	 * 1. Non-PWM mode, FOC1A and FOC1B set to 1 for normal port operation.
	 * 2. CTC Mode with WGM12=1, WGM13=0, WGM10=0, and WGM11=0.
	 * 3. No need for OC1A/B, so COM01=0 and COM00=0.
	 * 4. Clock source set to F_CPU/1024 with CS12=1, CS11=1, and CS10=1.
	 *
	 * Description of COM1A1/COM1B1 and COM1A0/COM1B0:
	 * 0 0 Normal port operation, OC1A/OC1B disconnected.
	 */

	TCCR1A = (1<<FOC1A) | (1 << FOC1B) ;
	TCCR1B = (1<<WGM12) | (1<<CS12) | (1<<CS10);
}

int main(void)
{

	DDRA |= 0x3F; // Set the first 6 pins of PORTA (PA0-PA5) as output pins
	PORTA = 0x3F; // Enable the first 7-segment display
	DDRC |= 0x0F; // configure first four pins of PORTC as output pins
	PORTC &= 0xF0; // initialize the 7-segment with value 0 by clear the first four bits in PORTC
	DDRD |= 0xF0;  // Configure the last 4-pins of PORTD as output
	PORTD &= 0xF0;
	DDRB |= 0x78;   // Configure pins in PORT B as enable/disable pins (Output pins)
	SREG |= (1<<7);// Enable the Global interrupt Pin(I-bit)
	Timer1_Init_CTC_Mode();
	INT0_Init();
	INT1_Init();
	INT2_Init();

	while (1)
	{
		if (updateDisplayFlag)
		{

			// Display SEC1
			PORTA = 0x01;
			PORTC = (PORTC & 0xF0) | (SEC1 & 0x0F);
			_delay_us(1);

			// Display SEC2
			PORTA = 0x02;
			PORTC = (PORTC & 0xF0) | (SEC2 & 0x0F);
			_delay_us(1);

			// Display MIN1
			PORTA = 0x04;
			PORTC = (PORTC & 0xF0) | (MIN1 & 0x0F);
			_delay_us(1);

			// Display MIN2
			PORTA = 0x08;
			PORTC = (PORTC & 0xF0) | (MIN2 & 0x0F);
			_delay_us(1);
			// Display HOUR1
			PORTA = 0x10;
			PORTC = (PORTC & 0xF0) | (HOUR1 & 0x0F);
			_delay_us(1);

			// Display HOUR2
			PORTA = 0x20;
			PORTC = (PORTC & 0xF0) | (HOUR2 & 0x0F);
			_delay_us(1);


			if(currentDisplay)
			{


				// Display SEC1
				PORTB = 0x08;
				//SECT1 = (SECT1<<4);
				//PORTD = SECT1;

				PORTD = (PORTD & 0x0F) | ((SECT1<<4) & 0xF0);
				_delay_us(1);

				// Display SEC2
				PORTB = 0x10;
				PORTD = (PORTD & 0x0F) | ((SECT2<<4) & 0xF0);
				_delay_us(1);

				// Display MIN1
				PORTB = 0x20;
				PORTD = (PORTD & 0x0F) | ((MINT1<<4) & 0xF0);
				_delay_us(1);

				// Display MIN2
				PORTB = 0x40;
				PORTD = (PORTD & 0x0F) | ((MINT2<<4) & 0xF0);
				_delay_us(1);

			}
		}
	}


	return 0;
}
