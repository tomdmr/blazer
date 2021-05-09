/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-websocket-server-arduino/
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

// Import required libraries
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include <ESPmDNS.h>


#define DEBUG_ESP_PORT Serial
#include "config.h"



bool ledState[3] = {0, 0, 0};
const int ledPins[3] = {19, 18, 17};
// Touch is on GPIO15, TOUCH3
bool touchDetected = false;
unsigned long msLastEvent;    // millis() of the last event

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


// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
DNSServer dnsServer;


void notifyClients() {
  char sString[12];
  sprintf(sString, "STATE%d%d%d", ledState[0], ledState[1], ledState[2]);
  ws.textAll(sString);
  msLastEvent = millis();
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    Serial.printf("Got cmd %s\n", data);
    int len = strlen((char*)data);
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
  }
}

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

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

String processor(const String& var){
  Serial.println(var);
  /*
  if(var == "STATE"){
    if (ledState){
      return "ON";
    }
    else{
      return "OFF";
    }
  }
  */
  return "Oops";
}

void onRequest(AsyncWebServerRequest *request){
  //Handle Unknown Request
  Serial.println("Request for host "+request->host()+", path "+request->url() );
  //request->send(404);
  request->redirect("/");
}
void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  while( !Serial ) { ; }
  // Setup LEDs
  for(int i=0; i<3; i++){
    pinMode(     ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);    
  }
  esp_sleep_enable_touchpad_wakeup();  
  touchAttachInterrupt(T3, gotTouch, TOUCH_SENS);
  if(!MDNS.begin(myName)) {
     Serial.println("Error starting mDNS");
  }
  if( setupWiFiClient(ssid, password, ledPins[1], myName) ){
    Serial.println("No WiFi found, giving up");
    goToSleep();    
  }
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
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/generate_204", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(204, "", "");
  });
  server.onNotFound(onRequest);
  // Start server
  server.begin();
  msLastEvent = millis();
}

void gotTouch(){
  touchDetected = true;
}
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

void loop() {
  ws.cleanupClients();
  digitalWrite(ledPins[0], ledState[0]);
  digitalWrite(ledPins[1], ledState[1]);
  digitalWrite(ledPins[2], ledState[2]);
  if(touchDetected){
    touchDetected = false;
    ws.textAll("TOUCH");
    DEBUG_MSG("TOUCH: %d\n", touchRead(T3));
    //Serial.printf("TOUCH: %d\n", touchRead(T3));
    msLastEvent = millis();
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
  if( millis() - msLastEvent > IDLE_TO_SLEEP * 1000L){
    Serial.printf("Loop: going to sleep, millis=%ld, msLastEvent=%ld\n", millis(), msLastEvent);
    goToSleep();
  }
  
}
