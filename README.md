\
# roseniot

Basbibliotek för mina ESP8266/ESP32-projekt (WiFi multi-SSID, lokal web-API, MQTT, REST, NTP, Dallas/OneWire m.m.).

## Installation

**Alternativ A – ZIP**
1. Ladda ner senaste release-ZIP.
2. Arduino IDE: *Sketch → Include Library → Add .ZIP Library…*.

**Alternativ B – Git (globalt)**
```bash
cd ~/Documents/Arduino/libraries
git clone https://github.com/<user>/roseniot.git
```

## Hemligheter (viktigt)
1. Öppna `src/RosenIoT_secrets_template.h` och kopiera innehållet till en **ny fil** `RosenIoT_secrets.h`.
2. Fyll i dina SSID/lösen.
3. **Publicera aldrig** `RosenIoT_secrets.h`.

## Snabbstart (ESP32/ESP8266)
```cpp
#define WIFI_V3
#include <RosenIoT.h>
SQLData sdata; String debugs=""; String ip=""; HttpServer server(80);
wifi_v3 net(sdata);
void setup(){ Serial.begin(115200); net.setup(); }
void loop(){ server.handleClient(); }
```

## Konfiguration
Justera `src/RosenIoT_config.h` (eller definiera makron före `#include <RosenIoT.h>` i din sketch):
- `MODULE_NUMBER`, `MODULE_NAME`
- `DBURL`, `APIURL`
- `S1_NAME..S5_NAME`, `C1_NAME..C6_NAME`

## Beroenden
- **ArduinoJson**
- **ElegantOTA**
- **PubSubClient**
- **NTPClient**
- **OneWire**, **DallasTemperature**

I PlatformIO:
```ini
lib_deps =
  bblanchon/ArduinoJson
  ayushsharma82/ElegantOTA
  knolleary/PubSubClient
  arduino-libraries/NTPClient
  paulstoffregen/OneWire
  milesburton/DallasTemperature
```

## Exempel
- `WiFi_V3_Basic` – kopplar upp mot starkaste kända SSID och exponerar `/data`.
- `WebServer_Info` – enkla HTML-sidor med sensor-/styrdata.
- `MQTT_Basic` – kopplar upp och publicerar ett testvärde.

## Licens
MIT
