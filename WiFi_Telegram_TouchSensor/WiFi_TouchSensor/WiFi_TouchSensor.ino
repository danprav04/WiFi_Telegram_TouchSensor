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

//Speaker
int freq = 2000;
int channel = 0;
int resolution = 8;


void setup() {
 
  Serial.begin(115200);
  pinMode(12, INPUT_PULLUP);

  //Speaker
  ledcSetup(channel, freq, resolution);
  ledcAttachPin(18, channel);
  //---

 
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

int count1 = 0;

void loop() {
      if((digitalRead(12) == HIGH) && isTouch){

        Serial.println("No Touch");
        bot.sendMessage(CHAT_ID, "No Touch", "");
        isTouch = false;
      }
      else if((digitalRead(12) == LOW) && !isTouch){
        
        Serial.println("Touch");
        bot.sendMessage(CHAT_ID, "Touch", "");
        isTouch = true;
      }
        
      delay(10);

      if((digitalRead(12) == HIGH)){
        count1+=1;
        Serial.println(count1);
        if(count1 == 200){
          Serial.println("Alert");
          noTouchAlert();
          count1 = 0;
        }
      }
      else
        count1 = 0;
}

void noTouchAlert() {
    bot.sendMessage(CHAT_ID, "No touch for 2sec", "");
    for(int i=0;i<15;i++){
      ledcWriteTone(channel, 2000);
      delay(500);
      ledcWriteTone(channel, 0);
      delay(500);
    }
}
