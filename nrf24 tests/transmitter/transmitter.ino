//===============================================

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#include <WiFiUdp.h>
#include <NTPClient.h>


#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <WiFiClientSecureBearSSL.h>

//===============================================
//Wifi

const char* ssid = "BELL695";
const char* password = "47515EDDFEF4";

//Your Domain name with URL path or IP address with path
const char* host = "http://192.168.2.246:80/send_data";

//===============================================
//Radio

#define CE_PIN D0
#define CSN_PIN D1

byte addresses[20][5] = {{'0','0','0','0','2'}};
const int maxIds=sizeof(addresses)/sizeof(addresses[0]);

const byte masterAddress[5] = {'T','X','a','a','a'};

typedef struct {
  float temp;
  float humidity;
  int16_t id = NULL;             
}package;

package data1;   // Package named data1

RF24 radio(CE_PIN, CSN_PIN); // Create a Radio

char dataToSend[] = "Data Request";

const int waitTime = 1000;
char rx_byte = 0;

//===============================================
//time shit
const long utcOffsetInSeconds = -4*60*60;

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

//===============================================

void setup() {

    Serial.begin(9600);

    Serial.println("Starting");
    Serial.println(sizeof(data1));

    //Radio setup

    radio.begin();
    radio.setChannel(80);
    radio.setDataRate( RF24_250KBPS );
    radio.setPALevel(RF24_PA_MIN);

    radio.openReadingPipe(1, masterAddress);

    //radio.setPayloadSize(8);

    radio.setRetries(5,15); // delay, count

    //Wifi setup

    Serial.printf("Connecting to %s ", ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }
    Serial.println(" connected");
}

//===============================================

void loop() {

  if (Serial.available() > 0) {    // is a character available?
    rx_byte = Serial.read();

    if (rx_byte=='1'){

/////checks if multiples when expected num checks is 1
      
      for(int i=0; i<maxIds; i++){
        if(addresses[i][5]!=48){
          Serial.println("sending");
          routine(addresses[i]);
        }
        }
      }
    }
}

//===============================================

void routine(byte address[5]) {

  radio.openWritingPipe(address);
  
  bool receiver=false;
  
  int prevMillis = millis();

  send();
    
  while (true){
      if (millis()-prevMillis>=waitTime){
        Serial.println("No response");
        break;
        }
        
        getData();
        if (data1.id!=NULL){
          showData();
          sendServer();
          break;
          }
      }
}
  

//===============================================

void send() {

        radio.stopListening();
            bool rslt;
            rslt = radio.write( &dataToSend, sizeof(dataToSend) );
        radio.startListening();
        //Serial.print("Data Sent ");
        //Serial.println(dataToSend);
//        if (rslt) {
//            Serial.println("  Acknowledge received");
//        }
//       else {
//            Serial.println("  Tx failed");
//        }
}

//===============================================

void getData() {

    if ( radio.available() ) {
      while (radio.available()) {
        radio.read(&data1, sizeof(package));
      }
    }
}

//===============================================

void showData() {
    Serial.print("Data received from: ");
    Serial.println(data1.id);
    Serial.println(sizeof(data1.id));
    Serial.println(data1.temp); 
    Serial.println(data1.humidity); 
}

//===============================================

void sendServer(){
  radio.stopListening();

  //get Time
  timeClient.begin();
  timeClient.update();
  String Time=timeClient.getFormattedDate();
  timeClient.end();


  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
 
    HTTPClient http;  //Declare an object of class HTTPClient
    //BearSSL::WiFiClientSecure client;


    //client.setInsecure();
 
    //http.begin(client, host);  //Secure
    http.begin(host);  //insecure

    //add headers
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    http.addHeader("Content-Type", "application/json");

    //send json data
    String httpRequestData = "{\"id\":\"{0}\",\"temp\":\"{1}\",\"time\":\"{2}\",\"humidity\":\"{3}\"}";
    
    httpRequestData.replace("{0}",String(data1.id));
    httpRequestData.replace("{1}",String(data1.temp));
    httpRequestData.replace("{2}",String(Time));
    httpRequestData.replace("{3}",String(data1.humidity));
    
    Serial.println(httpRequestData);
    
    int httpCode = http.POST(httpRequestData);     

    Serial.println(httpCode);
                                                             
    if (httpCode > 0) { //Check the returning code
 
      String payload = http.getString();   //Get the request response payload
      Serial.println(payload);                     //Print the response payload
 
    }
 
    http.end();   //Close connection
 
    }
  data1.id=NULL;

  radio.startListening();
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
