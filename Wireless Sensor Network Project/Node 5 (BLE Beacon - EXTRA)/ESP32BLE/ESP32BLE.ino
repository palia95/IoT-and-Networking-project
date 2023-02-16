//BLE libraries
#include "ESP32BleAdvertise.h"
SimpleBLE ble;

//variables and pins
int LightPin = 36; // select the input pin for Light Sensor
int MoisturePin = 26; // select the input pin for Moisture Sensor
double sensorValueLight = 0;
double sensorValueMoisture = 0;
double ValueMoisture = 0;
int ID = 1; //device ID
unsigned long timeHolder = 0;

//board setup
void setup() {
  //serail monitor setup
    Serial.begin(115200);
    //BLE setup, set the device name
    ble.begin("ESP32-BLE");
    //set input pin mode
    pinMode(MoisturePin, INPUT);
}

//loop
void loop() {
    //read light valur from light sensor
    sensorValueLight = analogRead(LightPin);
    //print the value
    Serial.println(sensorValueLight);

    //read moistore values from moisture sensor
    sensorValueMoisture = analogRead(MoisturePin);

    //map the voltage to percentile
    ValueMoisture = map(sensorValueMoisture,4095,0,50,100);
    
    //print it
    Serial.print("Analog EAW: ");
    Serial.println(sensorValueMoisture);
    Serial.print("% Value: ");
    Serial.println(ValueMoisture);

    //create Advertising Payload (it has to bee less than 28 Byte)
    String ADV = String(ID);
    ADV +="&lux=";
    ADV += String(sensorValueLight);
    ADV +="&mst=";
    ADV += String(ValueMoisture);
    ADV +="%";

    //advertise it
    ble.advertise(ADV);
}
