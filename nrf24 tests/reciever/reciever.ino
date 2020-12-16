//Radio===============================================

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>


#define CE_PIN   9
#define CSN_PIN 10

const byte myAddress[5] = {'0','0','0','0','2'};
const byte masterAddress[5] = {'T','X','a','a','a'};

RF24 radio(CE_PIN, CSN_PIN); // Create a Radio


//package===============================================
typedef struct {
  float temp;
  float humidity;
  int16_t id = 00001;             
}package;

package data1;   // Package named data1 contains pktNo, rawPot, perPot, perCent & msg

//===============================================

char dataReceived[32] = {0}; // must match dataToSend in master
bool newData = false;

//Sensor===============================================

#include "DHT.h"
#define DHTPIN 2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);


//===============================================

void setup() {

    Serial.begin(9600);

    Serial.print("size: ");
    Serial.println(sizeof(data1));

    //==============Radio==============
    radio.begin();
    radio.setChannel(80);
    radio.setDataRate( RF24_250KBPS );
    radio.setPALevel(RF24_PA_MIN);

    //radio.setPayloadSize(16);

    radio.openWritingPipe(masterAddress); // NB these are swapped compared to the master
    radio.openReadingPipe(1, myAddress);

    radio.setRetries(5,15); // delay, count
    radio.startListening();

    
    //==============Sensnor==============
    dht.begin(); // initialize the sensor
}

//===============================================

void loop() {
  getData();
  showData();
  send();
}

//===============================================

void send() {
    if (newData == true) {
        radio.stopListening();
        bool rslt;
        data1.humidity=dht.readHumidity();
        data1.temp=dht.readTemperature();
        Serial.println(data1.temp);
        rslt = radio.write(&data1, sizeof(package));
        radio.startListening();

        Serial.print("Reply Sent ");
        //Serial.println(replyData);

        if (rslt) {
            Serial.println("Acknowledge Received");
        }
        else {
            Serial.println("Tx failed");
        }
        Serial.println();
        newData = false;
    }
}

//===============================================

void getData() {

    if ( radio.available() ) {
        radio.read( &dataReceived, sizeof(dataReceived) );
        //if ((char*)dataReceived=="Data Request"){
          newData = true;
        //}
    }
}

//===============================================

void showData() {
    if (newData == true) {
        Serial.print("Data received: ");
        Serial.println(dataReceived);
    }
}

//===============================================

byte int2byte(int Int, byte * Byte, int length){  
  String stringInt = String(Int);
  int sizeOfInt = stringInt.length();

  for(int i=0;i<5;i++){
    Byte[i]='0';
    }
  
  int lenByte = length-1;

  for(int i=0; i<sizeOfInt; i++){
    Byte[lenByte-i]=(char)stringInt[(sizeOfInt-1)-i];
    }
  }
