#include <Arduino.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
// konek ke wifi
// const char* ssid = "WrgSederhana";
// const char* password = "sederhana21a#";
const char* ssid = "SPS";
const char* password = "Skip33rr";
// const char* ssid = "WrgSebelah";
// const char* password = "sederhana21a#";
// const char* ssid = "JTI-POLINEMA";
// const char* password = "jtifast!";



// server mqtt broker
const char* mqtt_server = "q7e59bdb.ala.us-east-1.emqxsl.com";
const int mqtt_port = 8883;
const char* mqtt_user = "esp-1";
const char* mqtt_pass = "esp-1";

WiFiClientSecure espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];

class KY032 {
  public:
    String kode;
    int pin;
    KY032(String k, int p) {
      kode = k;
      pin = p;
      pinMode(pin, INPUT);
    }
    bool checkObstacle() {
      return (digitalRead(pin) == LOW) ? true : false;
    }
    String getKode(){
      return kode;
    }
};

KY032 sensors[] = {
  KY032("A1", D3),
  KY032("A2",D4),
  KY032("A3",D5),
  KY032("B1",D6),  
  KY032("B2",D7),  
  KY032("B3",D8)  
};

// setup wifi
void setup_wifi() {
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
}

// reconnect mqtt
void reconnect(){
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
// callback mqtt
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  
  Serial.println();
}
// method untuk publish data
void publishData(String topic, String data){
  char charBuf[50];
  topic.toCharArray(charBuf, 50);
  client.publish(charBuf, data.c_str());
}
void setup() {
  Serial.begin(115200);
  setup_wifi();
  espClient.setInsecure();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}
void loop() {
  if(!client.connected()){
    reconnect();
  }else{
    client.loop();
    DynamicJsonDocument doc(1024);
    JsonArray data = doc.createNestedArray("data");
    for(int i = 0; i < 6; i++){
        JsonObject nested = data.createNestedObject();
        nested["kode"] = sensors[i].getKode();
        nested["isOccupied"] = sensors[i].checkObstacle();
    }
    char mqttMsg[1024];
    serializeJson(doc, mqttMsg);
    publishData("/parkir", mqttMsg);
    delay(3000);
  }
}