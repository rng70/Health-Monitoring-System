#include <WiFiClient.h>
#include <ThingSpeak.h>
#include <ESP8266WiFi.h>

#define CHANNELID1 1449921
// #define WIFISSID "Arafat Tanin"
// #define PASSWORD "NoConnectioN"
#define WIFISSID "Maisha"
#define PASSWORD "Maisha@876"
#define WRITEAPI1 "350OMYM5YGOUUADF"

WiFiClient client;

unsigned long CHANNELID = 1449921;
const char * WRITEAPI = "350OMYM5YGOUUADF";
enum channelFields {BODYTEMP=1, ROOMTEMP=2, ROOMHUM=3, PULSE=4};

void setup() {
  Serial.begin(115200);
  // delay(1000);

  while (!Serial) {
   ; // wait for serial port to connect. Needed for native USB port only
  }
  
  /* Connection for WiFi */
  WiFi.begin(WIFISSID, PASSWORD); 
  
  while(WiFi.status() != WL_CONNECTED){
    delay(10000);
    Serial.println("Reconnecting...");
  }
 
  Serial.println(WiFi.localIP());
  
  ThingSpeak.begin(client);
}

void writeToThingSpeak(int pulse, float humidity, float tempRoom, float tempBody){
  ThingSpeak.setField(BODYTEMP, tempBody);
  ThingSpeak.setField(ROOMTEMP, tempRoom);
  ThingSpeak.setField(ROOMHUM, humidity);
  ThingSpeak.setField(PULSE, pulse);
  
  int uploadStatus = ThingSpeak.writeFields(CHANNELID, WRITEAPI); 

  if(uploadStatus == 200){
    Serial.println("Channel update Successful");
  }else{
    Serial.println("Error at updating data");
  }
}

void loop() {
  if(Serial.available() > 0){
    int heartbeat = Serial.read();    
    float humidity = Serial.read();
    float tempRoom = Serial.read();
    float tempBody = Serial.read();
    
    writeToThingSpeak(heartbeat, humidity, tempRoom, tempBody);
    /* delay for next update */
    delay(20000);
  }else{
    Serial.println("Nothing available to read at that moment");
  }
}
