#include "WiFi.h"
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
 
const char* ssid = "Paw";
const char* password =  "0022446688";

#define BOTtoken "2047806331:AAFcQnxWUupxWX8FMQt28aWJs-86Ka7lIzE"
#define CHAT_ID "823900182"

bool isTouch = false;

WiFiClientSecure client;

UniversalTelegramBot bot(BOTtoken, client);

void setup() {
 
  Serial.begin(115200);
  pinMode(12, INPUT_PULLUP);
 
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
 
  Serial.println("Connected to the WiFi network");
  Serial.println(WiFi.localIP());
  
  bot.sendMessage(CHAT_ID, "Bot started up", "");
}
 
void loop() {
      if((digitalRead(12) == HIGH) && isTouch){
        client.println("No Touch\n");
        bot.sendMessage(CHAT_ID, "No Touch", "");
        isTouch = false;
      }
      else if((digitalRead(12) == LOW) && !isTouch){
        client.println("Touch\n");
        bot.sendMessage(CHAT_ID, "Touch", "");
        isTouch = true;
      }
        
      delay(10);
}
