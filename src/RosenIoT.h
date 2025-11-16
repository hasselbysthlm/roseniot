\
#pragma once
#include <Arduino.h>

// --- Kortspecifika nätverk/webserverklasser ---
#if defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESP8266WebServer.h>
  #include <WiFiClientSecureBearSSL.h>
  #include <ESP8266HTTPClient.h>
  using HttpServer = ESP8266WebServer;
#elif defined(ESP32)
  #include <WiFi.h>
  #include <WebServer.h>
  #include <WiFiClientSecure.h>
  #include <HTTPClient.h>
  using HttpServer = WebServer;
#else
  #include <WiFi.h>
  #include <WebServer.h>
  using HttpServer = WebServer;
#endif

#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ElegantOTA.h>
#include <vector>

#include "RosenIoT_config.h"

// Hemligheter (SSID/lösen)
#if __has_include(<RosenIoT_secrets.h>)
  #include <RosenIoT_secrets.h>
  #define ROSENIOT_HAS_SECRETS 1
#else
  // Du kan definiera ssidList/passList/numNetworks i din sketch i stället
  extern const char* ssidList[];
  extern const char* passList[];
  extern const int   numNetworks;
#endif

// Gemensamma data som används i flera moduler
struct SQLData {
  byte module = MODULE_NUMBER;
  // sensors
  float s1 = 0, s2 = 0, s3 = 0, s4 = 0, s5 = 0;
  // controls
  int c1 = 0, c2 = 0, c3 = 0, c4 = 0, c5 = 0, c6 = 0;
  bool c_update = false;
};

extern SQLData sdata;        // Deklareras i din sketch/exempel
extern String debugs;        // Samlad debugtext
extern String ip;            // Lokal IP som sträng

// Global HTTP-server (delad mellan moduler)
extern HttpServer server;    // Definieras i din sketch/exempel

// ===================== REST API =====================
#ifdef RESTAPI
class restapi {
public:
  void send(WiFiClientSecure& client, const String& json);
  void send_parameter(WiFiClientSecure& client, byte nr, const String& json);
  void send_c_parameter(WiFiClientSecure& client, byte nr, const String& json);
  void get_c_parameter(WiFiClientSecure& client, byte nr);
};
#endif

// ===================== HTTPGET (legacy DB) =====================
#ifdef HTTPGET
class httpget {
public:
  httpget(SQLData& sqldata);
  bool httpget_sensor(byte module, float s1, float s2, float s3, float s4, float s5);
  void C_upload();
  String httpsend(String serverPath);
  bool wifi_setup();
  bool wifi_setup_mini();
  void GetSetting();

private:
  SQLData& s_data;
  void sqldb_init();
  bool httpget_cmd(byte module, int cmd, int value1);
  String macaddress();
  int GetConfig(byte c_number);
  void APIsetting();
};
#endif

// ===================== WEBSERVER (HTML UI) =====================
#ifdef WEBSERVER
extern const char index_html[] PROGMEM;
extern const char info_html[] PROGMEM;
extern const char set_html[] PROGMEM;

class webservern {
public:
  void setup() {
    server.on("/", HTTP_GET, [this]() { sendDynamicString(index_html); });
    server.on("/setting", HTTP_GET, [this]() { sendDynamicString(set_html); });
    server.on("/info", HTTP_GET, [this]() { sendDynamicString(info_html); });
    server.on("/set", HTTP_GET, [this]() {
      if (server.hasArg("c") && server.hasArg("v")) {
        int c = server.arg("c").toInt();
        int v = server.arg("v").toInt();
        switch (c) {
          case 1: sdata.c1 = v; break; case 2: sdata.c2 = v; break; case 3: sdata.c3 = v; break;
          case 4: sdata.c4 = v; break; case 5: sdata.c5 = v; break; case 6: sdata.c6 = v; break;
          default: break;
        }
        sdata.c_update = true; server.send(200, "text/html", "Set Done");
      } else {
        server.send(400, "text/html", "Missing Parameters");
      }
    });
    server.on("/get", HTTP_GET, [this]() {
      if (server.hasArg("s")) {
        float svar = 0.0; int s = server.arg("s").toInt();
        switch (s) { case 1: svar=sdata.s1; break; case 2: svar=sdata.s2; break; case 3: svar=sdata.s3; break; case 4: svar=sdata.s4; break; case 5: svar=sdata.s5; break; default: server.send(400, "text/html", "Invalid Parameter"); return; }
        server.send(200, "text/html", String(svar));
      } else { server.send(400, "text/html", "Missing Parameters"); }
    });
  }
private:
  void sendDynamicString(const char* dynamicString);
};
#endif

// ===================== Temp I2C (OneWire/Dallas) =====================
#ifdef TEMP_I2C
class TempI2C {
public:
  explicit TempI2C(int pin) : _pin(pin), oneWire(pin), sensors(&oneWire) {}
  void setup() { pinMode(_pin, INPUT_PULLUP); sensors.begin(); }
  float Temp() { sensors.requestTemperatures(); return sensors.getTempCByIndex(0); }
private:
  int _pin; OneWire oneWire; DallasTemperature sensors;
};
#endif

// ===================== Foto (analog) =====================
#ifdef FOTO
class Foto {
public:
  Foto(byte VSS, byte Data) : _VSSpin(VSS), _Datapin(Data) {}
  void setup() { pinMode(_VSSpin, OUTPUT); digitalWrite(_VSSpin, HIGH); }
  int value() { int value = analogRead(_Datapin); return map(value, 0, 4095, 0, 100); }
private:
  byte _VSSpin, _Datapin;
};
#endif

// ===================== WWW Time (NTP) =====================
#ifdef WWWTIME
class wwwTime {
public:
  wwwTime() : timeClient(ntpUDP, "europe.pool.ntp.org", 7200, 60000) {}
  void begin() { timeClient.begin(); }
  byte getHour()   { update(); return timeClient.getHours(); }
  byte getMinute() { update(); return timeClient.getMinutes(); }
  byte getSeconds(){ update(); return timeClient.getSeconds(); }
  void setAlarm(byte hour, byte min, byte sec){ AlarmHour=hour; AlarmMin=min; AlarmSec=sec; AlarmActive=true; }
  bool checkAlarm(){ if(AlarmActive){ update(); if(AlarmHour==timeClient.getHours() && AlarmMin==timeClient.getMinutes() && AlarmSec<=timeClient.getSeconds()){ AlarmActive=false; return true; }} return false; }
  void activateAlarm(bool active){ AlarmActive=active; }
private:
  WiFiUDP ntpUDP; NTPClient timeClient; byte AlarmHour=0, AlarmMin=0, AlarmSec=0; bool AlarmActive=false;
  void update(){ timeClient.update(); }
};
#endif

// ===================== WIFI V3 (multi-SSID + local API) =====================
#ifdef WIFI_V3
class wifi_v3 {
public:
  explicit wifi_v3(SQLData& sqldata);
  bool setup();
  bool reconnectWiFi();
  String http(String serverPath);
private:
  SQLData& s_data; String mac;
  bool connectToKnownWiFi(unsigned long timeout);
  String macaddress();
  void localAPIsetting();
};
#endif

// ===================== MQTT =====================
#ifdef MQTT
class MqttHandler {
public:
  MqttHandler(const char* primaryServer, const char* secondaryServer, byte moduleId, const char* mqttUser, const char* mqttPassword, int port);
  void setup();
  bool sub(const char* topic);
  void pub(const char* topic, float payload);
  void pubtext(const char* topic, const char* payload);
  bool reconnect();
  PubSubClient& client();
private:
#if defined(ESP32)
  WiFiClientSecure espClient;
#elif defined(ESP8266)
  BearSSL::WiFiClientSecure espClient;
#else
  WiFiClient espClient;
#endif
  PubSubClient mqtt_client{espClient};
  const char* primary_server; const char* secondary_server; byte module_id; const char* mqtt_user; const char* mqtt_password; int mqtt_port; int mqtt_restart; String subscribedTopics[10]; int numSubscribed; int lastWorkingServerIndex;
};
#endif

// ===================== MyTimer =====================
class MyTimer {
public:
  explicit MyTimer(int delaySeconds): delayS(delaySeconds) { start(); }
  bool check(){ if(millis() >= startMillis + delayS*1000UL){ start(); return true;} return false; }
  void set(int delaySeconds){ delayS=delaySeconds; start(); }
  void start(){ startMillis = millis(); }
private:
  int delayS; unsigned long startMillis=0;
};
