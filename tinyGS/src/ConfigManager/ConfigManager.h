/*
  ConfigManager.h - Config Manager class
  
  Copyright (C) 2020 -2021 @G4lile0, @gmag12 and @dev_4m1g0

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#ifndef ConfigManager_h
#define ConfigManager_h

#include "IotWebConf2.h"
#include "logos.h"
#include <Wire.h>
#include "htmlOptions.h"

#ifdef ESP8266
  #include "ESP8266HTTPUpdateServer.h"
#elif defined(ESP32)
  #include "IotWebConf2ESP32HTTPUpdateServer.h"
#endif

constexpr auto STATION_NAME_LENGTH = 21;
constexpr auto COORDINATE_LENGTH = 10;
constexpr auto MQTT_SERVER_LENGTH = 31;
constexpr auto MQTT_PORT_LENGTH = 6;
constexpr auto MQTT_USER_LENGTH = 31;
constexpr auto MQTT_PASS_LENGTH = 31;
constexpr auto CHECKBOX_LENGTH = 9;
constexpr auto NUMBER_LEN = 32;
constexpr auto CB_SELECTED_STR = "selected";


constexpr auto ROOT_URL = "/";
constexpr auto CONFIG_URL = "/config";
constexpr auto DASHBOARD_URL = "/dashboard";
constexpr auto UPDATE_URL = "/firmware";
constexpr auto RESTART_URL = "/restart";

const char TITLE_TEXT[] PROGMEM = "TinyGS Configuration";


constexpr auto thingName = "My TinyGS";
constexpr auto initialApPassword = "";
constexpr auto configVersion = "0.04"; //max 4 chars

#define MQTT_DEFAULT_SERVER "mqtt.tinygs.com"
#define MQTT_DEFAULT_PORT  "8883"

constexpr auto AP_TIMEOUT_MS = "300000";

enum boardNum {
  HELTEC_V1 = 0,
  HELTEC_V2,
  TTGO_V1,
  TTGO_V2,
  TBEAM_OLED,
  ESP32_SX126X_XTAL,
  TTGO_V2_SX126X_XTAL,
  ESP32_SX126X_TXC0_1,
  ESP32_SX126X_TXC0_2,
  TBEAM_OLED_v1_0,

  NUM_BOARDS //this line always has to be the last one
};

typedef struct {
   uint8_t  OLED__address;
   uint8_t  OLED__SDA;
   uint8_t  OLED__SCL;
   uint8_t  OLED__RST;
   uint8_t  PROG__BUTTON;
   uint8_t  BOARD_LED;
   bool     L_SX127X;
   uint8_t  L_NSS;        // CS
   uint8_t  L_DI00;        
   uint8_t  L_DI01;
   uint8_t  L_BUSSY;        
   uint8_t  L_RST;
   uint8_t  L_MISO;
   uint8_t  L_MOSI;        
   uint8_t  L_SCK;
   float    L_TCXO_V;
   String   BOARD;
} board_type;

class ConfigManager : public IotWebConf2
{
public:
  static ConfigManager& getInstance()
  {
    static ConfigManager instance;
    return instance;
  }
  void resetAPConfig();
  void resetAllConfig();
  boolean init();
  void printConfig();

  uint16_t getMqttPort() { return (uint16_t) atoi(mqttPort); }
  const char* getMqttServer() { return mqttServer; }
  const char* getMqttUser() { return mqttUser; }
  const char* getMqttPass() { return mqttPass; }
  float getLatitude() { return atof(latitude); }
  float getLongitude() { return atof(longitude); }
  const char* getTZ() { return tz; }
  uint8_t getBoard() { return atoi(board); }
  uint8_t getOledBright() { return atoi(oledBright); }
  bool getAllowTx() { return !strcmp(allowTx, CB_SELECTED_STR); }
  bool getRemoteTune() { return !strcmp(remoteTune, CB_SELECTED_STR); }
  bool getTelemetry3rd() { return !strcmp(telemetry3rd, CB_SELECTED_STR); }
  bool getTestMode() { return !strcmp(testMode, CB_SELECTED_STR); }
  bool getAutoUpdate() { return !strcmp(autoUpdate, CB_SELECTED_STR); }
  void setAllowTx(bool status) { if (status) strcpy(allowTx, CB_SELECTED_STR); else allowTx[0] = '\0'; this->saveConfig(); }
  void setRemoteTune(bool status) { if (status) strcpy(remoteTune, CB_SELECTED_STR); else remoteTune[0] = '\0'; this->saveConfig(); }
  void setTelemetry3rd(bool status) { if (status) strcpy(telemetry3rd, CB_SELECTED_STR); else telemetry3rd[0] = '\0'; this->saveConfig(); }
  void setTest(bool status) { if (status) strcpy(testMode, CB_SELECTED_STR); else testMode[0] = '\0'; this->saveConfig(); }
  void setAutoUpdate(bool status) { if (status) strcpy(autoUpdate, CB_SELECTED_STR); else autoUpdate[0] = '\0'; this->saveConfig(); }

  const char* getWiFiSSID() { return getWifiSsidParameter()->valueBuffer; }
  bool isApMode() { return (getState() != IOTWEBCONF_STATE_CONNECTING && getState() != IOTWEBCONF_STATE_ONLINE); }
  bool isConnected() { return getState() == IOTWEBCONF_STATE_ONLINE; };
  board_type getBoardConfig(){ return boards[getBoard()]; }


private:
  
  class GSConfigHtmlFormatProvider : public iotwebconf2::HtmlFormatProvider
  {
  public:
    GSConfigHtmlFormatProvider(ConfigManager& x) : configManager(x) { }
  protected:
    String getScriptInner() override
    {
      return
        iotwebconf2::HtmlFormatProvider::getScriptInner();
        //String(FPSTR(CUSTOMHTML_SCRIPT_INNER));
    }
    String getBodyInner() override
    {
      return
        String(FPSTR(LOGO)) +
        iotwebconf2::HtmlFormatProvider::getBodyInner();
    }

    ConfigManager& configManager;
  };

  ConfigManager();
  void handleRoot();
  void handleDashboard();
  void handleRestart();
  bool formValidator(iotwebconf2::WebRequestWrapper* webRequestWrapper);
  void boardDetection();
  
  std::function<boolean(iotwebconf2::WebRequestWrapper*)> formValidatorStd;
  DNSServer dnsServer;
  WebServer server;
#ifdef ESP8266
  ESP8266HTTPUpdateServer httpUpdater;
#elif defined(ESP32)
  HTTPUpdateServer httpUpdater;
#endif
  GSConfigHtmlFormatProvider gsConfigHtmlFormatProvider;
  board_type boards[NUM_BOARDS]; 

  char latitude[COORDINATE_LENGTH] = "";
  char longitude[COORDINATE_LENGTH] = "";
  char tz[TZ_LENGTH] = "";
  char mqttServer[MQTT_SERVER_LENGTH] = MQTT_DEFAULT_SERVER;
  char mqttPort[MQTT_PORT_LENGTH] = MQTT_DEFAULT_PORT;
  char mqttUser[MQTT_USER_LENGTH] = "";
  char mqttPass[MQTT_PASS_LENGTH] = "";
  char board[BOARD_LENGTH] = "";
  char oledBright[NUMBER_LEN] = "";
  char allowTx[CHECKBOX_LENGTH] = "";
  char remoteTune[CHECKBOX_LENGTH] = "";
  char telemetry3rd[CHECKBOX_LENGTH] = "";
  char testMode[CHECKBOX_LENGTH] = "";
  char autoUpdate[CHECKBOX_LENGTH] = "";



  iotwebconf2::NumberParameter latitudeParam = iotwebconf2::NumberParameter("Latitude (will be public)", "lat", latitude, COORDINATE_LENGTH, NULL, NULL, "required min='-180' max='180' step='0.001'");
  iotwebconf2::NumberParameter longitudeParam = iotwebconf2::NumberParameter("Longitude (will be public)", "lng", longitude, COORDINATE_LENGTH, NULL, NULL, "required min='-180' max='180' step='0.001'");
  iotwebconf2::SelectParameter tzParam = iotwebconf2::SelectParameter("Time Zone", "tz", tz, TZ_LENGTH, (char*)TZ_VALUES, (char*)TZ_NAMES, sizeof(TZ_VALUES) / TZ_LENGTH, TZ_NAME_LENGTH);

  iotwebconf2::ParameterGroup groupMqtt = iotwebconf2::ParameterGroup("MQTT credentials" , "MQTT credentials (get them <a href='https://t.me/joinchat/DmYSElZahiJGwHX6jCzB3Q'>here</a>)");
  iotwebconf2::TextParameter mqttServerParam = iotwebconf2::TextParameter("Server address", "mqtt_server", mqttServer, MQTT_SERVER_LENGTH, MQTT_DEFAULT_SERVER, MQTT_DEFAULT_SERVER, "required type=\"text\" maxlength=30");
  iotwebconf2::NumberParameter mqttPortParam = iotwebconf2::NumberParameter("Server Port", "mqtt_port", mqttPort, MQTT_PORT_LENGTH, MQTT_DEFAULT_PORT, MQTT_DEFAULT_PORT, "required min=\"0\" max=\"65536\" step=\"1\"");
  iotwebconf2::TextParameter mqttUserParam = iotwebconf2::TextParameter("MQTT Username", "mqtt_user", mqttUser, MQTT_USER_LENGTH, NULL, NULL, "required type=\"text\" maxlength=30");
  iotwebconf2::TextParameter mqttPassParam = iotwebconf2::TextParameter("MQTT Password", "mqtt_pass", mqttPass, MQTT_PASS_LENGTH, NULL, NULL, "required type=\"text\" maxlength=30");

  iotwebconf2::ParameterGroup groupBoardConfig = iotwebconf2::ParameterGroup("Board config" , "Board config");
  iotwebconf2::SelectParameter boardParam = iotwebconf2::SelectParameter("Board type", "board", board, BOARD_LENGTH, (char*)BOARD_VALUES, (char*)BOARD_NAMES, sizeof(BOARD_VALUES) / BOARD_LENGTH, BOARD_NAME_LENGTH);
  iotwebconf2::NumberParameter oledBrightParam = iotwebconf2::NumberParameter("OLED Bright", "oled_bright", oledBright, NUMBER_LEN, "100", "0..100", "min='0' max='100' step='1'");
  iotwebconf2::CheckboxParameter AllowTxParam  = iotwebconf2::CheckboxParameter("Enable TX (HAM licence/ no preamp)", "tx", allowTx, CHECKBOX_LENGTH, true);
  iotwebconf2::CheckboxParameter remoteTuneParam = iotwebconf2::CheckboxParameter("Allow Remote Tunning","remote_tune",remoteTune, CHECKBOX_LENGTH, true);
  iotwebconf2::CheckboxParameter telemetry3rdParam = iotwebconf2::CheckboxParameter("Allow third party telemetry (sat owners,  satnog... )","telemetry3rd",telemetry3rd, CHECKBOX_LENGTH, true);
  iotwebconf2::CheckboxParameter testParam = iotwebconf2::CheckboxParameter("Test mode","test",testMode, CHECKBOX_LENGTH, false);
  iotwebconf2::CheckboxParameter autoUpdateParam = iotwebconf2::CheckboxParameter("Automatic Firmware Update","auto_update",autoUpdate, CHECKBOX_LENGTH, true);
};

#endif