#include <WiFiClient.h>
#include <ThingSpeak.h>
#include <ESP8266WiFi.h>

#define CHANNELID1 1449921
#define WIFISSID "Arafat Tanin"
#define PASSWORD "NoConnectioN"
#define WRITEAPI1 "350OMYM5YGOUUADF"

WiFiClient client;

unsigned long CHANNELID = 1449921;
const char * WRITEAPI = "350OMYM5YGOUUADF";
enum channelFields { BODYTEMP, ROOMTEMP, ROOMHUM, PULSE };

void setup() {
  Serial.begin(115200);
  // delay(1000);

  while (!Serial) {
   ; // wait for serial port to connect. Needed for native USB port only
  }
  
  /* Connection for WiFi */
  WiFi.begin(WIFISSID, PASSWORD); 
  
  while(WiFi.status() != WL_CONNECTED){
    delay(2000);
    Serial.println("Reconnecting...");
  }
  
  Serial.println();
  Serial.println("NodeMCU is connected!");
  Serial.println(WiFi.localIP());
  
  ThingSpeak.begin(client);
}

void writeToThingSpeak(int fieldNo, float data, bool isInt){
  if(isInt){
    data = (int)data;
  }
  
  int uploadStatus = ThingSpeak.writeField(CHANNELID, fieldNo, data, WRITEAPI); 

  if(uploadStatus == 200){
    Serial.println("Channel update Successful");
  }
}

void loop() {
  if(Serial.available() > 0){
    // delay(17000);
  int heartbeat = Serial.read();

  writeToThingSpeak(PULSE+1, heartbeat, true);
  Serial.print("In esp HeartBeat: ");
  Serial.print(heartbeat);
  //delay(5000);
  
  float humidity = Serial.read();

  writeToThingSpeak(ROOMHUM+1, humidity, false);
  Serial.print(" Humidity: ");
  Serial.println(humidity);
  //delay(5000);

  float temp = Serial.read();

  writeToThingSpeak(ROOMTEMP+1, temp, false);
  Serial.print(" Tempearture: ");
  Serial.println(temp);
  //delay(5000);
  delay(17000);
  }else{
    Serial.println("Nothing available at that moment");
  }
}
