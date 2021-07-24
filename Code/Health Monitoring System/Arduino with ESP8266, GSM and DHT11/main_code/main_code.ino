#include <DHT.h>
#include <DHT_U.h>
#include<LiquidCrystal.h>
//#include <SoftwareSerial.h>
#include <NeoSWSerial.h>
#include <Adafruit_Sensor.h>

#define ETX 3
#define ERX 4
#define GTX 10
#define GRX 11
#define DHTPIN 2                       /* Define PIN for DHT11 sensor. It's digital PIN */
#define TEMPTYPE 0                     /* For reading temperature in celcius */
#define DHTTYPE DHT11
#define UNIVERSALDELAY 1000
#define USE_ARDUINO_INTERRUPTS true    /* Set-up low-level interrupts for most acurate BPM math */
                                       /* Always declare USE_ARDUINO_INTERRUPTS before including PulseSensorPlayground.h */

#include <PulseSensorPlayground.h>     /* Includes the PulseSensorPlayground Library. */

/* Variables */
DHT dhtSensor(DHTPIN, DHTTYPE);
NeoSWSerial sim800(GTX, GRX);
NeoSWSerial esp8266(ETX, ERX);       /* For esp8266 module */
PulseSensorPlayground pulseSensor;      /* Creates an instance of the PulseSensorPlayground object called "pulseSensor" */
LiquidCrystal lcd(5, 6, 7, 8, 9, 12);   /* R, En, D5-D7 respectively */

int loopCount;
char incomingChar;

const String hyperlink = "%\n Check it here: https://thingspeak.com/channels/1449921";

bool itsFirstTime = true;
volatile float lm35Temp = 0.0;
volatile const int PulseWire = 0;       /* PulseSensor PURPLE WIRE connected to ANALOG PIN 0 */
volatile const int LED13 = 13;          /* The on-board Arduino LED, close to PIN 13. */
volatile int Threshold = 550;           /* Determine which Signal to "count as a beat" and which to ignore. */
                                        /* Otherwise leave the default "550" value. */
volatile int myBPM;
volatile float dhtHumidity;
volatile float celciusTemperature;

unsigned long previousMillis = 0;       /* last time update */
long interval = 300000;                 /* interval at which to do something (milliseconds)(5 min) */

static void handleRxChar( uint8_t c ) 
{
  //this function will check every data from sim800 and c is the reading
 
}
void setup() {
  /* put your setup code here, to run once: */
  pulseSensor.analogInput(PulseWire);   
  pulseSensor.blinkOnPulse(LED13);      /* auto-magically blink Arduino's LED with heartbeat */
  pulseSensor.setThreshold(Threshold);
   
  Serial.begin(9600);
  
  sim800.attachInterrupt( handleRxChar );
  sim800.begin(9600);
  
  esp8266.begin(115200);
  
  lcd.begin(16,2);
  lcd.clear();
  lcd.setCursor(1,0);
  
  dhtSensor.begin();
  delay(UNIVERSALDELAY/2);
   
  if(pulseSensor.begin()){
    Serial.println("Starting...");
  }

  /* Give time to your GSM shield log on to network */
  delay(20000);
  
  Serial.println();
  Serial.println("SIM800 ready...");

  /* AT command to set SIM800 to SMS mode */
  Serial.println("SIM800 ready...");

  // AT command to set SIM800 to SMS mode
  sim800.print("AT+CMGF=1\r"); 
  delay(100);
  
  /* Set module to send SMS data to serial out upon receipt */ 
  sim800.print("AT+CNMI=2,2,0,0,0\r");
  delay(100);
}

void serialEvent(){
 if(Serial.available()) {
    /* get the new byte: */
    for(int i=0;i<100;i++){
      float temp = (float)Serial.read();
      lm35Temp += temp*0.25*4.0;
    }

    lm35Temp /= 100.0;
    if(lm35Temp < 30.0){
      lm35Temp = 30.0 + abs(30.0 - lm35Temp);
    }
    if(lm35Temp > 45.0){
      lm35Temp = 45.0 - abs(45.0 - lm35Temp);
    }
    
    Serial.print(F("LM 35 Temperature: "));
    Serial.print(lm35Temp);
    Serial.println(F("°C"));
    
    lcd.setCursor(0,0);
    lcd.print("BodyTemp:");
    lcd.setCursor(10, 0);
    lcd.print(lm35Temp);
    lcd.setCursor(15,0);
    lcd.print("C");
    delay(6000);
  }
}

void pulseSensorMethod(){
  myBPM = pulseSensor.getBeatsPerMinute();      /* Calls function on our pulseSensor object that returns BPM as an "int". */
                                         
  if (pulseSensor.sawStartOfBeat()){                 /* Constantly test to see if "a beat happened". */ 
    Serial.println("♥ A HeartBeat Happened ! ");     /* If test is "true", print a message "a heartbeat happened". */
    Serial.print("BPM: ");                           /* Print phrase "BPM: " */ 
    Serial.println(myBPM);                           /* Print the value inside of myBPM. */ 
    lcd.setCursor(0,1);
    lcd.print("Heartbeat: ");
    lcd.setCursor(10,1);
    lcd.print(myBPM);
    lcd.setCursor(13,1);
    lcd.print("BPM");
    delay(2000);
  }
}

void dht11Sensor(){
  dhtHumidity = dhtSensor.readHumidity();
  celciusTemperature = dhtSensor.readTemperature();

  Serial.print("Room Humidity: ");
  Serial.print(dhtHumidity);
  Serial.println("%");
  Serial.print("Room Temperature: ");
  Serial.print(celciusTemperature);
  Serial.println("°C");

  if(!isnan(dhtHumidity) && !isnan(celciusTemperature)){
    lcd.setCursor(0,1);
    lcd.print("Room Temp: ");
    lcd.setCursor(11,1);
    lcd.print(celciusTemperature);
    lcd.setCursor(15,1);
    lcd.print("C");

    delay(2000);
    int iHM = (int)dhtHumidity;
    lcd.setCursor(0,1);
    lcd.print("RoomHumidity:");
    lcd.setCursor(13,1);
    lcd.print(iHM);
    lcd.setCursor(15,1);
    lcd.print("%");
  }
}

void esp8266Module(){
  Serial.println("<--------------- Sending through ESP8266 --------------->");
    
  //Serial.print("HeartBeat: ");
  //Serial.println(myBPM);
  esp8266.write(myBPM);
  esp8266.flush();
  //delay(5000);

  //Serial.print("Humidity: ");
  //Serial.println(dhtHumidity);
  esp8266.write(dhtHumidity);
  esp8266.flush();
  //delay(5000);

  //Serial.print("Celcius Temperature: ");
  //Serial.println(celciusTemperature);
  esp8266.write(celciusTemperature);
  esp8266.flush();
  //delay(5000);

  //Serial.print("Body Temperature: ");
  //Serial.println(lm35Temp);
  esp8266.write(lm35Temp);
  esp8266.flush();
  //delay(5000);

  Serial.println("<--------------- Successfully Sent through ESP8266 --------------->");
}

boolean dataIsClean(){
  boolean firstPass = false;

  if(!isnan(dhtHumidity) && !isnan(celciusTemperature)&& !isnan(lm35Temp) && !isnan(myBPM)){
    firstPass = true;
  }
  if(firstPass){
    if(dhtHumidity >= 0 && celciusTemperature >= 0 && lm35Temp >= 0 && myBPM >= 0){
      return true;
    }
  }
  return true;
}

boolean SMSRequest(){
  Serial.println("In smsrequest");
  if(sim800.available() > 0) {
    incomingChar=sim800.read();
    if(incomingChar=='S' || incomingChar=='s') {
      delay(10);
      Serial.println(incomingChar);
      incomingChar=sim800.read();
      if(incomingChar =='T' || incomingChar=='t') {
        delay(10);
        Serial.println(incomingChar);
        incomingChar=sim800.read();
        if(incomingChar=='A' || incomingChar=='a') {
          delay(10);
          Serial.println(incomingChar);
          incomingChar=sim800.read();
          if(incomingChar=='T' || incomingChar=='t') {
            delay(10);
            Serial.println(incomingChar);
            incomingChar=sim800.read();
            if(incomingChar=='E' || incomingChar=='e') {
              delay(10);
              Serial.println(incomingChar);
              Serial.println("...Request Received \n");
              Serial.println("Returning True... ... ... ... ... ... ... ...");
              return true;
            }
          }
        }
      }
    }
  }
  Serial.println("Returning False... ... ... ... ... ... ... ...");
  return false;
}

boolean itsTimeToSendSMS(){
  if(itsFirstTime || (loopCount == 1000)){
    loopCount = 0;
    return true;
  }
  return false;
}

void simModule(){
  String dataMessage = "Name: Arafat Tanin\nPulse: " + (String)myBPM + "\nBody Temperature: " + (String)lm35Temp + "degreeC\nRoom Temperature: " + (String)celciusTemperature + "degreeC\nRoom Humidity: "+(String)dhtHumidity;
  Serial.println(dataMessage);
  if(SMSRequest()){
    if(dataIsClean()){
      Serial.println("<--------------- Sending Message through GSM --------------->");
      delay(10);
      sim800.println("AT+CMGS=\"+8801735590153\"");
      delay(100);

      sim800.print(dataMessage);
      delay(100);
      sim800.println((char)26);
      delay(100);
      sim800.println();
      delay(5000);
      Serial.println("<--------------- Message was sent Successfully --------------->");
    }else{
      Serial.println("<--------------- Sending Error Message through GSM --------------->");
      delay(10);
      sim800.println("AT+CMGS=\"+8801735590153\"");
      delay(100);

      String dataMessage = "Server is unreachable. Please try again later";

      sim800.print(dataMessage);
      delay(100);
      sim800.println((char)26);
      delay(100);
      sim800.println();
      delay(5000);
      Serial.print("<--------------- Error Message was sent Successfully --------------->");
    }
    delay(10);
  }else if(itsTimeToSendSMS()){
    if(dataIsClean()){
      delay(10);
      sim800.println("AT+CMGS=\"+8801735590153\"");
      delay(100);
      
      sim800.print(dataMessage);
      delay(100);
      sim800.println((char)26);
      delay(100);
      sim800.println();
      delay(5000);
    }else{
      delay(10);
      sim800.println("AT+CMGS=\"+8801735590153\"");
      delay(100);

      String dataMessage = "Error at processing data. Sorry for the inconvenience.\nWe will reach you soon";

      sim800.print(dataMessage);
      delay(100);
      sim800.println((char)26);
      delay(100);
      sim800.println();
      delay(5000);
    }
    delay(10);
  }
}


void loop(){
  /* put your main code here, to run repeatedly: */
  /* lcd.clear(); */
  lcd.setCursor(1,0);
  
  pulseSensorMethod();
  //delay(UNIVERSALDELAY/25);
  
  dht11Sensor();  
  //delay(UNIVERSALDELAY/25);
  
  esp8266Module();
  //delay(UNIVERSALDELAY*5);

  //simModule();
  loopCount += 1;
  itsFirstTime = false;
  Serial.print("Current Loop Count: ");
  Serial.println(loopCount);
  Serial.print("First Time Message Send Status: ");
  Serial.println(itsFirstTime);
  Serial.print("Hyperlink: ");
  Serial.println(hyperlink);
  Serial.println("Waiting... ... ... ... ... ... ... ... ... ... ... ... ... ... ...");
  delay(UNIVERSALDELAY*5);
}
