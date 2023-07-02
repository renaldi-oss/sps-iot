#include <Arduino.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>
// konek ke wifi
// const char* ssid = "JTI-POLINEMA";
// const char* password = "jtifast!";
const char* ssid = "SPS";
const char* password = "Skip33rr";

// server mqtt broker
const char* mqtt_server = "q7e59bdb.ala.us-east-1.emqxsl.com";
const int mqtt_port = 8883;
const char* mqtt_user = "esp-2";
const char* mqtt_pass = "esp-2";

int led[6] = {D3, D4, D5, D6, D7, D8};

WiFiClientSecure espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];
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



bool showLCD = false;
// callback
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
  Serial.println("Payload: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Create a StaticJsonDocument with a capacity large enough to hold the message
  StaticJsonDocument<400> doc;
  
  // Deserialize the JSON payload
  DeserializationError error = deserializeJson(doc, payload, length);
  
  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }

  // Extract values
  JsonArray data = doc["data"];
  bool isAllOccupied = false;
  for (JsonObject elem : data) {
    String kode = elem["kode"];
    bool isOccupied = elem["isOccupied"];
    Serial.print("kode: ");
    Serial.print(kode);
    Serial.print(", isOccupied: ");
    Serial.println(isOccupied);
    if(kode == "A1"){
      if(isOccupied){
        digitalWrite(led[0], HIGH);
      }else{
        digitalWrite(led[0], LOW);
      }
    }else if(kode == "A2"){
      if(isOccupied){
        digitalWrite(led[1], HIGH);
      }else{
        digitalWrite(led[1], LOW);
      }
    }else if(kode == "A3"){
      if(isOccupied){
        digitalWrite(led[2], HIGH);
      }else{
        digitalWrite(led[2], LOW);
      }
    }else if(kode == "B1"){
      if(isOccupied){
        digitalWrite(led[3], HIGH);
      }else{
        digitalWrite(led[3], LOW);
      }
    }else if(kode == "B2"){
      if(isOccupied){
        digitalWrite(led[4], HIGH);
      }else{
        digitalWrite(led[4], LOW);
      }
    }else if(kode == "B3"){
      if(isOccupied){
        digitalWrite(led[5], HIGH);
      }else{
        digitalWrite(led[5], LOW);
      }
    }
  }
  for (JsonObject elem : data) {
    bool isOccupied = elem["isOccupied"];
    if(!isOccupied){
      isAllOccupied = false;
      break;
    }else{
      isAllOccupied = true;
    }
  }
  if(isAllOccupied){
    showLCD = true;
  }else{
    showLCD = false;
  }
}

// method untuk publish data
void publishData(String topic, String data){
  char charBuf[50];
  topic.toCharArray(charBuf, 50);
  client.publish(charBuf, data.c_str());
}

LiquidCrystal_I2C lcd(0x27,20,4);
void setup() {
  Serial.begin(115200);
  setup_wifi();
  espClient.setInsecure();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  for(int i = 0; i < 6; i++){
    pinMode(led[i], OUTPUT);
  }
  lcd.init(); // initialize the lcd
  lcd.backlight();
  lcd.clear();
  lcd.home();

}

void loop() {
  if(!client.connected()){
    reconnect();
  }else{
    client.loop();
    client.subscribe("/parkir");
    if(showLCD){
      lcd.clear();
      lcd.home();
      lcd.print("PARKIR FULL");
      lcd.setCursor(0, 1);
      lcd.print("MOHON TUNGGU");

    }else{
      lcd.clear();
      lcd.home();
      lcd.print("PARKIR KOSONG");
      lcd.setCursor(0, 1);
      lcd.print("SILAHKAN MASUK");
    }
  }
}

// void scrollText(int row, String message, int delayTime, int lcdColumns){
//    for (int i = 0; i < lcdColumns; i++)
//    {
//      message = " " + message;
//    }
//    message = message + " ";
//    for (int pos = 0; pos < message.length(); pos++)
//    {
//      lcd.setCursor(0, row);
//      lcd.print(message.substring(pos, pos + lcdColumns));
//      delay(delayTime);
//    }
//  }