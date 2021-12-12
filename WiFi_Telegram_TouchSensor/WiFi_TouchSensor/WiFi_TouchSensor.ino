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
int delayBetweanAlerts;


void setup() {
 
  Serial.begin(115200);
  pinMode(12, INPUT_PULLUP);

  //Speaker
  ledcSetup(channel, freq, resolution);
  ledcAttachPin(18, channel);
  delayBetweanAlerts = 90;
  pinMode(19, OUTPUT);
  digitalWrite(19, LOW);
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

  xTaskCreatePinnedToCore(
    TelegramCheck,      // Function that should be called
    "TelegramCheck",    // Name of the task (for debugging)
    2048,               // Stack size (bytes)
    NULL,               // Parameter to pass
    1,                  // Task priority
    NULL,               // Task handle
    1          // Core you want to run the task on (0 or 1)
);
  
  bot.sendMessage(CHAT_ID, "Bot started up", "");
}

int count1 = 0;

void TelegramCheck(void * parameter){
  for(;;){
//       int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
//  
//      while(numNewMessages) {
//        //Serial.println("got response");
//        handleNewMessages(numNewMessages);
//        numNewMessages = bot.getUpdates(bot.last_message_received + 1);
//      }
        
      delay(10);

      if((digitalRead(12) == HIGH)){
        count1+=1;
//        Serial.print(count1); Serial.print(" / "); Serial.println(delayBetweanAlerts);
//        if(count1 > delayBetweanAlerts){
//          Serial.println("Alert");
//          noTouchAlert();
//          count1 = 0;
//        }
      }
      else
        count1 = 0;
  }
  
    vTaskDelete(NULL);
}


void loop() {
//  if(!isTouch){
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
//  }
    
  
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
//        count1+=1;
        Serial.print(count1); Serial.print(" / "); Serial.println(delayBetweanAlerts);
        if(count1 > delayBetweanAlerts){
          Serial.println("Alert");
          noTouchAlert();
          count1 = 0;
        }
      }
      else
        count1 = 0;
}

void noTouchAlert() {
    bot.sendMessage(CHAT_ID, "Alert", "");
    digitalWrite(19, HIGH);
    for(int i=0;i<15;i++){
      ledcWriteTone(channel, 2000);
      delay(500);
      ledcWriteTone(channel, 0);
      delay(500);
    }
    digitalWrite(19, LOW);
}

void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    
    // Print the received message
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;
    
    if (text.indexOf("/alertset") >= 0) {
      text.remove(0, 10);
      delayBetweanAlerts = text.toInt();
      bot.sendMessage(chat_id, "Done", "");
    }
    if (text.indexOf("/alert") >= 0) {
      noTouchAlert();
      bot.sendMessage(chat_id, "Done", "");
    }
    else
     bot.sendMessage(chat_id, "I can't understand you.", "");
  }
}
