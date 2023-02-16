#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <HTTPClient.h>
#include <ESP32httpUpdate.h>
#include <TimeLib.h>
#include <ESP32Ping.h>

// Replace the next variables with your SSID/Password combination
const char* ssid = "xxx";
const char* password = "xxx";

//Firmware version for update
const int FW_VERSION = 3;

//Watch Dog Const
const int wdtTimeout = 100000;  //time in ms to trigger the watchdog
hw_timer_t *timer = NULL;

// Add your MQTT Broker local (or global) IP address, example:
const char* mqtt_server = "192.168.178.51";

// IP to ping for Wi-Fi status checking
IPAddress ip (192, 168, 178, 1); // The remote ip to ping
bool pingResult;

// Define clients and variables
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
long wifirssi;

// LED & Control Pin
const int ledPin = 2;
const int voltPin = 23;
const int voltPin2 = 22;  

// Update Check
int realtime = 0;
int interval = 60;
int nxtcheck = 60;  

//Watch Dog
void IRAM_ATTR resetModule() {
  ets_printf("reboot\n");
  esp_restart();
}

// Check for new firmware update
void checkForUpdates() 
{
  String fwURL = "http://www.webhost.com/firmware/esp32/gate_opener";
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

// Board Init
void setup() {
  Serial.begin(115200);
  setup_wifi();
  checkForUpdates();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  pinMode(ledPin, OUTPUT);
  pinMode(voltPin, OUTPUT);
  pinMode(voltPin2, OUTPUT);
  digitalWrite(voltPin, HIGH);
  digitalWrite(voltPin2, HIGH);
  //digitalWrite(voltPin, HIGH);

  timer = timerBegin(0, 80, true);                  //timer 0, div 80
  timerAttachInterrupt(timer, &resetModule, true);  //attach callback
  timerAlarmWrite(timer, wdtTimeout * 1000, false); //set time in us
  timerAlarmEnable(timer);                          //enable interrupt
}

// WiFi setup
void setup_wifi() {
  delay(10);
  int trial = 0;
  // We start by connecting to a WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    trial = trial + 1;
    if (trial == 120){
      ESP.restart();
    }
  }
  Serial.println(WiFi.localIP());
}

// MQTT Client Callback
void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message
  if (String(topic) == "esp32/led2") {
    Serial.println("Changing output to ");
    if(messageTemp == "on"){
      Serial.println("on");
      digitalWrite(ledPin, HIGH);
    }
    else if(messageTemp == "off"){
      Serial.println("off");
      digitalWrite(ledPin, LOW);
    }
  }

  if (String(topic) == "esp32/opengate2") {
    Serial.println("Changing output to ");
    if(messageTemp == "on"){
      Serial.println("on");
      digitalWrite(voltPin, LOW);
      delay(400);
      digitalWrite(voltPin, HIGH);
      Serial.println("Changing output to off");
    }
    else if(messageTemp == "off"){
      Serial.println("off");
      digitalWrite(voltPin, HIGH);
    }
  }

  if (String(topic) == "esp32/opengate2partial") {
    Serial.println("Changing output to ");
    if(messageTemp == "on"){
      Serial.println("on");
      digitalWrite(voltPin2, LOW);
      delay(400);
      digitalWrite(voltPin2, HIGH);
      Serial.print("Changing output to off");
    }
    else if(messageTemp == "off"){
      Serial.println("off");
      digitalWrite(voltPin2, HIGH);
    }
  }
}

// Recconect Client to Broker if disconnected
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Gate2", "username", "password")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/led2");
      client.subscribe("esp32/opengate2");
      client.subscribe("esp32/opengate2partial");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


// Looping function
void loop() {
  // Get time
  realtime = now();
  //Serial.println(realtime);

  // Check if is check update time
  if (realtime >= nxtcheck){
    pingResult = Ping.ping(ip);
    // Check if still connected
    if (pingResult) {
      Serial.println("SUCCESS!");
      timerWrite(timer, 0); //reset timer (feed watchdog)
    } else {
      Serial.println("FAILED!");
    }
    // get WiFi RSSi for debug
    wifirssi = WiFi.RSSI();
    char rssiString[16];
    dtostrf(wifirssi, 1, 2, rssiString);   
    client.publish("esp32/statusgate2", rssiString);
    // Check update
    checkForUpdates();
    nxtcheck = nxtcheck + interval;
  }
  // Check if client is still connected
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
