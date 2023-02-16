//call the needed libraries
#include <ESP8266WiFi.h>
#include <Adafruit_BME280.h>
#include <String.h>
#include "coap_client.h"

//create CoAP client
coapClient coap;

//set the IP address and port of the CoAP server
IPAddress ip(192,168,178,149); 
int port =5683;

//WiFi and Host parameters
const char* server = "xxx";
const char* ssid     = "xxx";
const char* password = "xxxxxxxx";

//used constants
#define ALTITUDE 25.0
#define I2C_SDA 4
#define I2C_SCL 5
#define BME280_ADDRESS 0x76  //If the sensor does not work, use 0x77
#define ANALOGPIN A0

//used variables
float cTemp = 0;
float humidity = 0;
float pressure = 0;
float DP = 0;
//sensor declaration
Adafruit_BME280 bme;

//coap client response callback
void callback_response(coapPacket &packet, IPAddress ip, int port) {
    char p[packet.payloadlen + 1];

    //payload copy into the support variable
    memcpy(p, packet.payload, packet.payloadlen);
    p[packet.payloadlen] = NULL;

    //response from coap server to the PING request
    if(packet.type==3 && packet.code==0){
       Serial.println("ping ok");
    }
    Serial.println(p);
}

//WiFi setup funcion
void connectToWifi()
{
  WiFi.enableSTA(true); 
  delay(100);
  //connect to a WiFi network
  WiFi.begin(ssid, password);  
  while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
  Serial.println("");
  Serial.println("WiFi connected");  
}

//sensor initiliazation
void initSensor()
{
  bool status = bme.begin(BME280_ADDRESS);
  //check sensor
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
}

//setup of the board
void setup() 
{
  //serail monitor setup
  Serial.begin(115200);
  //recall sensor initialization
  initSensor();
  //input pin work mode
  pinMode(A0, INPUT);
  //WiFi setup recall
  connectToWifi();
  //CoAP response recall
  coap.response(callback_response);
  //start coap client
  coap.start();
}

//fast dew point calculation function
double dewPointFast(double celsius, double humidity)
{
        double a = 17.271;
        double b = 237.7;
        double temp = (a * celsius) / (b + celsius) + log(humidity*0.01);
        double Td = (b * temp) / (a - temp);
        return Td;
}

//loop
void loop() {
  bool state;

  //read temeprature values from sensor
  cTemp = bme.readTemperature();

  //check if values are real or fake (we could make it an all data, but on temperature is enought)
  if (cTemp>=-20 && cTemp<=50)
  {
    //print temperature
    Serial.print("Temperature in Celsius : ");
    Serial.print(cTemp);
    Serial.println(" C");

    //read pressure and correct it to the current altitude
    pressure = bme.readPressure();
    pressure = bme.seaLevelForAltitude(ALTITUDE,pressure);
    pressure = pressure/100.0F;

    //print pressure
    Serial.print("Pressure : ");
    Serial.print(pressure);
    Serial.println(" hPa");

    //read humidity and print it
    humidity = bme.readHumidity();  
    Serial.print("Relative Humidity : ");
    Serial.print(humidity);
    Serial.println(" RH");

    //calculate Dew Point and print it
    DP = dewPointFast(cTemp, humidity);
    Serial.print("Dew Point : ");
    Serial.print(DP);
    Serial.println(" C");

    //create the string to POST
    String postStr;
    postStr +="temp=";
    postStr += String(cTemp);
    postStr +="&hum=";
    postStr += String(humidity);
    postStr +="&pr=";
    postStr += String(pressure);
    postStr +="&dp=";
    postStr += String(DP);

    //use WiFiClient class to create TCP connections
    WiFiClient client;
    //POST to the HTTP server
    const int httpPort = 80;
    if (!client.connect(server, httpPort)) {
        Serial.println("connection failed");
        return;
    }
    
    //finalize URL for the POST
    String url = "/add_data.php?";
    url += postStr;
    
    //print URL for debug
    Serial.print("URL: ");
    Serial.println(url);

    //POST to the HTTP server
    client.print(String("POST ") + url + " HTTP/1.1\r\n" +
               "Host: " + server + "\r\n" +
               "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while (client.available() == 0) {
        if (millis() - timeout > 5000) {
            Serial.println(">>> Client Timeout !");
            client.stop();
            return;
        }
    }
  
    //convert string into a char array
    char temp[sizeof(cTemp)];
    String sTemp = String(cTemp);
    sTemp.toCharArray(temp, sizeof(sTemp));
    //post request temp
    int msgid0 =coap.post(ip,port,"temp", temp, strlen(temp));

    //convert string into a char array   
    char hum[sizeof(humidity)];
    String sHum = String(humidity);
    sHum.toCharArray(hum, sizeof(sHum));
    //post request hum
    int msgid1 =coap.post(ip,port,"hum", hum, strlen(hum));

    //convert string into a char array
    char pr[sizeof(pressure)];
    String sPr = String(pressure);
    sPr.toCharArray(pr, sizeof(sPr));
    //post request pr
    int msgid2 =coap.post(ip,port,"pr", pr, strlen(pr));

    //convert string into a char array
    char dp[sizeof(DP)];
    String sDP = String(DP);
    sDP.toCharArray(dp, sizeof(sDP));
    //post request DP
    int msgid3 =coap.post(ip,port,"dp", dp, strlen(dp));

    //delay and loop
    delay(300);
    state=coap.loop();
  }
  //send to sleep for 5 minutes
  ESP.deepSleep(300e6);
  
  //delay(5000);
}
