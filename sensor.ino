/*
 * Water sensor application for Amica NODEMCU Lolin V3 (ESP6266 Microcontroller with 802.11 b/g/n WLAN)
 * 
 * 1. Connects to Wifi
 * 2. Creates a Web Server instance to show the level of water graphically
 * 
 * https://www.az-delivery.de/products/copy-of-nodemcu-lua-amica-v2-modul-mit-esp8266-12e
 * 
 * Cem YILDIZ <cem.yildiz@ya.ru>
 * 
 * License: GPL 3 https://www.gnu.org/licenses/gpl-3.0.txt
 */
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

const int ANALOG_PIN_FOR_WATER_SENSOR = 0;
const int DIGITAL_PIN_FOR_ENABLING_WATER_SENSOR = 5; /* D3 on board */
const char *SSID_FOR_WIFI = "<your ssid>"; 
const char *PASSWORD_FOR_WIFI = "<your wifi password>";
const int HTTP_PORT_FOR_WEB_SERVER = 80;
const int REFRESH_INTERVAL_FOR_WEB = 1000; /* Miliseconds */
const int BOUD_RATE_FOR_SERIAL_DEBUG = 115200;

ESP8266WebServer server(HTTP_PORT_FOR_WEB_SERVER);

int value = 0;
int height = 0;

const char MAIN_page[] PROGMEM = R"=====(

<!DOCTYPE html>
<html>
<head>
  <title>Water Tank</title>
  <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.6.0/jquery.min.js"></script>
  <style>
    body {font-family: helvetica,arial,sans-serif;}
    .error { color: #ff0000; visibility: hidden; }
    .outer-box { width: 80px; height: 400px; position: relative; background-color: #dddddd }
    .inner-box { width: 80px; bottom: 0; left: 0; position: absolute; background-color: #d4f1f9 }
  </style>
  <script>
    setInterval(function() {
      $.ajax({
        url: '/data/',
        success: function(result) {
          $("#value").text(result['level']);
          level = result['level'] - 40;
          height = level * 40;
          $(".inner-box").css("height", height + "px");      
          $(".error").css("visibility", "hidden");
        },
        error: function(error) {
          $(".error").css("visibility", "visible");
        }
      });
    }, 1000);
  </script>
</head>

<body>
  <div class="error">Connection lost</div>
  <div class="outer-box">
    <div class="inner-box"></div>
  </div>
  <div class="received">
    <span>Value: </span>
    <span id="value"></span>
  </div>
</body>
</html>

)=====";

void handleRoot() {
  server.send(200, "text/html", MAIN_page);
}

void handleAjax() {
  digitalWrite(DIGITAL_PIN_FOR_ENABLING_WATER_SENSOR, HIGH);
  value = analogRead(ANALOG_PIN_FOR_WATER_SENSOR);
  digitalWrite(DIGITAL_PIN_FOR_ENABLING_WATER_SENSOR, LOW);
  server.send(200, "application/json", "{\"level\": " + String(value) + "}");
}

void connectWifi() {
  Serial.print("Configuring access point...");
  WiFi.begin(SSID_FOR_WIFI, PASSWORD_FOR_WIFI);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());  
}

void bindRoutes() {
  server.on("/", handleRoot );
  server.on("/data/", handleAjax);
}

void startWebServer() {
  server.begin();
  Serial.println ("HTTP server started");
}

void initialize() {
  delay(3000);
  Serial.begin(BOUD_RATE_FOR_SERIAL_DEBUG);
  Serial.println();
}

void setup() {
  initialize();
  connectWifi();
  bindRoutes();
  startWebServer();
}

void loop() {
  server.handleClient();
}
