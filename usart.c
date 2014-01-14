/*
 *  usart.h
 *  servo-serial
 *
 *  Created by Tomáš Pokorný on 30.12.10.
 *  Copyright 2010 Jaroška. All rights reserved.
 *
 */

//********* USART functions *******//
#define FOSC 16000000// Clock Speed
#define BAUD 38400
#define MYUBRR FOSC/16/BAUD-1

volatile CQUEUE USTxbuf;
volatile CQUEUE USRxbuf;

inline void USART_Init( unsigned int ubrr )
{
	
	/* Set baud rate */
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	/* Enable receiver and transmitter */
	UCSR0B = (1<<RXEN) | (1<<TXEN) | (1<<RXCIE0);
	/* Set frame format: 8data, 2stop bit */
	UCSR0C = /*(1<<USBS)|*/(3<<UCSZ0);
}
inline void USSendByte(unsigned char data)
{
	CQpush(data, &USTxbuf);
	UCSR0B |= 1<<UDRIE0;
	
}
void USSendString(char string[])
{
	unsigned char i;
	i=0;
	while (string[i]!=0) {
		CQpush(string[i], &USTxbuf);
		i++;
	}
	UCSR0B |= 1<<UDRIE0;
	
}
void USSendNumber(unsigned char cislo)
{
	unsigned char d1,d2;
	d1=cislo%10;
	cislo /=10;
	d2 = cislo%10;
	cislo /=10;
	CQpush((cislo%10)+'0',&USTxbuf);
	CQpush(d2+'0',&USTxbuf);
	CQpush(d1+'0',&USTxbuf);

}

ISR(USART0_UDRE_vect)
{
	unsigned char data;
	data = CQpop(&USTxbuf);
	UDR0 = data;
	if (USTxbuf.state==2) 
	{
		UCSR0B &= ~(1<<UDRIE0);
	}
}

inline unsigned char USGetByte()
{
	return CQpop(&USRxbuf);
}

inline unsigned char USisByte()
{
	return (USRxbuf.state==2)?0:1;
}

ISR(USART0_RX_vect)
{
	
	unsigned char data;
	data = UDR0;
	CQpush(data	, &USRxbuf);

	if ((data>32)&&(data<128))
	{
	USSendByte(data);
	}
	detectServoSetting(data); // Detecting data for servo
	detectBL(data); // Detecting STOP sequence
	
}
