#include <DHT.h>
#include <DHT_U.h>
#include<LiquidCrystal.h>
#include <Adafruit_Sensor.h>
#include <PulseSensorPlayground.h>     /* Includes the PulseSensorPlayground Library. */

#define DHTPIN 2
#define DHTTYPE DHT11 
#define TEMPTYPE 0 //for celcius
#define UNIVERSALDELAY 1000
#define USE_ARDUINO_INTERRUPTS true    // Set-up low-level interrupts for most acurate BPM math.

/* Variables */
DHT dhtSensor(DHTPIN, DHTTYPE);
LiquidCrystal lcd(5,6,7,8,9,10);
PulseSensorPlayground pulseSensor;      /* Creates an instance of the PulseSensorPlayground object called "pulseSensor" */

volatile const int PulseWire = 0;       /* PulseSensor PURPLE WIRE connected to ANALOG PIN 0 */
volatile const int LED13 = 13;          /* The on-board Arduino LED, close to PIN 13. */
volatile int Threshold = 550;           /* Determine which Signal to "count as a beat" and which to ignore. */
                                        /* Otherwise leave the default "550" value. */

volatile float dhtHumidity;
volatile float celciusTemperature;

void setup() {
  /* put your setup code here, to run once: */
  pulseSensor.analogInput(PulseWire);   
  pulseSensor.blinkOnPulse(LED13);      /* auto-magically blink Arduino's LED with heartbeat. */
  pulseSensor.setThreshold(Threshold);
   
  Serial.begin(9600);
  
  lcd.begin(16,2);
  lcd.setCursor(1,0);
  
  dhtSensor.begin();
   
  if(pulseSensor.begin()){
    Serial.print("Starting...");
   // lcd.print("Starting");
  }
}
void serialEvent() {
 if(Serial.available()) {
    /* get the new byte: */
    int temp = Serial.read();
    float t=temp*0.25*4.0;
    
    Serial.print(F("LM 35 Temperature: "));
    Serial.print(t);
    Serial.println(F("°C"));
    
    lcd.setCursor(1,1);
    lcd.print(t);
    lcd.setCursor(4,1);
    lcd.print("C       ");
    delay(1000);
  }
}
void loop() {
  /* put your main code here, to run repeatedly: */
  lcd.setCursor(1,0);
  
  int myBPM = pulseSensor.getBeatsPerMinute();      /* Calls function on our pulseSensor object that returns BPM as an "int". */
                                         
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
  
  delay(UNIVERSALDELAY/25);
  dhtHumidity = dhtSensor.readHumidity();
  celciusTemperature = dhtSensor.readTemperature();

  Serial.print("Humidity: ");
  Serial.print(dhtHumidity);
  Serial.print("Celcius Temperature: ");
  Serial.print(celciusTemperature);
  Serial.println("°C");
    
  delay(UNIVERSALDELAY);
}
