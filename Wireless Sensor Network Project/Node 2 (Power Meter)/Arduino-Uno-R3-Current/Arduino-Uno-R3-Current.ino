//needed libraries
#include <SoftwareSerial.h> 
#include "EmonLib.h" 
#include <Wire.h>  

//serial port declaration (we only use the TX port)
SoftwareSerial sw(6, 7); // RX, TX
//declaration of the sensor
EnergyMonitor emon1;
//declare voltage
int VOLT = 230.0;
//Pin where the analog input is located
int pin_sct = 0;

//board setup
void setup() 
{
  //serial monitor setup
  Serial.begin(115200);
  //serial to ESP setup
  Serial.println("Interfacfing Arduino Uno R3 with ESP32");
  sw.begin(115200);
  //Pin, calibration of the SCt sensor. 
  emon1.current(pin_sct, 33);
} 

//loop
void loop() 
{ 
  //retrive current 
  double Irms = emon1.calcIrms(1480);
  //show current
  Serial.print("Corrente : ");
  Serial.print(Irms);
   
  //power calculation and show it
  Serial.print(" Potenza : ");
  Serial.println(Irms*VOLT);

  //for debug
  Serial.println("Sending data to nodemcu");

  //serial port TX
  sw.println(Irms);
  
  delay(1000);
}
