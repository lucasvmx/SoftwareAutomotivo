/*
	Código da ECU 1 - responsável por ler informações do motor
*/

#include <mcp_can.h>
#include <SPI.h>
#include "board_sender.h"

short speed_timer = 0;
short rpm_timer = 0;
int velocidade, rpm;
long mapped_speed, mapped_rpm;
static byte speedBytes[4];
static byte rpmBytes[4];
static byte data[4];
char msgString[128];

#define SPEED_SEND_INTERRUPT 2
#define RPM_SEND_INTERRUPT 3

MCP_CAN CAN0(10);     // Set CS to pin 10

void encodeLong(long n) {

  data[0] = (n >> 24) & 0xFF;
  data[1] = (n >> 16) & 0xFF;
  data[2] = (n >> 8) & 0xFF;
  data[3] = n & 0xFF;
}

void setup()
{
	// Configura o pino que representa o potenciometro
	pinMode(ECU1_AIN1, INPUT);
	pinMode(ECU1_AIN2, INPUT);
	pinMode(SPEED_SEND_INTERRUPT, INPUT);
	pinMode(RPM_SEND_INTERRUPT, INPUT);

	Serial.begin(115200);

	// Initialize MCP2515 running at 8MHz with a baudrate of 500kb/s and the masks and filters disabled.
	if(CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK) Serial.println("MCP2515 inicializado");
	else Serial.println("Falha ao inicializar o MCP2515...");

	CAN0.setMode(MCP_NORMAL);   // Change to normal mode to allow messages to be transmitted
}

byte sendSpeed()
{
	Serial.print("SPEED FRAME: ");
	for(byte i = 0; i<sizeof(speedBytes); i++){
		sprintf(msgString, " 0x%.2X", speedBytes[i]);
		Serial.print(msgString);
	}
	Serial.println("");

	return CAN0.sendMsgBuf(0x100, 0, 4, speedBytes);
}

byte sendEngineRpm()
{
	Serial.print("RPM FRAME: ");
	for(byte i = 0; i<sizeof(rpmBytes); i++){
		sprintf(msgString, " 0x%.2X", rpmBytes[i]);
		Serial.print(msgString);
	}
	Serial.println("");
	return CAN0.sendMsgBuf(0x100, 0, 4, rpmBytes);
}

void loop()
{
	rpm = analogRead(ECU1_AIN1);
	velocidade = analogRead(ECU1_AIN2);

	mapped_rpm = map(rpm, 0, 1023, 0, 8000);
	mapped_speed = map(velocidade, 0, 1023, 0, 220);

	if(rpm_timer == 5) {
		rpm_timer = 0;

		sprintf(msgString, "rpm: %ld", mapped_rpm);
		Serial.println(msgString);

		// enviar a rotação do motor
		encodeLong(mapped_rpm);
		memcpy(rpmBytes, data, 4);

		byte sndRpmStat = sendEngineRpm();

		if(sndRpmStat == CAN_OK){
		  Serial.println("rpm enviado com sucesso");
		} else {
		  Serial.println("falha ao enviar rotação do motor");
		}
	}

	if(speed_timer == 10) {
		speed_timer = 0;

		sprintf(msgString, "speed: %ld", mapped_speed);
		Serial.println(msgString);
		// enviar a velocidade
		encodeLong(mapped_speed);
		memcpy(speedBytes, data, 4);

		byte sndSpeedStat = sendSpeed();
		if(sndSpeedStat == CAN_OK){
		  Serial.println("velocidade enviada com sucesso");
		} else {
		  Serial.println("falha ao enviar velocidade");
		}
	}

	delay(1000);
	++speed_timer;
	++rpm_timer;
}
