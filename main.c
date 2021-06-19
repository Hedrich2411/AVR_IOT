/*
 

 */

#define F_CPU 16000000UL

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

float v1,v2;
char cadena1[20];
char dato;

void ADCC_Init(void);
uint16_t Analagic_Read(uint8_t adc_channel);
void Uart_Init(void);
void Uart_PutChar(char Ch);
void Uart_PutString(const char *str);


int main(void) {
    
    cli();
    //PortB as output
    DDRB = 0xFF;
    ADCC_Init();
    Uart_Init();
    sei();
    
    while (1) {
        
        
        v1 = ((float)Analagic_Read(ADC0D))*(5.0/1023.0);
        v2 = ((float)Analagic_Read(ADC1D))*(5.0/1023.0);
       
        
        sprintf(cadena1,"%.1f-%.1f\n",v1,v2);
        Uart_PutString(cadena1);
        _delay_ms(500);
        
    }
    return 0;
}



void ADCC_Init(void){
    
    //ADC VREF: VCC -> 5V del MCU
	ADMUX &= ~(1<<REFS1);
	ADMUX |= (1<<REFS0);
	//Right justified conversion
	ADMUX &= ~(1<<ADLAR);
	
	//Initially the ADC is Off 
	ADCSRA &= ~(1<<ADEN);
	//Disable Auto Shot 
	ADCSRA &= ~(1<<ADATE);
	//Eligiendo el prescaler
	/*
	ADC_clock = 50Khz - 200Khz
	
	ADC_clock = F_CPU / 2
	ADC_clock = 8Mhz
	
	PRESCALER = 128  -> 111
	ADC_clock = 16Mhz / 128
	ADC_clock = 125Khz
	*/
	ADCSRA |= (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); //pre = 128
	//Disable the digital input buffer 
	DIDR0 = 0xFF;

}


uint16_t Analagic_Read(uint8_t adc_channel){
  
    ADMUX = ( (ADMUX & 0xF0) | (0x0F & adc_channel) ); //Channel selection 
    ADCSRA |= (1<<ADEN)|(1<<ADSC); //Start the conversion 
    while((ADCSRA & (1<<ADSC))!=0);//Wait for the conversion to finish 
    ADCSRA &= ~(1<<ADEN);//Turn off the Adc 
    return ADC;
    
}


void Uart_Init(void){
    //USART in UART mode 
    
    UCSR0C &=~(1<<UMSEL00);
    UCSR0C &=~(1<<UMSEL01);
    
    //Deactivate Parity 
    
    UCSR0C &=~ (1<<UPM00);
	UCSR0C &=~ (1<<UPM01);
    
    //Stop bit 1 
    UCSR0C &=~ (1<<USBS0);
    
    //8 bit data 
	UCSR0C |=  (1<<UCSZ00);
	UCSR0C |=  (1<<UCSZ01);
	UCSR0B &=~ (1<<UCSZ02);
    
    
    // Baudrate calculation 
	UCSR0A |= (1<<U2X0);
	UBRR0 = (F_CPU/8/9600) - 1;

	UCSR0B |= (1<<TXEN0);
	UCSR0B |= (1<<RXEN0);

	UCSR0B |= (1<<RXCIE0);

}

void Uart_PutChar(char Ch){
  
    while(!(UCSR0A & (1<<UDRE0)));
    UDR0=Ch;
}

void Uart_PutString(const char *str){
   
    while(*str)
        Uart_PutChar(*str++);
    
}


ISR(USART_RX_vect){
    
      dato = UDR0;
      while(!(UCSR0A & (1<<UDRE0)));
      
      switch(dato){
      
          case '1':
              PORTB &=~(1<<PORTB0);
              break;
          case '2':
              PORTB |=(1<<PORTB0);
              break;
          case '3':
              PORTB &=~(1<<PORTB1);
              break;
          case '4':
              PORTB |=(1<<PORTB1);
              break;
            
      }
   
   
}

