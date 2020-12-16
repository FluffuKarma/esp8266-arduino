#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

const char* ssid = "BELL695";
const char* password = "47515EDDFEF4";

//Your Domain name with URL path or IP address with path
const char* host = "http://jsonplaceholder.typicode.com/users/1";

String url = "/data";

const int port = 80;

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 5000;

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA); //defines as station
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 
  Serial.println("Timer set to 5 seconds.");
}

void loop() {
  // Use WiFiClientSecure class to create TLS connection
  WiFiClientSecure connection;
  Serial.print("connecting to : ");
  Serial.println(host);
  
  if (!connection.connect(host,80)) {
    Serial.println("connection failed");
    return;
  }

//  if (connection.verify(fingerprint, host)) {
//    Serial.println("certificate matches");
//  } else {
//    Serial.println("certificate doesn't match");
//  }

  Serial.print("requesting URL: '");
  Serial.print(host);
  Serial.println("'");

  String msg="";

  connection.print(String("POST ") + url + " HTTP/1.1\r\n" +
          "Host: " + host + "\r\n" +
          "Connection: close\r\n" +
          "Accept: */*\r\n" +
          "User-Agent: BuildFailureDetectorESP8266\r\n" +
          "Content-Type: application/json;charset=utf-8\r\n" +
          "Content-Length: "+msg.length()+"\r\n" +
          "\r\n" + 
          msg + "\r\n");

  Serial.println("request sent");
  unsigned long timeout = millis();
  while (connection.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> connection Timeout !");
      connection.stop();
      return;
    }
  }
  
  // Read all the lines of the reply from server and print them to Serial
  while(connection.available()){
    String line = connection.readStringUntil('\r');
    Serial.print(line);
  }
  
  delay(5000);  //GET Data at every 5 seconds
}
