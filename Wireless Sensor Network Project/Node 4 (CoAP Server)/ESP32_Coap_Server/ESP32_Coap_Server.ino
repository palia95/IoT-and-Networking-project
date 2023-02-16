//libraries needed
#include <WiFi.h>
#include <WiFiUdp.h>
#include <coap.h>
#include <U8x8lib.h>

//WiFi parameters
const char* ssid = "MikroTik GUEST";
const char* password = "xxxxxxxx";

//the display OLED used
U8X8_SSD1306_128X64_NONAME_SW_I2C u8g(/* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);

//CoAP server endpoint url callback
void callback_light(CoapPacket &packet, IPAddress ip, int port);
void callback_temp(CoapPacket &packet, IPAddress ip, int port);
void callback_hum(CoapPacket &packet, IPAddress ip, int port);
void callback_pr(CoapPacket &packet, IPAddress ip, int port);
void callback_dp(CoapPacket &packet, IPAddress ip, int port);
void callback_irms(CoapPacket &packet, IPAddress ip, int port);
void callback_pwr(CoapPacket &packet, IPAddress ip, int port);
void callback_indtemp(CoapPacket &packet, IPAddress ip, int port);
void callback_indhum(CoapPacket &packet, IPAddress ip, int port);

//UDP and CoAP class
WiFiUDP udp;
Coap coap(udp);

//LED STATE
bool LEDSTATE;

//CoAP server endpoint URL (one for each resource, we'll see only one)
void callback_light(CoapPacket &packet, IPAddress ip, int port) {
  Serial.println("[Light] ON/OFF");

  //get payload
  char p[packet.payloadlen + 1];
  memcpy(p, packet.payload, packet.payloadlen);
  p[packet.payloadlen] = NULL;

  String message(p);
  
  //check light status
  if (message.equals("0"))
    LEDSTATE = false;
  else if (message.equals("1"))
    LEDSTATE = true;
    
  //switch on or off the light and reply to the client
  if (LEDSTATE) {
    digitalWrite(0, HIGH) ;
    coap.sendResponse(ip, port, packet.messageid, "1");
    Serial.println("Luce accesa!");
  } else {
    digitalWrite(0, LOW) ;
    coap.sendResponse(ip, port, packet.messageid, "0");
    Serial.println("Luce spenta!");
  }
}

// CoAP server endpoint URL for data
void callback_temp(CoapPacket &packet, IPAddress ip, int port) {
  //check and copy payload into support variable
  char p[packet.payloadlen + 1];
  memcpy(p, packet.payload, packet.payloadlen);
  p[packet.payloadlen] = NULL;
  String message(p);
  String temp = String(p);
  //send an ACK to the CoAP client (CON approach)
  coap.sendResponse(ip, port, packet.messageid, "ACK");
  //print data on the serial monito and on the display
  Serial.println("Temp: "+ temp);
  u8g.setCursor(0, 0);
  u8g.print("Temp:"); u8g.print(temp); u8g.print(" C");
}
//same as before
void callback_hum(CoapPacket &packet, IPAddress ip, int port) {
  char p[packet.payloadlen + 1];
  memcpy(p, packet.payload, packet.payloadlen);
  p[packet.payloadlen] = NULL;
  String message(p);
  String hum = String(p);
  coap.sendResponse(ip, port, packet.messageid, "ACK");
  Serial.println("Humidity: "+ hum);
  u8g.setCursor(0, 1);
  u8g.print("Hum:"); u8g.print(hum); u8g.print(" %");
}

void callback_pr(CoapPacket &packet, IPAddress ip, int port) {
  char p[packet.payloadlen + 1];
  memcpy(p, packet.payload, packet.payloadlen);
  p[packet.payloadlen] = NULL;
  String message(p);
  String pr = String(p);
  coap.sendResponse(ip, port, packet.messageid, "ACK");
  Serial.println("Pressure: "+ pr);
  u8g.setCursor(0, 2);
  u8g.print("Pr:"); u8g.print(pr); u8g.print(" hPa");
}

void callback_dp(CoapPacket &packet, IPAddress ip, int port) {
  char p[packet.payloadlen + 1];
  memcpy(p, packet.payload, packet.payloadlen);
  p[packet.payloadlen] = NULL;
  String message(p);
  String dp = String(p);
  coap.sendResponse(ip, port, packet.messageid, "ACK");
  Serial.println("Dew Point: "+ dp);
  u8g.setCursor(0, 3);
  u8g.print("DP:"); u8g.print(dp); u8g.print(" C");
}

void callback_irms(CoapPacket &packet, IPAddress ip, int port) {
  char p[packet.payloadlen + 1];
  memcpy(p, packet.payload, packet.payloadlen);
  p[packet.payloadlen] = NULL;
  String message(p);
  String irms = String(p);
  coap.sendResponse(ip, port, packet.messageid, "ACK");
  Serial.println("Irms: "+ irms);
  u8g.setCursor(0, 4);
  u8g.print("Irms:"); u8g.print(irms); u8g.print(" A");
}

void callback_pwr(CoapPacket &packet, IPAddress ip, int port) {
  char p[packet.payloadlen + 1];
  memcpy(p, packet.payload, packet.payloadlen);
  p[packet.payloadlen] = NULL;
  String message(p);
  String pwr = String(p);
  coap.sendResponse(ip, port, packet.messageid, "ACK");
  Serial.println("Dew Point: "+ pwr);
  u8g.setCursor(0, 5);
  u8g.print("Pwr:"); u8g.print(pwr); u8g.print(" W");
}

void callback_indtemp(CoapPacket &packet, IPAddress ip, int port) {
  char p[packet.payloadlen + 1];
  memcpy(p, packet.payload, packet.payloadlen);
  p[packet.payloadlen] = NULL;
  String message(p);
  String indtemp = String(p);
  coap.sendResponse(ip, port, packet.messageid, "ACK");
  Serial.println("Temp: "+ indtemp);
  u8g.setCursor(0, 6);
  u8g.print("Temp:"); u8g.print(indtemp); u8g.print(" C");
}

void callback_indhum(CoapPacket &packet, IPAddress ip, int port) {
  char p[packet.payloadlen + 1];
  memcpy(p, packet.payload, packet.payloadlen);
  p[packet.payloadlen] = NULL;
  String message(p);
  String indhum = String(p);
  coap.sendResponse(ip, port, packet.messageid, "ACK");
  Serial.println("Humidity: "+ indhum);
  u8g.setCursor(0, 7);
  u8g.print("Hum:"); u8g.print(indhum); u8g.print(" %");
}

//board setup
void setup() {
  //serail monito setup
  Serial.begin(115200);
  //display setup
  u8g.begin();
  u8g.setFont(u8x8_font_chroma48medium8_r);
  //WiFi setup
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  //LED State and PIN mode
  pinMode(0, OUTPUT);
  digitalWrite(0, HIGH);
  LEDSTATE = true;
  
  //Setup all CoAP server resource with a path
  Serial.println("Setup Callback Light");
  coap.server(callback_light, "light");
  //same as before
  Serial.println("Setup Callback Temp");
  coap.server(callback_temp, "temp");
  Serial.println("Setup Callback Hum");
  coap.server(callback_hum, "hum");
  Serial.println("Setup Callback pr");
  coap.server(callback_pr, "pr");
  Serial.println("Setup Callback DP");
  coap.server(callback_dp, "dp");
  Serial.println("Setup Callback irms");
  coap.server(callback_irms, "irms");
  Serial.println("Setup Callback pwr");
  coap.server(callback_pwr, "pwr");
  Serial.println("Setup Callback indtemp");
  coap.server(callback_indtemp, "indtemp");
  Serial.println("Setup Callback indhum");
  coap.server(callback_indhum, "indhum");
  
  //start coap server
  coap.start();
}

//loop
void loop() {
  delay(100);
  coap.loop();
}
