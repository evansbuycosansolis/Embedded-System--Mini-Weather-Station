#include <SoftwareSerial.h>
SoftwareSerial SMARTWeatherAlert(7, 8);
#include "DHT.h"
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

const int trigPin = 8;
const int echoPin = 7;

char serialByte;
uint8_t state = 0;

int ledPin =13;
int EP =9;

const int sensorMin = 0;
const int sensorMax = 1024;

void setup()
{
  SMARTWeatherAlert.begin(19200);
  SMARTWeatherAlertpower();  
  delay(20000);  // give time to log on to network.
   dht.begin();
        pinMode(ledPin, OUTPUT);
        pinMode(EP, INPUT);
}
 
void SMARTWeatherAlertpower()
// software equivalent of pressing the GSM shield "power" button
{
  digitalWrite(9, HIGH);
  delay(1000);
  digitalWrite(9, LOW);
  delay(5000);
}
 
void sendSMS()
{
  //1)proximity=================================
  long duration, inches, cm;
  pinMode(trigPin, OUTPUT);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
  inches = microsecondsToInches(duration);
  cm = microsecondsToCentimeters(duration);
  int waterlevelinches;
  int waterlevelcm;
  waterlevelinches= 115- inches;
  waterlevelcm= 294-cm;
  //2)TempHumid================================
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;  }
  float hif = dht.computeHeatIndex(f, h);
  float hic = dht.computeHeatIndex(t, h, false);
  //3)Rain=====================================
  int sensorReading = analogRead(A0);
  int range = map(sensorReading, sensorMin, sensorMax, 0, 3);
  //4)Vibration================================
  long measurement =TP_init();
  
  //===========================================
  SMARTWeatherAlert.print("AT+CMGF=1\r");                                                        // AT command to send SMS message
  delay(100);
  SMARTWeatherAlert.println("AT + CMGS = \"+639399150254\"");                                     // recipient's mobile number, in international format
 // SMARTWeatherAlert.println("AT + CMGS = \"+639397004779\"");                                     // recipient's mobile number, in international format
  delay(100);
  //===========================================
  
  
  //2)TempHumid
  SMARTWeatherAlert.println("Humidity (%):");
  delay(100);
  SMARTWeatherAlert.println(h);
  delay(100);
  SMARTWeatherAlert.println("Temperature (*C):");
  delay(100);
  SMARTWeatherAlert.println(t);
  delay(100);
    SMARTWeatherAlert.println("Heat Index (*C):");
  delay(100);
  SMARTWeatherAlert.println(hic);
  delay(100);
 

  //3)Rain
  switch (range) {
  case 0:
    SMARTWeatherAlert.println("Rain Condition:");
    SMARTWeatherAlert.println("Heavy");
    break;
  case 1:
    SMARTWeatherAlert.println("Rain Condition:");
    SMARTWeatherAlert.println("Moderate");
    break;
  case 2: 
    SMARTWeatherAlert.println("Rain Condition:");
    SMARTWeatherAlert.println("None");
    break;
  }
  //delay(1);  // delay between reads

  
  SMARTWeatherAlert.println((char)26);                       // End AT command with a ^Z, ASCII code 26
  delay(100); 
  SMARTWeatherAlert.println();
  delay(5000);                                     // give module time to send SMS
  SMARTWeatherAlertpower();                                   // turn off module
}



//============= Void Loop =============
void loop()
{
  sendSMS();
  delay(20000);
  //do {} while (1);
} 
//============= Void Loop =============




