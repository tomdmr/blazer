/*********  
 *   Some parts were inspired by Rui Santos
 *   https://RandomNerdTutorials.com/esp32-websocket-server-arduino/
 *   
 *   WiFi Strategy:
 *     1 Read hostname, SSID and Password from preferences
 *     2 Try to connect and start AP at the same time
 *     3 If connection successful: Shutdown AP and do further inits
 *     4 If not: Keep AP open for some time. 
 *     5 If user enters data: Write to preferences and goto 2
 *     6 Shutdown AP and go to sleep
*********/
#define DEBUG_ESP_PORT Serial
#include "config.h"

// Import required libraries
#include <WiFi.h>
#ifdef WITH_UDP
#  include <WiFiUdp.h>
#endif

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include <ESPmDNS.h>
#include <Preferences.h>





bool ledState[3] = {0, 0, 0};
const int ledPins[3] = {19, 18, 17};
// Touch is on GPIO15, TOUCH3
bool touchDetected = false;



// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
DNSServer dnsServer;
Preferences preferences;
bool isAP   = true;
bool tryExt = false;

unsigned long msLastEvent;    // millis() of the last event
unsigned long msLastTouch;    // millis() of the last event
unsigned long msAPActivity;


#ifdef WITH_UDP
WiFiUDP udp;
const char * udpAddress = "192.168.178.255";
const int udpPort = 1304;
char packetBuffer[255]; //buffer to hold incoming packet
char  ReplyBuffer[] = "acknowledged";       // a string to send back
#endif

#ifdef WITH_WATCHDOG
// Timer section
volatile int interruptCounter;
int totalInterruptCounter;
hw_timer_t *timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
/*
 * Timer Service routine
 */
void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  interruptCounter++;
  portEXIT_CRITICAL_ISR(&timerMux);
 
}
#endif

/************************************************************************/
void gotTouch(){
  touchDetected = true;
}
/************************************************************************/
void goToSleep(){
  DEBUG_MSG("Going to sleep now\n");
  ws.textAll("CLOSE");
  // blink 4 seconds to give glients time to close
  // the connection
  for(int i=0; i<4; i++){
    digitalWrite(ledPins[2], HIGH);
    delay(100);
    digitalWrite(ledPins[2], LOW);
    delay(900);
  }
  server.end();
  esp_deep_sleep_start();
}
/************************************************************************/
void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason){
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}
/************************************************************************/
void notifyClients() {
  char sString[256];
  sprintf(sString, "STATE%d%d%d %lu %d", ledState[0], ledState[1], ledState[2], millis(), touchRead(T3));
  ws.textAll(sString);
  msLastEvent = millis();
}
/************************************************************************/
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    DEBUG_MSG("Got cmd %s\n", data);
    int len = strlen((char*)data);
#if 0    
    if (strcmp((char*)data, "STATE") == 0) {
      notifyClients();
    }
    if((len>3)&&(strncmp((char*)data, "set", 3)==0)){
      int s = data[3]-'0';
      if(s>=0 && s<3){
        ledState[s] = 1;
      }
      notifyClients();
    }
    else if((len>3)&&(strncmp((char*)data, "clr", 3)==0)){
      int s = data[3]-'0';
      if(s>=0 && s<3){
        ledState[s] = 0;
      }
      notifyClients();
    }
    else if((len>6)&&(strncmp((char*)data, "SET",3)==0)){
      for(int i=0; i<3; i++){
        int s = data[3+i];
        ledState[i] = s ? 1 : 0;
      }
      notifyClients();
    }
    else if(strcmp((char*)data, "CLR")==0){
      ledState[0] = ledState[1] = ledState[2]=0;
      notifyClients();
    }
#else
    char *req = (char *)data;
    if     (!strncmp(req, "STATE", 4)){
      notifyClients();
    }
    else if(!strncmp(req, "set", 3)){
      req += 3;
      int v = atoi(req) & 0x03;
      ledState[v] = 1;
      notifyClients();
    }
    else if(!strncmp(req, "clr", 3)){
      req += 3;
      int v = atoi(req) & 0x03;
      ledState[v] = 0;
      notifyClients();
    }
    else if((len==6) && !strncmp(req, "SET", 3)){
      req += 3;
      for(int i=0; i<3; i++){
        int s = (int)req[i]-(int)'0';
        ledState[i] = s ? 1 : 0;
      }
      notifyClients();
      
    }
    else if(!strncmp(req, "CLR", 3)){
      ledState[0] = ledState[1] = ledState[2]=0;
      notifyClients();
    }
    else if(!strncmp(req, "GTT", 3)){
      char rsp[256];
      sprintf(rsp, "%d", touchSens);
      ws.textAll(rsp);
      msLastEvent = millis();      
    }
    else if(!strncmp(req, "STT", 3)){
      req +=3;
      int newT = atoi(req);
      if((newT>0) && (newT<256)){
        touchSens = newT;
        savePreferences();
      }
      msLastEvent = millis();      
    }
#endif
  }
}
/************************************************************************/
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
      Serial.printf("PONG\n");
      break;
    case WS_EVT_ERROR:
      Serial.printf("ERROR\n");
      break;
  }
}
/************************************************************************/
void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}
/************************************************************************/
int tryConnectWiFi(){
  DEBUG_MSG("WiFi: AP with %s, WiFi to %s\n", myName, extSSID);
  int res = setupWiFiBoth(myName, "", extSSID, extPasswd, myName, ledPins[1]);
  if( res ){
    setupWiFiAP(myName, "");
    // FIXME: StartScanning
    DEBUG_MSG("Connect failed, keeping AP open\n");
    msAPActivity = millis();
    isAP = true;
  }
  else{
    // Connect OK. Shutdown AP
    DEBUG_MSG("Connect to %s as %s OK, shutting down AP\n", extSSID, myName);
    isAP = false;
    WiFi.enableAP(false);
    // And save preferences
    savePreferences();
    if(!MDNS.begin(myName)) {
       Serial.println("Error starting mDNS");
    }
    MDNS.addService("http", "tcp", 80);
#ifdef WITH_UDP
    udp.begin(WiFi.localIP(),udpPort);  
#endif    
  }
  return res;
}
/************************************************************************/
/************************************************************************/
/************************************************************************/
void setup(){
  Serial.begin(115200);
  while( !Serial ) { ; }
  loadPreferences();

  // Setup LEDs
  for(int i=0; i<3; i++){
    pinMode(     ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);    
  }
  esp_sleep_enable_touchpad_wakeup();  
  touchAttachInterrupt(T3, gotTouch, touchSens);
#if 0  
  if( setupWiFiClient(ssid, password, ledPins[1], myName) ){
    Serial.println("No WiFi found, giving up");
    goToSleep();    
  }
#else
  tryConnectWiFi();
#endif  
  if(!MDNS.begin(myName)) {
     Serial.println("Error starting mDNS");
  }
  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());  

#ifdef WITH_WATCHDOG  
  // Timer
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, WATCHDOG_MSG * 1000000, true);
  timerAlarmEnable(timer);
#endif
  
  // Web-Server/Sockets
  initWebSocket();
  // Route for root / web page
  server.onNotFound(onRequest);
  server.on("/",     HTTP_ANY, onRoot);
  server.on("/blazer.css", HTTP_ANY, onCSS);
  server.on("/blazer2.js", HTTP_ANY, onBlazer2);
  server.on("/portal.html", HTTP_ANY, onPortal);
  server.on("/demo01.html", HTTP_ANY, onDemo01);
  server.on("/demo02.html", HTTP_ANY, onDemo02);
  server.on("/demo03.html", HTTP_ANY, onDemo03);
  server.on("/scan", HTTP_ANY, onScan);
  server.on("/devhome", HTTP_ANY, onDevHome);
  server.on("/generate_204", HTTP_ANY, on204);
  // Start server
  server.begin();
  msAPActivity = msLastTouch = msLastEvent = millis();
}
/************************************************************************/
/************************************************************************/
void loop() {
  // Always
  ws.cleanupClients();
  digitalWrite(ledPins[0], ledState[0]);
  digitalWrite(ledPins[1], ledState[1]);
  digitalWrite(ledPins[2], ledState[2]);

#ifdef WITH_UDP
  int packetSize = udp.parsePacket();
  if (packetSize) {
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remoteIp = udp.remoteIP();
    Serial.print(remoteIp);
    Serial.print(", port ");
    Serial.println(udp.remotePort());
    int len = udp.read(packetBuffer, 255);
    if (len > 0) {
      packetBuffer[len] = 0;
    }
    Serial.println("Contents:");
    Serial.println(packetBuffer);  
    udp.beginPacket(udp.remoteIP(), udp.remotePort());
    udp.write((const uint8_t*)ReplyBuffer, sizeof(ReplyBuffer));
    udp.endPacket();
  }
#endif  
  // if /scan got submitted
  if( tryExt ){
    DEBUG_MSG("New test on extrnal WiFi\n");
    tryExt = false;
    tryConnectWiFi();
  }
  if(touchDetected){
    touchDetected = false;
    if( millis() > msLastTouch + 50){
      char sString[255];
      sprintf(sString, "TOUCH%d%d%d %lu %d", ledState[0], ledState[1], ledState[2], millis(), touchRead(T3));
      ws.textAll(sString);
      DEBUG_MSG("TOUCH: %d %lu\n", touchRead(T3), millis());
    }
    msLastTouch = msLastEvent = millis();
  }
#ifdef WITH_WATCHDOG  
  if (interruptCounter > 0) {
    portENTER_CRITICAL(&timerMux);
    interruptCounter--;
    portEXIT_CRITICAL(&timerMux);

    totalInterruptCounter++;
    Serial.print("An interrupt as occurred. Total number: ");
    Serial.print(totalInterruptCounter);
    Serial.printf(" Touch: %d, WiFi is %d\n", touchRead(T3), WiFi.status());
  }  
#endif
  if( isAP && (millis()  > msAPActivity + AP_GRACEPERIOD)){
    Serial.println("Shutting down AP");
    isAP = false;
    WiFi.enableAP(false);
    msLastEvent = millis();
  }

  if( !isAP && (millis() > msLastEvent + IDLE_TO_SLEEP)){
    Serial.printf("Loop: going to sleep, millis=%ld, msLastEvent=%ld\n", millis(), msLastEvent);
    goToSleep();
  } 
}

void loadPreferences(){
  preferences.begin(STORAGE_SPACE, false);
  preferences.getString("hname", myName,   sizeof(myName) );
  preferences.getString("ssid",  extSSID,   sizeof(extSSID));
  preferences.getString("pswd",  extPasswd, sizeof(extPasswd));
  touchSens = preferences.getInt   ("tsns", 23);
  preferences.end();
  DEBUG_MSG("Load preferences: myName: <%s>, connect to %s/%s, touch: %d\n", myName, extSSID, extPasswd, touchSens);
}
void savePreferences(){
  preferences.begin(STORAGE_SPACE, false);
  preferences.putString("hname",  myName);
  preferences.putString("ssid", extSSID);
  preferences.putString("pswd", extPasswd);
  preferences.putInt   ("tsns", touchSens);
  preferences.end();
  DEBUG_MSG("Save preferences: myName: <%s>, connect to %s/%s, touch: %d\n", myName, extSSID, extPasswd, touchSens);
}
