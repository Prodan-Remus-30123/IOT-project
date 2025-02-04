
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into digital pin 2 on the Arduino
#define ONE_WIRE_BUS 2
// Setup a oneWire instance to communicate with any OneWire device
OneWire oneWire(ONE_WIRE_BUS);	
// Pass oneWire reference to DallasTemperature library
DallasTemperature sensors(&oneWire);

//Arduino to NodeMCU Lib
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

//Initialise Arduino to NodeMCU (10=Rx & 11=Tx)
SoftwareSerial nodemcu(10, 11);


//Humidity
#define sensorPower 3
#define sensorPin A0

//Pump 
#define DCwater_pump 4

//light
#define SensorLight 5

//intensity
#include <Wire.h>
#include <BH1750.h>
BH1750 intensity; 

float temp;
int humidity;
float light;
int flag0=-1;
int flag1=-1;
bool k=0;

void setup() {

  sensors.begin();	 
  pinMode(7,OUTPUT);
  digitalWrite(7,LOW);
//humidity
  pinMode(sensorPower, OUTPUT);
	digitalWrite(sensorPower, LOW);

//light
  pinMode(SensorLight,INPUT);

//pump
  pinMode(DCwater_pump, OUTPUT); 
  
  Serial.begin(115200);
  //intensity.begin();
  nodemcu.begin(115200);
  delay(1000);

  Serial.println("Program started");
}

void loop() {
  temp = temperature();
  humidity = readSensor();
  light = digitalRead(SensorLight);
  flag0=receiveFlag();
  sendSensorValues();
  
  if(flag0==0)
   pump(humidity);
   else
   if(flag0==1)
    digitalWrite(DCwater_pump, HIGH);

//Serial.print("flag1 value: ");
 // Serial.println(flag1);

  if(flag1==3)
   digitalWrite(7,LOW);
    else
   if(flag1==2)
   {Serial.println("A intrat");
    digitalWrite(7,HIGH);
   }
  
}

void sendSensorValues(){

 StaticJsonDocument<1000> data;
  Serial.println(temp);
  Serial.println(humidity);
  Serial.println(light);
 if(light==HIGH)               //HIGH means,light got blocked
  Serial.println("Fara lumina");         //if light is not present,LED on
  else
  Serial.println("Are lumina"); 

  //Assign collected data to JSON Object
  data["humidity"] = humidity;
  data["temperature"] = temp; 
  data["isLight"]=light;

  serializeJson(data,nodemcu);
  delay(2000);
}

int receiveFlag(){
  StaticJsonDocument<1000> data2; 
  DeserializationError error = deserializeJson(data2, nodemcu);
  Serial.print("Size of data2: ");
  Serial.println(data2.size());
if (error)
  {Serial.println("A avut loc o eroare");
  Serial.println(error.c_str());
    return;}

if(data2.isNull()==true)
Serial.println("Da");
else
Serial.println("Nu");

 int flag0=data2["flag0"];
  //flag1=data2["flag1"];
  Serial.print("Received flag0 value: ");
  Serial.println(flag0);
  //Serial.print("Received flag1 value: ");
  //Serial.println(flag1);
  return flag0;
}


float temperature()
{
  sensors.requestTemperatures(); 
  return sensors.getTempCByIndex(0);
}

//  This function returns the analog soil moisture measurement
int readSensor() {
	digitalWrite(sensorPower, HIGH);	// Turn the sensor ON
	delay(10);							// Allow power to settle
	int val = analogRead(sensorPin);
  if(val>=1000)	// Read the analog value form sensor
  val=999;
	digitalWrite(sensorPower, LOW);		// Turn the sensor OFF
	return val;							// Return analog moisture value
}

void pump(int humidity)
{
  if(humidity>=800)
    digitalWrite(DCwater_pump, HIGH);
  else
    digitalWrite(DCwater_pump, LOW);

}
