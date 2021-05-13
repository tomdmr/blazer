#include "http_gz.h"
void onRequest(AsyncWebServerRequest *request) {
  //Handle Unknown Request
  Serial.println("Request for host " + request->host() + ", path " + request->url() );
  request->send(404,"text/html", "Request for host " + request->host() + ", path " + request->url());
  //request->redirect("/");
}

void on204(AsyncWebServerRequest *request) {
  Serial.println("generate 204 Request for host " + request->host() + ", path " + request->url() );
  request->send_P(204, "", "");
  //request->redirect("/");
}

void onRoot(AsyncWebServerRequest *request) {
  Serial.println("/ Request for host " + request->host() + ", path " + request->url() );
  request->send_P(200, "text/html", index_html, processor);
  /*
  AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Hello World!");
  response->addHeader("Server", "ESP Async Web Server");
  request->send(response);
  */
}

/* We need something smarter here...*/
void onPortal(AsyncWebServerRequest *request) {
  Serial.println("Request for /portal");
  AsyncWebServerResponse *response = 
  request->beginResponse_P(200, "text/html", portal_html_gz, portal_html_gz_len);
  response->addHeader("Content-Encoding", "gzip");
  request->send(response);
}
void onBlazer2(AsyncWebServerRequest *request) {
  Serial.println("Request for /blazer2.js");
  AsyncWebServerResponse *response = 
  request->beginResponse_P(200, "text/script", blazer2_js_gz, blazer2_js_gz_len);
  response->addHeader("Content-Encoding", "gzip");
  request->send(response);
}
void onDemo01(AsyncWebServerRequest *request) {
  Serial.println("Request for /demo01");
  AsyncWebServerResponse *response = 
  request->beginResponse_P(200, "text/html", demo01_html_gz, demo01_html_gz_len);
  response->addHeader("Content-Encoding", "gzip");
  request->send(response);
}
void onDemo02(AsyncWebServerRequest *request) {
  Serial.println("Request for /demo02");
  AsyncWebServerResponse *response = 
  request->beginResponse_P(200, "text/html", demo02_html_gz, demo02_html_gz_len);
  response->addHeader("Content-Encoding", "gzip");
  request->send(response);
}

void onCSS(AsyncWebServerRequest *request) {
  Serial.println("Request for /blazer.css");
  AsyncWebServerResponse *response = 
  request->beginResponse_P(200, "text/css", blazer_css_gz, blazer_css_gz_len);
  response->addHeader("Content-Encoding", "gzip");
  request->send(response);
}
void onScan(AsyncWebServerRequest *request) {
  Serial.println("onScan Request");
  if( ON_AP_FILTER(request) ){ msAPActivity = millis(); }
  if ( (request->hasParam("NW", true)) && (request->hasParam("PASSWD", true)) && (request->hasParam("HNAME", true))) {
    Serial.println("Looks good");
    String NW = "" + request->getParam("NW", true)->value();
    String pw = "" + request->getParam("PASSWD", true)->value();
    String hname = "" + request->getParam("HNAME", true)->value();
    strncpy(extSSID,   request->getParam("NW", true)->value().c_str(), sizeof(extSSID));
    strncpy(extPasswd, request->getParam("PASSWD", true)->value().c_str(), sizeof(extPasswd));
    strncpy(myName,    request->getParam("HNAME", true)->value().c_str(), sizeof(myName));
    Serial.printf("Saving hname as %s\n", hname.c_str());
    preferences.begin(STORAGE_SPACE, false);
    savePreferences();
    preferences.end();
    //tryExt    = true;
    Serial.printf("Have params %s and %s\n", NW.c_str(), pw.c_str());
    request->send(200, "text/html", "OK" );
  }
  else if ( (request->hasParam("HNAME", true)) ) {
  }
  else {
    Serial.println("Request for host " + request->host() + ", path " + request->url() );
    String rsp  = "<!DOCTYPE HTML><html lang='de'><head><title>Config Page</title>"
        "<meta name='viewport' content='width=device-width, initial-scale=1'>"
        "<meta http-equiv='Content-Type' content='text/html; charset=utf-8'>"
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
