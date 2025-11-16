\
// Exempel: WiFi_V3 + WEBSERVER HTML-sidor
#define WIFI_V3
#define WEBSERVER
#include <RosenIoT.h>

SQLData sdata; String debugs=""; String ip=""; HttpServer server(80);
wifi_v3 net(sdata);
webservern ui;

void setup(){
  Serial.begin(115200);
  net.setup();
  // Demo: sätt lite sensordata
  sdata.s1=21.5; sdata.s2=55; sdata.s3=0.0; sdata.s4=0.0; sdata.s5=0.0;
  ui.setup();
}

void loop(){ server.handleClient(); }
