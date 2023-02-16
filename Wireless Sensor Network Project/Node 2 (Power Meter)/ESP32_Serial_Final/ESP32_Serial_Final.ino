//call the needed libraries
#include <WiFi.h>
#include <PubSubClient.h>
#include "coap_client.h"
#include <String.h>

//create CoAP client
coapClient coap;

//set the IP address and port of the CoAP server
IPAddress ip(192,168,178,149);
int port =5683;

//set the voltage
int VOLTAGE = 230;

//WiFi and Host parameters
const char* ssid = "xxx";
const char* password = "xxxxxxxx";
const char* host = "xxx";

//create WiFi client
WiFiClient wifiClient;
PubSubClient client(wifiClient);

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
void setup_wifi() {
    delay(10);
    //connect to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    randomSeed(micros());
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

//setup of the board
void setup() {
  //serial monitor setup
  Serial.begin(115200);
  //watchdog timer setup
  Serial.setTimeout(500);
  //WiFI setup recall
  setup_wifi();
  //CoAP response recall
  coap.response(callback_response);
  //start coap client
  coap.start();
}

//loop
void loop() {
   bool state;
   client.loop();
   //check if there is something in the serial port
   if (Serial.available() > 0) {
    //if true, read it and put int oa float (we're reading the Irms value only)
     float irms = Serial.parseFloat();
     //print it
     Serial.println("Irms: "+String(irms));
     //calculate power
     float pwr = irms*VOLTAGE;
     //print it
     Serial.println("Pwr: "+String(pwr));
     //use WiFiClient class to create TCP connections
     WiFiClient client;
     //port declaration
     const int httpPort = 80;
     //check server connection
     if (!client.connect(host, httpPort)) {
          Serial.println("connection failed");
          return;
     }     
     //creation of the URL to POST
     String url = "/add_current.php?irms=";
     url += String(irms);
     url += "&pwr=";
     url += String(pwr); 

     //print the url for debug
     Serial.print("POST: ");
     Serial.println(url);

     //POST to the HTTP server
     client.print(String("POST ") + url + " HTTP/1.1\r\n" +
             "Host: " + host + "\r\n" +
             "Connection: close\r\n\r\n");
     unsigned long timeout = millis();
     //watchdog timer
     while (client.available() == 0) {
        if (millis() - timeout > 5000) {
            Serial.println(">>> Client Timeout !");
            client.stop();
            return;
        }
      }
      //convert string into a char array
      char irmsv[sizeof(irms)];
      String sIrms = String(irms);
      sIrms.toCharArray(irmsv, sizeof(sIrms));
      //post request temp
      int msgid0 =coap.post(ip,port,"irms", irmsv, strlen(irmsv));

      //convert string into a char array
      char pwrv[sizeof(pwr)];
      String sPwr = String(pwr);
      sPwr.toCharArray(pwrv, sizeof(sPwr));
      //post request temp
      int msgid1 =coap.post(ip,port,"pwr", pwrv, strlen(pwrv));

      state=coap.loop();
   }
   delay(800);
}
