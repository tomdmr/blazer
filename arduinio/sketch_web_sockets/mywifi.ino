// DNS server
const byte DNS_PORT = 53;
#ifndef WIFI_RETRY
#define WIFI_RETRY    40
#endif

int 
setupWiFiClient(const char* ssid, const char *passwd, const int LED, const char *hostname){

  DEBUG_MSG("Connecting to %s\n", ssid);

  if(WiFi.status() == WL_CONNECTED){
    DEBUG_MSG("Wifi was connected\n");
    WiFi.disconnect();
    while(WiFi.status() == WL_CONNECTED) {;}
  }

  if(WiFi.status() != WL_CONNECTED){
    WiFi.persistent(false);
    if( WiFi.getMode() == WIFI_OFF ){
      WiFi.mode(WIFI_STA);
    }
    // Set hostname like this:
    if( hostname){
      WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
      WiFi.setHostname(hostname);
    }
    WiFi.begin(ssid, passwd);
  }

  int wificounter = 0;
  while (WiFi.status() != WL_CONNECTED && wificounter < WIFI_RETRY) {    
    if(LED) { digitalWrite(LED, HIGH); }
    for (int i = 0; i < 250; i++) {     
      delay(1);
    }
    if (LED){ digitalWrite(LED, LOW);}
    for (int i = 0; i < 250; i++) {     
      delay(1);
    }
    Serial.print(".");       
    wificounter++;
  }
  if( wificounter >= WIFI_RETRY){
    Serial.println("Connect as Client failed");
    return 1;
  }
  delay(10);
  // We start by connecting to a WiFi network  
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  return 0;  
}

int
setupWiFiAP(const char *ssid, const char *passwd){
  DEBUG_MSG("Setting up AP for ssid %s\n", ssid);

  if(WiFi.status() == WL_CONNECTED){
    WiFi.disconnect();
    while (WiFi.status() == WL_CONNECTED) { ; }
  }

  WiFi.persistent(false);
  WiFi.mode(WIFI_OFF); 
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, passwd);
  
  delay(500); // Without delay I've seen the IP address blank
  Serial.print("AP IP address: ");
  Serial.print(WiFi.softAPmacAddress());
  Serial.println(WiFi.softAPIP());

  /* Setup the DNS server redirecting all the domains to the apIP */  
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
  
  return 0;  
}

int 
setupWiFiBoth(const char *softSSID, const char *softPasswd, const char *extSSID, const char *extPasswd,
  const char *hostname, 
    const int LED){
  DEBUG_MSG("myWiFi Both: AP is %s, extern is %s/%s\n", softSSID, extSSID, extPasswd);

  if(WiFi.status() == WL_CONNECTED){
    DEBUG_MSG("Wifi was connected\n");
    WiFi.disconnect();
    while(WiFi.status() == WL_CONNECTED) {;}
  }
  else{
    DEBUG_MSG("Wifi was not connected\n");
  }

  if(WiFi.status() != WL_CONNECTED){
    WiFi.persistent(false);
    WiFi.mode(WIFI_OFF); 
    WiFi.mode(WIFI_MODE_APSTA);
    if( hostname){
      WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
      WiFi.setHostname(hostname); //define hostname
    }
    WiFi.softAP(softSSID, softPasswd);
    WiFi.begin(extSSID, extPasswd);
  }
  DEBUG_MSG("AP IP address: %s\n", WiFi.softAPmacAddress().c_str());
  //Serial.print(WiFi.softAPmacAddress());
  //Serial.println(WiFi.softAPIP());
  /* Setup the DNS server redirecting all the domains to the apIP */  
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
  int wificounter = 0;
  while (WiFi.status() != WL_CONNECTED && wificounter < WIFI_RETRY) {    
    if(LED) { digitalWrite(LED, HIGH); }
    for (int i = 0; i < 250; i++) {     
      delay(1);
    }
    if (LED){ digitalWrite(LED, LOW);}
    for (int i = 0; i < 250; i++) {     
      delay(1);
    }
    Serial.print(".");       
    wificounter++;
  }
  if( wificounter >= WIFI_RETRY){
    DEBUG_MSG("Connect as Client failed\n");
    if(LED){
      for(int i=0; i<10; i++){
        digitalWrite(LED, HIGH);
        delay(50);
        digitalWrite(LED, LOW);
        delay(50);
      }
    }
    return 1;
  }
  delay(10);
  // We start by connecting to a WiFi network  
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  return 0;
}
