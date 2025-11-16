\
#include "RosenIoT.h"
// Flyttade implementationer som inte bör ligga i header för att minska header-tyngd.

// Definiera HTML-strängar som tidigare låg i header (PROGMEM)
const char index_html[] PROGMEM = R"rawliteral(
# WEMOS %modulename%
## WEMOS %modulenr% %modulename%

Links:
[ Upload ](/update)
[ Setting ](/setting) [ Info ](/info)
/get?s=sensor index(1-5)
/get?c=index index(1-6)
/set?c=index (1-6)&v=value (int)
/debug
/data
/sleep
/nosleep
/reset
)rawliteral";

const char info_html[] PROGMEM = R"rawliteral(
# Sensor data
## Sensor %modulenr% %modulename%
<table>
<tr><th>Id</th><th>Name</th><th>Value</th></tr>
<tr><td>s1</td><td>%s1name%</td><td>%s1%</td></tr>
<tr><td>s2</td><td>%s2name%</td><td>%s2%</td></tr>
<tr><td>s3</td><td>%s3name%</td><td>%s3%</td></tr>
<tr><td>s4</td><td>%s4name%</td><td>%s4%</td></tr>
<tr><td>s5</td><td>%s5name%</td><td>%s5%</td></tr>
<tr><td>c1</td><td>%c1name%</td><td>%c1%</td></tr>
<tr><td>c2</td><td>%c2name%</td><td>%c2%</td></tr>
<tr><td>c3</td><td>%c3name%</td><td>%c3%</td></tr>
<tr><td>c4</td><td>%c4name%</td><td>%c4%</td></tr>
<tr><td>c5</td><td>%c5name%</td><td>%c5%</td></tr>
<tr><td>c6</td><td>%c6name%</td><td>%c6%</td></tr>
</table>

[ Meny ](/) [ Setting ](/setting) %debugs%
)rawliteral";

const char set_html[] PROGMEM = R"rawliteral(
# Setting
Cmd: <input id="c" type="number" min="1" max="6"/>
Value: <input id="v" type="number"/>
<button onclick="setv()">Set</button>
<script>
function setv(){var c=document.getElementById('c').value;var v=document.getElementById('v').value;fetch('/set?c='+c+'&v='+v).then(r=>r.text()).then(t=>alert(t));}
</script>
[ Meny ](/)
)rawliteral";

// Implementera metoden som skickar den dynamicska HTML:n (flyttad från header)
void webservern::sendDynamicString(const char* dynamicString) {
	String processedString = dynamicString;
	processedString.replace("%modulenr%", String(MODULE_NUMBER));
	processedString.replace("%modulename%", MODULE_NAME);
	processedString.replace("%s1name%", S1_NAME); processedString.replace("%s2name%", S2_NAME);
	processedString.replace("%s3name%", S3_NAME); processedString.replace("%s4name%", S4_NAME);
	processedString.replace("%s5name%", S5_NAME);
	processedString.replace("%c1name%", C1_NAME); processedString.replace("%c2name%", C2_NAME);
	processedString.replace("%c3name%", C3_NAME); processedString.replace("%c4name%", C4_NAME);
	processedString.replace("%c5name%", C5_NAME); processedString.replace("%c6name%", C6_NAME);
	processedString.replace("%s1%", String(sdata.s1)); processedString.replace("%s2%", String(sdata.s2));
	processedString.replace("%s3%", String(sdata.s3)); processedString.replace("%s4%", String(sdata.s4));
	processedString.replace("%s5%", String(sdata.s5));
	processedString.replace("%c1%", String(sdata.c1)); processedString.replace("%c2%", String(sdata.c2));
	processedString.replace("%c3%", String(sdata.c3)); processedString.replace("%c4%", String(sdata.c4));
	processedString.replace("%c5%", String(sdata.c5)); processedString.replace("%c6%", String(sdata.c6));
	processedString.replace("%debugs%", debugs);
	server.send(200, "text/html", processedString);
}

// Medvetet tom: andra globala objekt (server, sdata, debugs, ip) definieras i exempel/sketch.

#ifdef RESTAPI
void restapi::send(WiFiClientSecure& client, const String& json) {
	HTTPClient httpapi;
	String fullUrl = String(APIURL) + "/data";
#if defined(ESP8266)
	client.setInsecure();
#endif
	httpapi.begin(client, fullUrl);
	httpapi.addHeader("Content-Type", "application/json");
	httpapi.addHeader("accept", "application/json");
	int httpResponseCode = httpapi.POST(json);
	if (httpResponseCode > 0) {
		String response = httpapi.getString();
		debugs = String("\nRESTAPI data: ") + json + " - RESTAPI: " + String(httpResponseCode) + ", resp: " + response + debugs;
	} else {
		debugs = String("\nRESTAPI SEND ERROR: ") + String(httpResponseCode) + debugs;
	}
	httpapi.end();
}

void restapi::send_parameter(WiFiClientSecure& client, byte nr, const String& json) {
	HTTPClient httpapi;
	String fullUrl = String(APIURL) + "/para/" + String(nr);
#if defined(ESP8266)
	client.setInsecure();
#endif
	httpapi.begin(client, fullUrl);
	httpapi.addHeader("Content-Type", "application/json");
	httpapi.addHeader("accept", "application/json");
	int httpResponseCode = httpapi.PUT(json);
	if (httpResponseCode <= 0) {
		debugs = String("\nSEND PARA ERROR: ") + String(httpResponseCode) + debugs;
	}
	httpapi.end();
}

void restapi::send_c_parameter(WiFiClientSecure& client, byte nr, const String& json) {
	HTTPClient httpapi;
	String fullUrl = String(APIURL) + "/c_para/" + String(nr);
#if defined(ESP8266)
	client.setInsecure();
#endif
	httpapi.begin(client, fullUrl);
	httpapi.addHeader("Content-Type", "application/json");
	httpapi.addHeader("accept", "application/json");
	int httpResponseCode = httpapi.PUT(json);
	if (httpResponseCode <= 0) {
		debugs = String("\nSEND C ERROR ") + String(httpResponseCode) + debugs;
	}
	httpapi.end();
}

void restapi::get_c_parameter(WiFiClientSecure& client, byte nr) {
	if (WiFi.status() != WL_CONNECTED) return;
	HTTPClient httpapi;
	String fullUrl = String(APIURL) + "/c_para/" + String(nr);
#if defined(ESP8266)
	client.setInsecure();
#endif
	httpapi.begin(client, fullUrl);
	httpapi.addHeader("Content-Type", "application/json");
	httpapi.addHeader("accept", "application/json");
	int code = httpapi.GET();
	if (code > 0) {
		String response = httpapi.getString();
		DynamicJsonDocument doc(512);
		if (deserializeJson(doc, response) == DeserializationError::Ok) {
			if (doc.containsKey("c1") && !doc["c1"].isNull()) sdata.c1 = doc["c1"].as<int>();
			if (doc.containsKey("c2") && !doc["c2"].isNull()) sdata.c2 = doc["c2"].as<int>();
			if (doc.containsKey("c3") && !doc["c3"].isNull()) sdata.c3 = doc["c3"].as<int>();
			if (doc.containsKey("c4") && !doc["c4"].isNull()) sdata.c4 = doc["c4"].as<int>();
			if (doc.containsKey("c5") && !doc["c5"].isNull()) sdata.c5 = doc["c5"].as<int>();
			if (doc.containsKey("c6") && !doc["c6"].isNull()) sdata.c6 = doc["c6"].as<int>();
			debugs = String("\nC-Parameter RX: ") + response + debugs;
		} else {
			debugs = String("\nGET JSON Parsing Failed") + debugs;
		}
	} else {
		debugs = String("\nGET request error: ") + String(code) + debugs;
	}
	httpapi.end();
}
#endif

#ifdef HTTPGET
httpget::httpget(SQLData& sqldata) : s_data(sqldata) {}

bool httpget::httpget_sensor(byte module, float s1, float s2, float s3, float s4, float s5) {
	String www = String(DBURL) + "add.php?db=S" + String(module) +
							 "&s1=" + String(s1) + "&s2=" + String(s2) + "&s3=" + String(s3) +
							 "&s4=" + String(s4) + "&s5=" + String(s5);
	return httpsend(www) != "NACK";
}

void httpget::C_upload() {
	httpget_cmd(sdata.module, 1, sdata.c1);
	httpget_cmd(sdata.module, 2, sdata.c2);
	httpget_cmd(sdata.module, 3, sdata.c3);
	httpget_cmd(sdata.module, 4, sdata.c4);
	httpget_cmd(sdata.module, 5, sdata.c5);
	httpget_cmd(sdata.module, 6, sdata.c6);
	sdata.c_update = false;
}

String httpget::httpsend(String serverPath) {
	HTTPClient httpC;
	String payload;
#if defined(ESP8266) || defined(ESP32)
	WiFiClient client;
	httpC.begin(client, serverPath);
#else
	httpC.begin(serverPath);
#endif
	int httpResponseCode = httpC.GET();
	if (httpResponseCode > 0) {
		payload = httpC.getString();
		debugs += String(" - HTTPGET code: ") + String(httpResponseCode) + ", payload: " + payload;
	} else {
		debugs += String(" - HTTPGET code: ") + String(httpResponseCode);
		payload = "NACK";
	}
	httpC.end();
	return payload;
}

bool httpget::wifi_setup() {
	byte i = 0;
	WiFi.mode(WIFI_STA);
#if defined(WIFI_SSID) && defined(WIFI_PASSWORD)
	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
#else
	return false; // Kräver antingen WIFI_SSID/WIFI_PASSWORD eller wifi_v3
#endif
	while (WiFi.status() != WL_CONNECTED && i <= 10) {
		delay(500);
		i++;
	}
	if (i >= 10) return false;
	ElegantOTA.begin(&server);
	APIsetting();
	server.begin();
	sqldb_init();
	return true;
}

bool httpget::wifi_setup_mini() {
	byte i = 0;
#if defined(WIFI_SSID) && defined(WIFI_PASSWORD)
	WiFi.mode(WIFI_STA);
	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
#else
	return false;
#endif
	while (WiFi.status() != WL_CONNECTED && i <= 10) { delay(300); i++; }
	if (i >= 10) return false;
	GetSetting();
	return true;
}

void httpget::GetSetting() {
	int answer = GetConfig(1); if(answer != -1) sdata.c1 = answer;
	answer = GetConfig(2); if(answer != -1) sdata.c2 = answer;
	answer = GetConfig(3); if(answer != -1) sdata.c3 = answer;
	answer = GetConfig(4); if(answer != -1) sdata.c4 = answer;
	answer = GetConfig(5); if(answer != -1) sdata.c5 = answer;
	answer = GetConfig(6); if(answer != -1) sdata.c6 = answer;
}

void httpget::sqldb_init() {
	IPAddress ipAddress = WiFi.localIP();
	ip = String(ipAddress[0]) + "." + String(ipAddress[1]) + "." + String(ipAddress[2]) + "." + String(ipAddress[3]);
	httpsend(String(DBURL) + "setsensorlist.php?nr=" + String(MODULE_NUMBER) + "&db=S" + String(MODULE_NUMBER) + "&mac=" + macaddress() + "&ip=" + ip + "&name=" + MODULE_NAME + "&s1=" + S1_NAME + "&s2=" + S2_NAME + "&s3=" + S3_NAME + "&s4=" + S4_NAME + "&s5=" + S5_NAME);
	httpsend(String(DBURL) + "setsensorlist.php?nr=" + String(MODULE_NUMBER) + "&c1name=" + C1_NAME + "&c2name=" + C2_NAME + "&c3name=" + C3_NAME + "&c4name=" + C4_NAME + "&c5name=" + C5_NAME + "&c6name=" + C6_NAME);
	String extIP = httpsend("http://api.ipify.org");
	httpsend(String(DBURL) + "setsensorlist.php?nr=" + String(MODULE_NUMBER) + "&extip=" + extIP + "&ssid=" + WiFi.SSID());
}

bool httpget::httpget_cmd(byte module, int cmd, int value1) {
	String www = String(DBURL) + "setsensorlist.php?nr=" + String(module) + "&c" + String(cmd) + "value=" + String(value1);
	return httpsend(www) != "NACK";
}

String httpget::macaddress() {
	byte mac[6];
	WiFi.macAddress(mac);
	char buf[18];
	snprintf(buf, sizeof(buf), "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	return String(buf);
}

int httpget::GetConfig(byte c_number) {
	String url = String(DBURL) + "getsensorlist.php?nr=" + String(MODULE_NUMBER) + "&id=c" + String(c_number) + "_value";
	String response = httpsend(url);
	if (response.length() == 0) return -1;
	return response.toInt();
}

void httpget::APIsetting() {
	server.on("/nosleep", HTTP_GET, [this]() { s_data.c6 = 0; s_data.c_update = 1; server.send(200, "text/plain", "No sleep Mode"); });
	server.on("/sleep",  HTTP_GET, [this]() { s_data.c6 = 1; s_data.c_update = 1; server.send(200, "text/plain", "Sleep Mode");   });
	server.on("/debug",  HTTP_GET, [this]() { server.send(200, "text/html", String(debugs)); });
	server.on("/reset",  HTTP_GET, [this]() { server.send(200, "text/plain", "Reset"); delay(1000); ESP.restart(); });
	server.on("/data",   HTTP_GET, [this]() {
		DynamicJsonDocument jsonDoc(500);
		jsonDoc["module"] = s_data.module;
		jsonDoc["module_number"] = MODULE_NUMBER;
		jsonDoc["module_name"] = MODULE_NAME;
		jsonDoc["ip"] = ip;
		JsonObject sensorNames = jsonDoc.createNestedObject("sensor_names");
		sensorNames["s1_name"] = S1_NAME; sensorNames["s2_name"] = S2_NAME; sensorNames["s3_name"] = S3_NAME; sensorNames["s4_name"] = S4_NAME; sensorNames["s5_name"] = S5_NAME;
		JsonObject controlNames = jsonDoc.createNestedObject("control_names");
		controlNames["c1_name"] = C1_NAME; controlNames["c2_name"] = C2_NAME; controlNames["c3_name"] = C3_NAME; controlNames["c4_name"] = C4_NAME; controlNames["c5_name"] = C5_NAME; controlNames["c6_name"] = C6_NAME;
		JsonObject sensorData = jsonDoc.createNestedObject("sensor_data");
		sensorData["s1"] = s_data.s1; sensorData["s2"] = s_data.s2; sensorData["s3"] = s_data.s3; sensorData["s4"] = s_data.s4; sensorData["s5"] = s_data.s5;
		JsonObject controlData = jsonDoc.createNestedObject("control_data");
		controlData["c1"] = s_data.c1; controlData["c2"] = s_data.c2; controlData["c3"] = s_data.c3; controlData["c4"] = s_data.c4; controlData["c5"] = s_data.c5; controlData["c6"] = s_data.c6; controlData["c_update"] = s_data.c_update;
		String jsonString; serializeJson(jsonDoc, jsonString);
		server.send(200, "application/json", jsonString);
	});
}
#endif

#ifdef WIFI_V3
wifi_v3::wifi_v3(SQLData& sqldata) : s_data(sqldata) {}

bool wifi_v3::setup() {
	WiFi.mode(WIFI_STA);
	connectToKnownWiFi(15000);
	ElegantOTA.begin(&server);
	localAPIsetting();
	server.begin();
	mac = macaddress();
	IPAddress ipAddress = WiFi.localIP();
	ip = String(ipAddress[0])+"."+String(ipAddress[1])+"."+String(ipAddress[2])+"."+String(ipAddress[3]);
	return true;
}

bool wifi_v3::reconnectWiFi(){ if (WiFi.status()!=WL_CONNECTED){ connectToKnownWiFi(5000); return WiFi.status()==WL_CONNECTED; } return true; }

String wifi_v3::http(String serverPath){ HTTPClient httpC; String payload; WiFiClient client; httpC.begin(client, serverPath); int code=httpC.GET(); if(code>0){ payload=httpC.getString(); debugs += String(" - HTTPGET code: ")+String(code)+", payload: "+payload; } else { payload="NACK"; } httpC.end(); return payload; }

bool wifi_v3::connectToKnownWiFi(unsigned long timeout){
	int n = WiFi.scanNetworks(); if(n<=0) return false;
	// sort by RSSI
	std::vector<int> idx(n); for(int i=0;i<n;i++) idx[i]=i; for(int i=0;i<n-1;i++){ for(int j=i+1;j<n;j++){ if(WiFi.RSSI(idx[j])>WiFi.RSSI(idx[i])) std::swap(idx[i],idx[j]); }}
	for(int k=0;k<n;k++){
		String scannedSSID = WiFi.SSID(idx[k]);
		for(int j=0;j<numNetworks;j++){
			if(scannedSSID == ssidList[j]){
				WiFi.begin(ssidList[j], passList[j]);
				unsigned long start = millis();
				while(WiFi.status()!=WL_CONNECTED && millis()-start<timeout){ delay(500); yield(); }
				if(WiFi.status()==WL_CONNECTED) return true;
			}
		}
	}
	return false;
}

String wifi_v3::macaddress(){ byte macb[6]; WiFi.macAddress(macb); char buf[18]; snprintf(buf, sizeof(buf), "%02X:%02X:%02X:%02X:%02X:%02X", macb[0],macb[1],macb[2],macb[3],macb[4],macb[5]); return String(buf); }

void wifi_v3::localAPIsetting(){
	server.on("/nosleep", HTTP_GET, [this](){ s_data.c6=0; s_data.c_update=1; server.send(200, "text/plain", "No sleep Mode"); });
	server.on("/sleep",  HTTP_GET, [this](){ s_data.c6=1; s_data.c_update=1; server.send(200, "text/plain", "Sleep Mode"); });
	server.on("/debug",  HTTP_GET, [this](){ server.send(200, "text/html", String(debugs)); });
	server.on("/reset",  HTTP_GET, [this](){ server.send(200, "text/plain", "Reset"); delay(1000); ESP.restart(); });
	server.on("/data",   HTTP_GET, [this](){
		DynamicJsonDocument jsonDoc(500);
		jsonDoc["module"] = s_data.module; jsonDoc["module_number"] = MODULE_NUMBER; jsonDoc["module_name"] = MODULE_NAME; jsonDoc["ip"] = ip;
		auto sensorNames = jsonDoc.createNestedObject("sensor_names"); sensorNames["s1_name"]=S1_NAME; sensorNames["s2_name"]=S2_NAME; sensorNames["s3_name"]=S3_NAME; sensorNames["s4_name"]=S4_NAME; sensorNames["s5_name"]=S5_NAME;
		auto controlNames = jsonDoc.createNestedObject("control_names"); controlNames["c1_name"]=C1_NAME; controlNames["c2_name"]=C2_NAME; controlNames["c3_name"]=C3_NAME; controlNames["c4_name"]=C4_NAME; controlNames["c5_name"]=C5_NAME; controlNames["c6_name"]=C6_NAME;
		auto sensorData = jsonDoc.createNestedObject("sensor_data"); sensorData["s1"]=s_data.s1; sensorData["s2"]=s_data.s2; sensorData["s3"]=s_data.s3; sensorData["s4"]=s_data.s4; sensorData["s5"]=s_data.s5;
		auto controlData = jsonDoc.createNestedObject("control_data"); controlData["c1"]=s_data.c1; controlData["c2"]=s_data.c2; controlData["c3"]=s_data.c3; controlData["c4"]=s_data.c4; controlData["c5"]=s_data.c5; controlData["c6"]=s_data.c6; controlData["c_update"]=s_data.c_update;
		String jsonString; serializeJson(jsonDoc, jsonString); server.send(200, "application/json", jsonString);
	});
}
#endif

#ifdef MQTT
MqttHandler::MqttHandler(const char* primaryServer, const char* secondaryServer, byte moduleId, const char* mqttUser, const char* mqttPassword, int port)
	: primary_server(primaryServer), secondary_server(secondaryServer), module_id(moduleId), mqtt_user(mqttUser), mqtt_password(mqttPassword), mqtt_port(port), mqtt_restart(0), numSubscribed(0), lastWorkingServerIndex(0) {}

void MqttHandler::setup() {
#if defined(ESP8266)
	espClient.setInsecure();
#endif
	mqtt_client.setServer(primary_server, mqtt_port);
	mqtt_client.setCallback(Mqtt_Callback);
}

bool MqttHandler::sub(const char* topic) {
	for (int i=0;i<numSubscribed;i++){ if(subscribedTopics[i]==topic) return true; }
	if (mqtt_client.subscribe(topic)) { subscribedTopics[numSubscribed++] = String(topic); return true; }
	debugs = String("\nFailed to subscribe: ") + topic + debugs; return false;
}

void MqttHandler::pub(const char* topic, float payload){ char mqttpayload[16]; dtostrf(payload, 6, 3, mqttpayload); mqtt_client.publish(topic, mqttpayload); }
void MqttHandler::pubtext(const char* topic, const char* payload){ mqtt_client.publish(topic, payload); }

bool MqttHandler::reconnect(){ if(mqtt_client.connected()) return true; mqtt_restart=0; const char* servers[]={primary_server, secondary_server}; for(int j=0;j<2;j++){ int i=(lastWorkingServerIndex+j)%2; mqtt_client.setServer(servers[i], mqtt_port); while(!mqtt_client.connected() && mqtt_restart<3){ mqtt_restart++; String clientId = String("S")+module_id; if(mqtt_client.connect(clientId.c_str(), mqtt_user, mqtt_password)){ mqtt_client.loop(); lastWorkingServerIndex=i; return true; } delay(1000);} } debugs = String("\nFailed to connect MQTT")+debugs; return false; }

PubSubClient& MqttHandler::client(){ return mqtt_client; }
#endif
