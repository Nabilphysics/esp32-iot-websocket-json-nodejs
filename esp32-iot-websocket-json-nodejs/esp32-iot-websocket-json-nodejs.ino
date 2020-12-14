
/*
 * Source:
   Main Source(nodejs code + esp32 code): https://esp8266-shop.com/blog/websocket-connection-between-esp8266-and-node-js-server/
   EPS8266 Websocket Library: https://github.com/morrissinger/ESP8266-Websocket
   ESP8266 Websocket to ESP32 Websocket Library Conversion: https://techtutorialsx.com/2017/11/01/esp32-arduino-websocket-client/
   Rewritten with JSON Intigration: Syed Razwanul Haque(Nabil) , https://www.github.com/Nabilphysics
.
*/
/*******************Esp8266_Websocket.ino****************************************/
#include <WiFi.h>
#include <WebSocketClient.h>
#include <ArduinoJson.h>

boolean handshakeFailed = 0;
String data = "";
char path[] = "/";   //identifier of this device
const char* ssid     = "HACKED";
const char* password = "python123456";
char* host = "192.168.0.104";  //replace this ip address with the ip address of your Node.Js server
const int espport = 3000;
int readPin;

WebSocketClient webSocketClient;

// Use WiFiClient class to create TCP connections
WiFiClient client;



void setup() {
  Serial.begin(115200);
  delay(10);

  
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  delay(1000);

  wsconnect();
  
}
void loop() {
  
  //JSON - Memory pool for JSON object
  const size_t bufferSize = JSON_OBJECT_SIZE(3);// for example: Sensor1, Sensor2, Sensor3 = 3 Object
  DynamicJsonBuffer jsonBuffer(bufferSize);
  // Create the root of the object tree.
  //
  // It's a reference to the JsonObject, the actual bytes are inside the
  // JsonBuffer with all the other nodes of the object tree.
  // Memory is freed when jsonBuffer goes out of scope.
  JsonObject& root = jsonBuffer.createObject();
  root["sensor"] = "sensorData";
  root["analogValue"] = 1024;
  //Nested Array
  JsonArray& d1 = root.createNestedArray("d1");
  JsonArray& d2 = root.createNestedArray("d2");

  // Add a nested array.
  //
  // It's also possible to create the array separately and add it to the
  // JsonObject but it's less efficient.

  //Adding demo data to nested array
  for (int k = 0; k < 750; k++) {
    d1.add(4000 + k);
  }
  for (int k = 0; k < 750; k++) {
    d2.add(k);
  }


  String dataBuffer; //It is a buffer to hold json value
  root.printTo(dataBuffer); // print json data to buffer
  
  if (client.connected()) {
     
     webSocketClient.sendData(dataBuffer); //Send buffer to server
     Serial.println("Data Sent to Server");
  }
    
//    webSocketClient.getData(data);
//    if (data.length() > 0) {
//      Serial.println(data);
//      //*************send log data to server in certain interval************************************
//      //currentMillis=millis();
//      if (abs(currentMillis - previousMillis) >= interval) {
//        previousMillis = currentMillis;
//        data = (String) random(2, 100); //read adc values, this will give random value, since no sensor is connected.
//        //For this project we are pretending that these random values are sensor values
//        webSocketClient.sendData(data);//send sensor data to websocket server
//      }
//    }
//    else {
//    }
//    delay(5);
 // }
}
//*********************************************************************************************************************
//***************function definitions**********************************************************************************
void wsconnect() {
  // Connect to the websocket server
  if (client.connect(host, espport)) {
    Serial.println("Connected");
  } else {
    Serial.println("Connection failed.");
    delay(1000);

    if (handshakeFailed) {
      handshakeFailed = 0;
      ESP.restart();
    }
    handshakeFailed = 1;
  }
  // Handshake with the server
  webSocketClient.path = path;
  webSocketClient.host = host;

  if (webSocketClient.handshake(client)) {
    Serial.println("Handshake successful");
  } else {

    Serial.println("Handshake failed.");
    delay(4000);

    if (handshakeFailed) {
      handshakeFailed = 0;
      ESP.restart();
    }
    handshakeFailed = 1;
  }
}
