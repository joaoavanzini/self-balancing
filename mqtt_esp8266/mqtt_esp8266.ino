#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define MAX_DELAY 1000

const char* ssid = "nobyte 2.5";
const char* pass = "fjmwifi2019B";

const char* mqtt_server = "192.168.15.131";
const char* mqtt_topic = "selfbalancing";
const char* mqtt_client = "mk1esp_client";

WiFiClient espClient;
PubSubClient client(espClient);

String myStr;
char dataJson[256];

int aux = 0; 

void setup(){

    Serial.begin(115200);
    WiFi.begin(ssid, pass);
  
    Serial.println();
    Serial.println("Connecting to Wi-Fi");
    while(WiFi.status() != WL_CONNECTED && aux <= 10){
            delay(MAX_DELAY);
            Serial.print(".");
            aux++;
    }

    Serial.println();
    Serial.print("Connected to IP: ");
    Serial.println(WiFi.localIP());

    Serial.println("Connecting to MQTT server...");
    client.setServer(mqtt_server, 1883);
}

void loop(){

    if (client.connect(mqtt_client)){
        Serial.println("Connected to MQTT server!");
        while(client.state() == MQTT_CONNECTED){
            memset(dataJson, 0, 256);
            myStr = Serial.readStringUntil('\n');
            myStr.toCharArray(dataJson, myStr.length()+1);
            if(myStr != ""){
                client.publish(mqtt_topic, dataJson);
                Serial.println("Published data-JSON!");
            }            
        }
    }else{
        Serial.println("Disconnected to MQTT server!");
        delay(MAX_DELAY);
    }
}
