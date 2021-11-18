/*
 * recepcionTrama.c
 *
 * Ejemplo en el cual se envia una cadena de caracteres que comineza con @ y finaliza con #
 * La cadena tiene formato JSON donde se posee un campo clave y un valor
 * Ejemplo de trama @{"Clave":"Valor"}#
 * En este ejemplo si se recibe @{"Integrante":"Edu"}# se responde con otra trama en formato JSON {"Nombre":"Eduardo Velazquez"}
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

char tramaRX[30]="@{\"Integrante\":\"edu\"}#";
char clave[30];
char valor[30]; 
char nombre[]="Eduardo Velazquez";
char noesnombre[]="no es un nombre";
void enviarTrama(char clave[],char valor[]);
unsigned char indice=0;
volatile unsigned char finRX=0;
int main(void)
{
    /* Configure USART en 9600,8,N,1 */
	UCSR0A=0;
	UCSR0B=(1<<TXEN0)|(1<<RXEN0)|(1<<RXCIE0);
	UCSR0C=(1<<UCSZ00)|(1<<UCSZ01);
	UBRR0=103;
    sei();
	while (1) 
    {
		if(finRX){
			unsigned char i=0;
			//reseteo de bandera de fin de recepción
			finRX=0;
			
			//determinación de posición en la cadena de {:}			
			char *aperturaLLave =strchr(tramaRX,'{');
			char *dosPuntos =strchr(tramaRX,':');
			char *cierreLLave =strchr(tramaRX,'}');
			//determinación de longitud de campo clave y valor
			unsigned char longitudClave=dosPuntos-aperturaLLave-3;
			unsigned char longitudValor=cierreLLave-dosPuntos-3;
			//copiamos los valores de clave y valor
			strncpy(clave,aperturaLLave+2,longitudClave);
			strncpy(valor,dosPuntos+2,longitudValor);
			//damos fin a las cadenas de caracteres clave y valor
			clave[longitudClave]='\0';
			valor[longitudValor]='\0';
			
			//envio de caracter de retorno de carro
			while(!(UCSR0A&(1<<UDRE0)));
			UDR0='\r';
			
			//comparación de string por medio de STRCMP
			if((!strcmp(valor,"Edu"))&&(!strcmp(clave,"Integrante"))){
				enviarTrama("Nombre","Eduardo Velazquez");
			}else{
				i=0;
				while(noesnombre[i]!='\0'){
					while(!(UCSR0A&(1<<UDRE0)));
					UDR0=noesnombre[i];
					i++;
				}
				
			}
		}
    }
}

ISR(USART_RX_vect){
	char datoRX;
	datoRX=UDR0;
	UDR0=datoRX;
	if(datoRX=='@'){
		indice=0;
	}
	tramaRX[indice]=datoRX;	
	if(datoRX=='#'){
		indice++;
		tramaRX[indice]='\0';
		finRX=1;
	}
	indice++;
	if(indice>=30)
		indice=0;
}



void enviarTrama(char clave[],char valor[]){
	char tramaTX[65];
	strcpy(tramaTX,"{\"");
	strcat(tramaTX,clave);
	strcat(tramaTX,"\":\"");
	strcat(tramaTX,valor);
	strcat(tramaTX,"\"}");
	unsigned char i=0;
	while(tramaTX[i]!='\0'){
		while(!(UCSR0A&(1<<UDRE0)));
		UDR0=tramaTX[i];
		i++;
	}
	while(!(UCSR0A&(1<<UDRE0)));
	UDR0='\r';
	
}