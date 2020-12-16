#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

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



#define CE_PIN   9
#define CSN_PIN 10
RF24 radio(CE_PIN, CSN_PIN); // Create a Radio

const uint64_t pipes[2] = { 
  0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

void setup()
{
  Serial.begin(57600);
  radio.begin();
  radio.setChannel(80);
    radio.setDataRate( RF24_250KBPS );
    radio.setPALevel(RF24_PA_MIN);

  radio.openWritingPipe(pipes[1]);
  radio.openReadingPipe(1,pipes[0]);
  radio.startListening();
  Serial.println(sizeof(A_t));
  Serial.println(sizeof(B_t));
  
}

void loop(void)
{ 
  // we need data to sent...
  duino1.A = random(0,20);
  duino1.B = random(0,20);
  duino1.C = random(0.0,20.0);
  duino1.D = random(0.0,20.0);
  // end of analog reading

  // radio stuff
  if ( radio.available() )
  {
    bool done = false;
    while(radio.available()){
    radio.read( &duino2, sizeof(duino2) );
    }
    radio.stopListening();
    radio.write( &duino1, sizeof(duino1) );
    radio.startListening();
  }
  // end of radio stuff

  // serial print received data 
  Serial.print("duino2.W = ");
  Serial.println(duino2.W);
  Serial.print("duino2.X = ");
  Serial.println(duino2.X);
  Serial.print("duino2.Y = ");
  Serial.println(duino2.Y);
  Serial.print("duino2.Z = ");
  Serial.println(duino2.Z);
  // end of serial printing
}
