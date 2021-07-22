#include <DHT.h>
#include <DHT_U.h>
#include<LiquidCrystal.h>
#include <SoftwareSerial.h>
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
SoftwareSerial sim800(GTX, GRX);
SoftwareSerial esp8266(ETX, ERX);               /* For esp8266 module */
DHT dhtSensor(DHTPIN, DHTTYPE);
LiquidCrystal lcd(5, 6, 7, 8, 9, 10);
PulseSensorPlayground pulseSensor;      /* Creates an instance of the PulseSensorPlayground object called "pulseSensor" */

char incomingChar;

volatile float lm35Temp = 11.0;
volatile const int PulseWire = 0;       /* PulseSensor PURPLE WIRE connected to ANALOG PIN 0 */
volatile const int LED13 = 13;          /* The on-board Arduino LED, close to PIN 13. */
volatile int Threshold = 550;           /* Determine which Signal to "count as a beat" and which to ignore. */
                                        /* Otherwise leave the default "550" value. */
volatile int myBPM;
volatile float dhtHumidity;
volatile float celciusTemperature;

unsigned long previousMillis = 0; // last time update
long interval = 300000; // interval at which to do something (milliseconds)(5 min)

void setup() {
  /* put your setup code here, to run once: */
  pulseSensor.analogInput(PulseWire);   
  pulseSensor.blinkOnPulse(LED13);      /* auto-magically blink Arduino's LED with heartbeat. */
  pulseSensor.setThreshold(Threshold);
   
  Serial.begin(9600);
  esp8266.begin(115200);
  sim800.begin(9600);
  
  lcd.begin(16,2);
  lcd.setCursor(1,0);
  
  dhtSensor.begin();
  delay(UNIVERSALDELAY/2);
   
  if(pulseSensor.begin()){
    Serial.print("Starting...");
  }

  // Give time to your GSM shield log on to network
  delay(20000);
  Serial.print("SIM900 ready...");

  // AT command to set SIM900 to SMS mode
  sim800.print("AT+CMGF=1\r"); 
  delay(100);
  // Set module to send SMS data to serial out upon receipt 
  sim800.print("AT+CNMI=2,2,0,0,0\r");
  delay(100);
}

/*void serialEvent(){
 if(Serial.available()) {
    // get the new byte:
    int temp = Serial.read();
    lm35Temp=temp*0.25*4.0;

    if(lm35Temp < 30.0){
      lm35Temp = 30.0 + abs(30.0 - lm35Temp);
    }
    if(lm35Temp > 45.0){
      lm35Temp = 45.0 - abs(45.0 - lm35Temp);
    }
    
    Serial.print(F("LM 35 Temperature: "));
    Serial.print(lm35Temp);
    Serial.println(F("°C"));
    
    lcd.setCursor(1,1);
    lcd.print(lm35Temp);
    lcd.setCursor(4,1);
    lcd.print("C       ");
    delay(1000);
  }
}*/

void pulseSensorMethod(){
  myBPM = pulseSensor.getBeatsPerMinute();      /* Calls function on our pulseSensor object that returns BPM as an "int". */
                                         
  if (pulseSensor.sawStartOfBeat()){                /* Constantly test to see if "a beat happened". */ 
    Serial.println("♥ A HeartBeat Happened ! ");    /* If test is "true", print a message "a heartbeat happened". */
    Serial.print("BPM: ");                          /* Print phrase "BPM: " */ 
    Serial.println(myBPM);  
    lcd.clear();                                    /* Print the value inside of myBPM. */ 
    lcd.setCursor(1,0);
    lcd.print(myBPM);
    lcd.setCursor(4,0);
    lcd.print("BPM             ");
  }
}

void dht11Sensor(){
  dhtHumidity = dhtSensor.readHumidity();
  celciusTemperature = dhtSensor.readTemperature();

  Serial.print("Humidity: ");
  Serial.print(dhtHumidity);
  Serial.print(" Celcius Temperature: ");
  Serial.print(celciusTemperature);
  Serial.println("°C");
}

void esp8266Module(){
  Serial.println("<--------------- In esp --------------->");
    
  Serial.print("HeartBeat: ");
  Serial.println(myBPM);
  esp8266.write(myBPM);
  esp8266.flush();
  //delay(5000);

  Serial.print("Humidity: ");
  Serial.println(dhtHumidity);
  esp8266.write(dhtHumidity);
  esp8266.flush();
  //delay(5000);

  Serial.print("Celcius Temperature: ");
  Serial.println(celciusTemperature);
  esp8266.write(celciusTemperature);
  esp8266.flush();
  //delay(5000);

}

boolean dataIsClean(){
  boolean firstPass = false;

  if(!isnan(dhtHumidity) && !isnan(celciusTemperature)&& !isnan(lm35Temp) && !isnan(myBPM)){
    firstPass = true;
  }
  if(firstPass){
    if(dhtHumidity > 0 && celciusTemperature > 0 && lm35Temp > 0 && myBPM > 0){
      return true;
    }
  }
  return false;
}

boolean SMSRequest() {
  Serial.println("In smsrequest");
  if(sim800.available() > 0) {
    incomingChar=sim800.read();
    if(incomingChar=='S') {
      delay(10);
      Serial.print(incomingChar);
      incomingChar=sim800.read();
      if(incomingChar =='T') {
        delay(10);
        Serial.print(incomingChar);
        incomingChar=sim800.read();
        if(incomingChar=='A') {
          delay(10);
          Serial.print(incomingChar);
          incomingChar=sim800.read();
          if(incomingChar=='T') {
            delay(10);
            Serial.print(incomingChar);
            incomingChar=sim800.read();
            if(incomingChar=='E') {
              delay(10);
              Serial.print(incomingChar);
              Serial.print("...Request Received \n");
              return true;
            }
          }
        }
      }
    }
  }
  return false;
}

boolean itsTimeToSendSMS(){
  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis > interval) {
     previousMillis = currentMillis; 
     return true;
  }
  else
  return false;
}
void simModule(){
  if(SMSRequest()){
    if(dataIsClean()){
      Serial.print("In here successful");
      delay(10);
      sim800.println("AT + CMGS = \"+8801735590153\"");
      delay(100);

      String dataMessage = "Name: Tanin\nPulse: " + (String)myBPM + "\nBody Temperature: " + (String)lm35Temp + "°C Room Condition:\n\tRoom Temperature: " + (String)celciusTemperature + "°C\n\tRoom Humidity: "+(String)dhtHumidity+"%";

      sim800.print(dataMessage);
      delay(100);
      sim800.println((char)26);
      delay(100);
      sim800.println();
      delay(5000);
    }else{
      Serial.print("In here successful--1");
      delay(10);
      sim800.println("AT + CMGS = \"+8801735590153\"");
      delay(100);

      String dataMessage = "Server is unreachable. Please try again later";

      sim800.print(dataMessage);
      delay(100);
      sim800.println((char)26);
      delay(100);
      sim800.println();
      delay(5000);
    }
    delay(10);
  }else if(itsTimeToSendSMS()){
    if(dataIsClean()){
      delay(10);
      sim800.println("AT + CMGS = \"+8801735590153\"");
      delay(100);

      String dataMessage = "Patient Name: Tanin\nPulse: " + (String)myBPM + "\nBody Temperature: " + (String)lm35Temp + "°C Room Condition:\n\tRoom Temperature: " + (String)celciusTemperature + "°C\n\tRoom Humidity: "+(String)dhtHumidity+"%";

      sim800.print(dataMessage);
      delay(100);
      sim800.println((char)26);
      delay(100);
      sim800.println();
      delay(5000);
    }else{
      delay(10);
      sim800.println("AT + CMGS = \"+8801735590153\"");
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
  lcd.setCursor(1,0);
  
  pulseSensorMethod();
  delay(UNIVERSALDELAY/25);
  //Serial.println("After pulse sensor");
  dht11Sensor();  
  delay(UNIVERSALDELAY);
  esp8266Module();
  Serial.println("-------------------- After esp ----------------");
  delay(UNIVERSALDELAY*5);
  simModule();
  delay(UNIVERSALDELAY);
}
