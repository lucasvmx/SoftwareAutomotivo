/*
	Código da ECU 1 - responsável por ler informações do motor
*/

#include <mcp_can.h>
#include <SPI.h>
#include "Board.h"

int velocidade, rpm;
byte speedBytes[4];
byte rpmBytes[4];
byte data[4];

#define SPEED_SEND_INTERRUPT 2
#define RPM_SEND_INTERRUPT 3

byte *encodeLong(unsigned long n) {
  // Zera o buffer
  memset(data, 0, 4);

  data[0] = (n >> 24) & 0xFF;
  data[1] = (n >> 16) & 0xFF;
  data[2] = (n >> 8) & 0xFF;
  data[3] = n & 0xFF;

  return &data[0];
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

  attachInterrupt(digitalPinToInterrupt(SPEED_SEND_INTERRUPT), CHANGE);
  attachInterrupt(digitalPinToInterrupt(RPM_SEND_INTERRUPT), CHANGE);
}

void loop()
{	
  rpm = analogRead(ECU1_AIN1);
	velocidade = analogRead(ECU1_AIN2);
	
	rpm = map(rpm, 0, 1023, 0, 8000);
	velocidade = map(velocidade, 0, 1023, 0, 220);

  encodeLong((unsigned long)velocidade);
  memcpy(speedBytes, data);
  encodeLong((unsigned long)rpm);
  memcpy(rpmBytes, data);

  // send data:  ID = 0x100, Standard CAN Frame, Data length = 8 bytes, 'data' = array of data bytes to send
  byte sndStat = CAN0.sendMsgBuf(0x100, 0, 4, data);
  if(sndStat == CAN_OK){
    Serial.println("Message Sent Successfully!");
  } else {
    Serial.println("Error Sending Message...");
  }
  delay(100);   // send data per 100ms
}
