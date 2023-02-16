#include <ESP8266WiFi.h>
#include <Adafruit_BME280.h>
#include <String.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

const char* server = "xxx";
const char* ssid     = "xxx";
const char* password = "xxx";
const char* api_key = "xxx";
const int FW_VERSION = 1;

#define ALTITUDE 10.0 // Altitude over the sea in Sabbioneta, Italy
#define I2C_SDA 4
#define I2C_SCL 5
#define BME280_ADDRESS 0x76  //If the sensor does not work, try the 0x77 address as well
#define ANALOGPIN A0

float cTemp = 0;
float humidity = 0;
float pressure = 0;
float DP = 0;

Adafruit_BME280 bme;

void connectToWifi()
{
  WiFi.enableSTA(true); 
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
  Serial.println("WiFi connected");  
}

void checkForUpdates() {
  String fwURL = "http://www.emanuelepagliari.it/firmware/esp8266/ESP8266_Garage";
  String fwVersionURL = fwURL;
  fwVersionURL.concat(".version");

  Serial.println("Checking for updates at URL");

  HTTPClient httpClient;
  httpClient.begin(fwVersionURL);
  int httpCode = httpClient.GET();
  if(httpCode == 200) {
    String newFWVersion = httpClient.getString();

    Serial.print( "Current firmware: ");
    Serial.println(FW_VERSION);
    Serial.print( "Available firmware: " );
    Serial.println(newFWVersion);

    int newVersion = newFWVersion.toInt();

    if( newVersion > FW_VERSION ) {
      Serial.println( "Update" );

      String fwImageURL = fwURL;
      fwImageURL.concat(".bin");
      t_httpUpdate_return ret = ESPhttpUpdate.update(fwImageURL);

      switch(ret) {
        case HTTP_UPDATE_FAILED:
          Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
          break;

        case HTTP_UPDATE_NO_UPDATES:
          Serial.println("HTTP_UPDATE_NO_UPDATES");
          break;
      }
    }
    else {
      Serial.println("Already last version");
    }
  }
  else {
    Serial.print( "Firmware version check failed, got HTTP response code ");
    Serial.println(httpCode);
  }
  httpClient.end();
}
  
void initSensor()
{
  bool status = bme.begin(BME280_ADDRESS);
  if (!status) {
    Serial.println("Can't find BME28!");
    while (1);
  }
}

void setup() 
{
  Serial.begin(115200);
  connectToWifi();
  checkForUpdates();
  initSensor();
  pinMode(A0, INPUT);
}

double dewPointFast(double celsius, double humidity)
{
        double a = 17.271;
        double b = 237.7;
        double temp = (a * celsius) / (b + celsius) + log(humidity*0.01);
        double Td = (b * temp) / (a - temp);
        return Td;
}

void loop() {
  bool state;

  cTemp = bme.readTemperature()-2;
  if (cTemp>=-30 && cTemp<=50)
  {
    Serial.print("Temp C: ");
    Serial.print(cTemp);
    
    pressure = bme.readPressure();
    pressure = bme.seaLevelForAltitude(ALTITUDE,pressure);
    pressure = pressure/100.0F;
    Serial.print("Press: ");
    Serial.print(pressure);
    
    humidity = bme.readHumidity();  
    Serial.print("RH: ");
    Serial.print(humidity);
    
    DP = dewPointFast(cTemp, humidity);
    Serial.print("DP: ");
    Serial.print(DP);
    
    String postStr;
    postStr +="api_key=";
    postStr += String(api_key);
    postStr +="&temp=";
    postStr += String(cTemp);
    postStr +="&hum=";
    postStr += String(humidity);
    postStr +="&pr=";
    postStr += String(pressure);
    postStr +="&dp=";
    postStr += String(DP);

    // Use WiFiClient class to create TCP connections
    WiFiClient client;
    const int httpPort = 80;
    if (!client.connect(server, httpPort)) {
        Serial.println("Conn failed");
        return;
    }
    // We now create a URI for the request
    String url = "/iot/add_garage.php?";
    url += postStr;

    // This will send the request to the server
    client.print(String("POST ") + url + " HTTP/1.1\r\n" +
               "Host: " + server + "\r\n" +
               "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while (client.available() == 0) {
        if (millis() - timeout > 5000) {
            Serial.println("Timeout!");
            client.stop();
            return;
        }
    }
  } 
  ESP.deepSleep(300e6);
}
