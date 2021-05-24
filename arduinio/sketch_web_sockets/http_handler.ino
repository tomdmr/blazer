#include "http_gz.h"
void onRequest(AsyncWebServerRequest *request) {
  //Handle Unknown Request
  //FIXME: This is a printf DEBUG_MSG("Request for host " + request->host() + ", path " + request->url() );
  request->send(404,"text/html", "Request for host " + request->host() + ", path " + request->url());
  //request->redirect("/");
}
void onBlazer2(AsyncWebServerRequest *request) {
  DEBUG_MSG("Request for /blazer2.js\n");
  AsyncWebServerResponse *response = 
  request->beginResponse_P(200, "text/javascript", blazer2_js_gz, blazer2_js_gz_len);
  response->addHeader("Content-Encoding", "gzip");
  request->send(response);
}
void onCSS(AsyncWebServerRequest *request) {
  DEBUG_MSG("Request for /blazer.css");
  AsyncWebServerResponse *response = 
  request->beginResponse_P(200, "text/css", blazer_css_gz, blazer_css_gz_len);
  response->addHeader("Content-Encoding", "gzip");
  request->send(response);
}


void on204(AsyncWebServerRequest *request) {
  //FIXME: This is a printf DEBUG_MSG("generate 204 Request for host " + request->host() + ", path " + request->url() );
  request->send_P(204, "", "");
  //request->redirect("/");
}
void sendZipped(AsyncWebServerRequest *request, const uint8_t data[], const uint16_t dlen){
  AsyncWebServerResponse *response = 
  request->beginResponse_P(200, "text/html", data, dlen);
  response->addHeader("Content-Encoding", "gzip");
  request->send(response);  
}
void onRoot(AsyncWebServerRequest *request) {
  DEBUG_MSG("Request for /");
  sendZipped(request, portal_html_gz, portal_html_gz_len);
}

/* We need something smarter here...*/
void onPortal(AsyncWebServerRequest *request) {
  DEBUG_MSG("Request for /portal");
  sendZipped(request, portal_html_gz, portal_html_gz_len);
}
void onDemo01(AsyncWebServerRequest *request) {
  DEBUG_MSG("Request for /demo01");
  sendZipped(request, demo01_html_gz, demo01_html_gz_len);
}
void onDemo02(AsyncWebServerRequest *request) {
  DEBUG_MSG("Request for /demo02");
  sendZipped(request, demo02_html_gz, demo02_html_gz_len);
}
void onDemo03(AsyncWebServerRequest *request) {
  DEBUG_MSG("Request for /demo02");
  sendZipped(request, demo03_html_gz, demo03_html_gz_len);
}

void onScan(AsyncWebServerRequest *request) {
  DEBUG_MSG("onScan Request\n");
  if( ON_AP_FILTER(request) ){ msAPActivity = millis(); }
  if ( (request->hasParam("NW", true)) && (request->hasParam("HNAME", true))) {
    DEBUG_MSG("Looks good\n");
    String NW, pw, hname;
    NW = "" + request->getParam("NW", true)->value();
    if( request->getParam("PASSWD", true) ){
      pw = "" + request->getParam("PASSWD", true)->value();
    }
    else{
      pw = "";
    }
    hname = "" + request->getParam("HNAME", true)->value();
    strncpy(extSSID,   request->getParam("NW", true)->value().c_str(), sizeof(extSSID));
    strncpy(extPasswd, request->getParam("PASSWD", true)->value().c_str(), sizeof(extPasswd));
    strncpy(myName,    request->getParam("HNAME", true)->value().c_str(), sizeof(myName));
    DEBUG_MSG("TryWiFi on %s/%s and hName %s\n", extSSID, extPasswd, myName);
    request->send(200, "text/html", "OK, try to connect" );
    tryExt = true;
  }
  else if ( (request->hasParam("HNAME", true)) ) {
    String  hname = "" + request->getParam("HNAME", true)->value();
    strncpy(myName, hname.c_str(), sizeof(myName));
    DEBUG_MSG("Changing name to %s\n", myName);
    request->send(200, "text/html", "OK, try to connect" );
    tryExt = true;
  }
  else {
    Serial.println("Request for host " + request->host() + ", path " + request->url() );
    String rsp  = "<!DOCTYPE HTML><html lang='de'><head><title>Config Page</title>"
        "<meta name='viewport' content='width=device-width, initial-scale=1'>"
        "<meta http-equiv='Content-Type' content='text/html; charset=utf-8'>"
        "<script src='blazer2.js'></script>"
        "</head>"
        "<body><h1>ESP WLAN Config</h1><form action='/scan' method='post'>"
        "<table>"
        "<tr><td>My Name</td><td><input type='text' name='HNAME' value='" + String(myName) + "' /></td></tr>";
    int n = WiFi.scanComplete();
    if (n == -2) {
      WiFi.scanNetworks(true);
    } else if (n) {
      for (int i = 0; i < n; ++i) {
        rsp += "<tr><td>" + String(i) + "</td>"
               "<td>" + WiFi.SSID(i) + "</td>"
               "<td><input type='radio' name='NW' value='" + WiFi.SSID(i) + "'/></td>"
               "<td>" + String(WiFi.RSSI(i)) + "</td></tr>";
      }
      rsp += "<tr><td>Password</td><td><input type='password' name='PASSWD'/></td></tr>";
      WiFi.scanDelete();
      if (WiFi.scanComplete() == -2) {
        WiFi.scanNetworks(true);
      }
    }
    rsp +=
      "</table><input type='submit' value='Submit'></form></body></html>";
    request->send(200, "text/html", rsp);
    rsp = String();
  }
}
void onDevHome(AsyncWebServerRequest *request) {
  DEBUG_MSG("Request for /devhome");
  strcpy(extSSID, "WLAN 7360");
  strcpy(extPasswd, "41209353118149546763");
  request->send(200, "text/html", "OK, try to connect" );
  tryExt = true;
}
