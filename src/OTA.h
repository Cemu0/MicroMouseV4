#define CONFIG_ESP32_WIFI_TASK_PINNED_TO_CORE_1 1
#include <esp_wifi.h>
#include <Arduino.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#ifdef USE_ASYNC_ELEGANT_OTA
    #include <AsyncElegantOTA.h>
#else
    // #include <WiFiUdp.h>
    #include <ArduinoOTA.h>
#endif

#include <TelnetStream.h>


AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
int Xvalue = 0, Yvalue = 0;

//note you should turn on and off wifiHotspot 
void connectWifi(const char* ssid, const char* password){
    WiFi.disconnect();
    esp_wifi_disconnect();
    esp_wifi_stop();
    esp_wifi_deinit();
    // WiFi.setSleep(false);
    // esp_wifi_set_ps(WIFI_PS_NONE);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    unsigned long timer = millis();
    while ((WiFi.status() != WL_CONNECTED) && (millis() - timer <= 10000)){
      Serial.println("ConnectingWIFI..."); 
      delay(1000);
    };
    if(WiFi.status() != WL_CONNECTED){
    // while (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("Connection Failed! Rebooting...");
        delay(100);
        ESP.restart();
    // }
    }
}

void handle_message(String msg) {
  int commaIndex = msg.indexOf(',');
  Xvalue = msg.substring(0, commaIndex).toInt();
  Yvalue = msg.substring(commaIndex + 1).toInt();
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    handle_message(String((char*)data));
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
 void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      TelnetStream.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      TelnetStream.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void setupServer(){
    MDNS.begin("myesp32");
    MDNS.addService("ws", "tcp", 80);
    MDNS.addService("telnet", "tcp", 23);
    
    #ifdef USE_ASYNC_ELEGANT_OTA
        AsyncElegantOTA.begin(&server);    // Start AsyncElegantOTA
    #else
        ArduinoOTA.setHostname("myesp32");
        // ArduinoOTA.setHostname("myesp32");
        // Port defaults to 3232
        // ArduinoOTA.setPort(3232);

        // Hostname defaults to esp3232-[MAC]
        // ArduinoOTA.setHostname("myesp32");

        // No authentication by default
        // ArduinoOTA.setPassword("admin");

        // Password can be set with it's md5 value as well
        // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
        // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

        // ArduinoOTA
        //   .onStart([]() {
        //     String type;
        //     if (ArduinoOTA.getCommand() == U_FLASH)
        //       type = "sketch";
        //     else // U_SPIFFS
        //       type = "filesystem";

        //     // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
        //     Serial.println("Start updating " + type);
        //   })
        //   .onEnd([]() {
        //     Serial.println("\nEnd");
        //   })
        //   .onProgress([](unsigned int progress, unsigned int total) {
        //     Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
        //   })
        //   .onError([](ota_error_t error) {
        //     Serial.printf("Error[%u]: ", error);
        //     if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
        //     else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
        //     else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
        //     else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
        //     else if (error == OTA_END_ERROR) Serial.println("End Failed");
        //   });
        ArduinoOTA.begin();
    #endif

    // server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    //     AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", index_html_gz, sizeof(index_html_gz));
    //     response->addHeader("Content-Encoding", "gzip");
    //     request->send(response);
    // });
    
    ws.onEvent(onEvent);
    server.addHandler(&ws);
    server.begin();
    TelnetStream.begin();
}