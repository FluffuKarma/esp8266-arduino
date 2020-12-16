#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

#define CE_PIN D0
#define CSN_PIN D1

RF24 radio(CE_PIN, CSN_PIN); // Create a Radio
const uint64_t pipes[2] = { 
  0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };


typedef struct{
  float C;
  float D;
  int8_t A;
  int8_t B;
}
A_t;

typedef struct{
  float Y;
  float Z;
  int8_t W;
  int8_t X;
}
B_t;

A_t duino1;  
B_t duino2; 

void setup()
{
  Serial.begin(57600);
  radio.begin();
  radio.setChannel(80);
    radio.setDataRate( RF24_250KBPS );
    radio.setPALevel(RF24_PA_MIN);

  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1,pipes[1]);

  Serial.println(sizeof(A_t));
  Serial.println(sizeof(B_t));
}

void loop()
{
  Serial.println("");
  // we need data to sent...
  duino2.W = random(0,20);
  duino2.X = random(0,20);
  duino2.Y = random(0,20);
  duino2.Z = random(0,20);
  // end of analog reading

  // radio stuff
  radio.stopListening();
  bool ok = radio.write( &duino2, sizeof(duino2) );
  radio.startListening();

  unsigned long started_waiting_at = millis();
  bool timeout = false;
  while ( ! radio.available() && ! timeout ){
    if (millis() - started_waiting_at > 250 ){
      timeout = true;}
  }
  if ( timeout )
  {
    Serial.println("Failed, response timed out.");
  }
  else
  {
    radio.read( &duino1, sizeof(duino1) );
  }
  // end of radio stuff

  // serial print received data
  Serial.print("duino1.A = ");
  Serial.println(duino1.A);
  Serial.print("duino1.B = ");
  Serial.println(duino1.B);
  Serial.print("duino1.C = ");
  Serial.println(duino1.C);
  Serial.print("duino1.D = ");
  Serial.println(duino1.D);
  // end of serial printing

  delay(2000);
}
