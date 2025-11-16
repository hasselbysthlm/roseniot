\
// Exempel: WiFi_V3 + lokal /data-API
#define WIFI_V3
#include <RosenIoT.h>

// Obligatoriska globala definitioner som biblioteket använder
SQLData sdata;      // data
String debugs = ""; // debuglogg
String ip = "";     // fylls i vid uppkoppling
HttpServer server(80);

// Hemligheter: skapa en fil i din sketchmapp med namnet RosenIoT_secrets.h
// och klistra in innehållet från RosenIoT_secrets_template.h (justerat för dina nätverk).

wifi_v3 net(sdata);

void setup(){
  Serial.begin(115200);
  delay(1000);
  net.setup();
}

void loop(){
  server.handleClient();
}
