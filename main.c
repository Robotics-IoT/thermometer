/*----------------------------------------------------------------
 * File:        TEMP.C
 *
 * Purpose:     To display Temperature  between 2 to 100 Degree C
 *
 * Inputs:      LM35
 *             
 * Outputs:     Three seven segment common Cathod LED Display
 *
 * LM35 gives 10mv for a 1 degree temperature change
 *
 * Author: C.V. Niras, vu3cns@yahoo.com 
 *-----------------------------------------------------------------*/

#include <pic.h>
 
__CONFIG(WDTDIS & XT & UNPROTECT &PWRTEN & LVPDIS);

#define	CH0	0b00000000	// AN0
#define	CH1	0b00001000	// AN1
#define	CH2	0b00010000	// AN2
#define	CH3	0b00011000	// AN3
#define	CH4	0b00100000	// AN4 

#define ADCON0_VALUE 0b11000001 // RC OSC, ADON

volatile static unsigned char digit[3];
//volatile static unsigned char a, b, c;
void adc_delay(void);
void set_adc_ch(unsigned char channel);
unsigned int adc_read(void);
union
{
	unsigned int i;
	unsigned char ch[2];
}adc;

void interrupt timer (void);
void DelayMs(unsigned char cnt);
//float floor (float f);

#define	MHZ	*1000L			
#define	KHZ	*1
#define	DelayUs(x)	{ unsigned char _dcnt; \
			  _dcnt = (x)/((12MHZ)/(4MHZ))|1; \
			  while(--_dcnt != 0) \
				  continue; }

void main()
{
	unsigned int temp, temp_real, temp1, i;
	volatile static char channel;
	
	ADCON1 = 0x82;		// RA's are input and RE's are digital output
	TRISA = 0x1F;		// all are input 
	TRISB = 0x00;		// All portB are output
	TRISC = 0x00;		// All PortC are out put
	PORTC = 0x00;
	TRISE = 0x00;
 	OPTION = 0x04;		/* Prescale of 32. */
 						/* which is really Fosc/4*32. */
  	INTCON = 0;
 	T0IE = 1;
 	GIE = 1;
 	
	for( ; ;)
	{
		if ( RA4 )
		{	
			channel = CH0;
		}
		else
		{
			channel = CH1; 
		}
		set_adc_ch(channel);
		adc_delay();
		
		temp = adc_read();
		temp += adc_read();
		temp += adc_read();
		temp += adc_read();
	
		temp_real = (int) (( temp * 1.2207 ) + 0.5);
		temp1 = temp_real;
		
		if(temp1 > 1000)
		{
			digit[0] = digit[1] = digit[2] = 10;	// display error i.e."E"
		}
		else
		{
			for ( i = 0; i < 3; i++)
			{
				digit[i] = temp1 % 10;
				temp1 = temp1 / 10;
			}
		}
		// Temperature control for devices (X10, i.e 390 is 39.0)
		if ( temp_real > 390 ) 
		RC0 = 1 ;
		else 
		RC0 = 0 ;
		if ( temp_real > 400 ) 
		RC1 = 1 ;
		else 
		RC1 = 0 ;
		if ( temp_real > 420 ) 
		RC2 = 1 ;
		else 
		RC2 = 0 ;
		if ( temp_real > 500 ) 
		RC3 = 1 ;
		else 
		RC3 = 0 ;
		if ( temp_real < 350 ) 
		RC4 = 1 ;
		else 
		RC4 = 0 ;
		if ( temp_real < 320 ) 
		RC5 = 1 ;
		else 
		RC5 = 0 ;
		if ( temp_real < 300 ) 
		RC6 = 1 ;
		else 
		RC6 = 0 ;
		if ( temp_real < 250 ) 
		RC7 = 1 ;
		else 
		RC7 = 0 ;
		DelayMs(200);
	}
}
void adc_delay(void)
{
	//19 Cycles
	char n;
	for(n=0; n<2; n++)
	{
		NOP();
	}
}

void set_adc_ch(unsigned char channel)
{
	ADCON0 = channel | ADCON0_VALUE;
}

unsigned int adc_read(void)
{
	ADGO = 1;				// Start Conversion, 1 cycles
	NOP();
	while(ADGO);			// wait for conversion complete
	adc.ch[0]	= ADRESL;
	adc.ch[1]	= ADRESH;
	return adc.i;
}

void interrupt timer (void)
{
	static unsigned char display[11] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x79};
	static unsigned char d, i;
	if ( i > 2 )
	{
		d = 0b1111011;		// Only one Common Cathode display ON
		i = 0;
	}
	PORTE = 0xFF;			// Off display
	PORTB = display[digit[i]];
	PORTE = d;				// Turn on curresponding segment
	
	i += 1;
	d = d >> 1;
	TMR0 = 0;
	T0IF = 0; 
}
	
void DelayMs(unsigned char cnt)
{
	unsigned char	i;
	do {
		i = 4;
		do {
			DelayUs(250);
		} while(--i);
	} while(--cnt);
}