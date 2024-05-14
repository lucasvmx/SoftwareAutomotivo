/*
	Ler o potenciometro
*/

#ifndef DESKTOP
	#include "Board.h"
#else
	#include <stdio.h>
	#include <stdlib.h>
	#include <windows.h>

	long map(long x, long in_min, long in_max, long out_min, long out_max);
#endif

int velocidade, rpm;

void setup()
{
#ifndef DESKTOP
	// Configura o pino que representa o potenci�metro
	pinMode(ECU1_AIN1, INPUT);
	Serial.begin(9600);
#endif
}

void loop()
{
	// L� o valor na porta
#ifndef DESKTOP
	velocidade = analogRead(ECU1_AIN1);

	if(Serial.available() > 0)
	{
		String incoming = Serial.readString();


	}
#else
	srand(GetTickCount());
	velocidade = rand() % 1023 + 1;
	printf("valor lido da porta analogica: %ld\n", velocidade);
#endif
	rpm = velocidade;

	rpm = map(rpm, 0, 1023, 0, 8000);
	velocidade = map(velocidade, 0, 1023, 0, 220);

#ifdef DESKTOP
	printf("RPM %ld\nVelocidade do ve�culo: %ld\n\n", rpm, velocidade);
	Sleep(5000);
#endif
}

#ifdef DESKTOP

long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int main()
{
	setup();

	while(true) {
		loop();
	}
}
#endif