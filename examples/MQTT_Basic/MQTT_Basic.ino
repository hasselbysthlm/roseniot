\
// Exempel: WiFi_V3 + MQTT publicering
#define WIFI_V3
#define MQTT
#include <RosenIoT.h>

SQLData sdata; String debugs=""; String ip=""; HttpServer server(80);
wifi_v3 net(sdata);

// Ange dina MQTT-uppgifter
const char* MQTT_PRIMARY = "test.mosquitto.org";
const char* MQTT_SECONDARY = "broker.hivemq.com";
const char* MQTT_USER = "";
const char* MQTT_PASS = "";
int MQTT_PORT = 1883;

// Callback måste finnas i globalt namnrum
void Mqtt_Callback(char* topic, byte* payload, unsigned int length){
  Serial.print("MQTT RX "); Serial.println(topic);
}

MqttHandler mqtt(MQTT_PRIMARY, MQTT_SECONDARY, MODULE_NUMBER, MQTT_USER, MQTT_PASS, MQTT_PORT);

void setup(){
  Serial.begin(115200);
  net.setup();
  mqtt.setup();
}

void loop(){
  server.handleClient();
  if(!mqtt.reconnect()) return; // försöker loopvis
  mqtt.client().loop();
  static unsigned long t=0; if(millis()-t>5000){ t=millis(); mqtt.pub("roseniot/demo", random(0,100)/1.0); }
}
