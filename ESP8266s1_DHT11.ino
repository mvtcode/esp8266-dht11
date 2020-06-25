#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <SimpleDHT.h>

const char* ssid = "MVT_ESP8266_DHT11";
const char* password = "12345678jqk";

IPAddress local_IP(192,168,5,1);
IPAddress gateway(192,168,5,1);
IPAddress subnet(255,255,255,0);

ESP8266WebServer server(80);

int pin = 2;
SimpleDHT11 dht11(pin);

byte temp, humi;

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void) {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_IP, gateway, subnet);

  if (MDNS.begin("esp8266")) {
    //
  }

  server.on("/", []() {
    String s = "<!DOCTYPE html> <html lang='vi'> <head> <meta charset='UTF-8'> <meta name='viewport' content='width=device-width, initial-scale=1.0'> <meta http-equiv='X-UA-Compatible' content='ie=edge'> <title>Cảm biến nhiệt độ & độ ẩm qua wifi</title> <style> * {margin: 0; padding: 0; } body {font-family: Arial, Helvetica, sans-serif; max-width: 800px; margin: 0 auto; background-color: #eee; text-align: center; } h1 {font-size: 24px; } .contentner {font-size: 30px; font-weight: bold; float: left; width: 100%; margin-top: 30px; } .contentner div {float: left; width: 50%; } #temp, #humi {font-size: 60px; } </style> <script> onloadPage = () => {const temp = document.getElementById('temp'); const humi = document.getElementById('humi'); const GET = (url, callback) => {const req = new XMLHttpRequest(); req.open('GET', url); req.addEventListener('load', event => {const res = event.currentTarget; callback(res.response); }); req.addEventListener('error', event => {alert(event); }); req.send(); }; const updateData = () => {GET('/status', res => {const arr = res.split('-'); temp.innerText = arr[0]; humi.innerText = arr[1]; }); }; setTimeout(() => {updateData(); }, 1000); setInterval( () => {updateData(); }, 3000); }; </script> </head> <body onload='onloadPage()'> <h1>Cảm biến nhiệt độ - độ ẩm qua wifi</h1> <h3>Vui lòng liên hệ Mạc Tân - 0964 335 688</h3> <div class='contentner'> <div> <p>Nhiệt độ</p> <p id='temp'>0</p> </div> <div> <p>Độ ẩm</p> <p id='humi'>0</p> </div> </div> </body> </html>";
    server.send(200, "text/html", s);
  });

  server.on("/status", []() {
    int err = SimpleDHTErrSuccess;
    if ((err = dht11.read(&temp, &humi, NULL)) != SimpleDHTErrSuccess) {
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    }
  
    server.send(200, "text/plain", (String)temp + "-" + (String)humi);
  });

  server.onNotFound(handleNotFound);

  server.begin();
}

void loop(void) {
  server.handleClient();
  MDNS.update();
}
