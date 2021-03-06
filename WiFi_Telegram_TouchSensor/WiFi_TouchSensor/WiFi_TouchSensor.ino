#include "WiFi.h"
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Preferences.h>

int LED = 14;
int Sensor = 12;

const char* ssid = "Ira";
const char* password =  "0022446688";

#define BOTtoken "5123186265:AAFJXLrM0Sa3y_YEVg72CrGXUk6RevcUKkU"
#define CHAT_ID "547696948"

int restartTime = 0; // 0-23

Preferences preferences;

bool isTouch = false;

WiFiClientSecure client;

UniversalTelegramBot bot(BOTtoken, client);

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp;
int hour;
int minute;
int second;

//Speaker
int SPIN = 19;
int freq = 2000;
int channel = 0;
int resolution = 8;
int delayBetweanAlerts;


void setup() {
 
  Serial.begin(115200);
  pinMode(Sensor, INPUT_PULLUP);

  pinMode(LED, OUTPUT);

  preferences.begin("TDOC", false);

  //Speaker
  ledcSetup(channel, freq, resolution);
  ledcAttachPin(18, channel);
  delayBetweanAlerts = preferences.getInt("dba", 12000);
  pinMode(SPIN, OUTPUT);
  digitalWrite(SPIN, LOW);
  //---

 
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);

  int countTimeToRestart = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
    countTimeToRestart += 1;
    if(countTimeToRestart > 20)
      ESP.restart();
  }
 
  Serial.println("Connected to the WiFi network");
  Serial.println(WiFi.localIP());

// Initialize a NTPClient to get time
  timeClient.begin();
  timeClient.setTimeOffset(10800);

  xTaskCreatePinnedToCore(
    CountTime,      // Function that should be called
    "CountTime",    // Name of the task (for debugging)
    2048,               // Stack size (bytes)
    NULL,               // Parameter to pass
    1,                  // Task priority
    NULL,               // Task handle
    1          // Core you want to run the task on (0 or 1)
);

  xTaskCreatePinnedToCore(
    GetTime,      // Function that should be called
    "GetTime",    // Name of the task (for debugging)
    2048,               // Stack size (bytes)
    NULL,               // Parameter to pass
    1,                  // Task priority
    NULL,               // Task handle
    1          // Core you want to run the task on (0 or 1)
);
  
  bot.sendMessage(CHAT_ID, "Bot started up", "");
}

void GetTime(void * parameter){
  for(;;){
    while(!timeClient.update())
      timeClient.forceUpdate();
    formattedDate = timeClient.getFormattedDate();
    
    // Extract time
    int splitT = formattedDate.indexOf("T");
    timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
    hour = timeStamp.substring(0, 2).toInt();
    minute = timeStamp.substring(3, 5).toInt();
    second = timeStamp.substring(6, 8).toInt();
    delay(1000);
  }
}

int count1 = 0;

void CountTime(void * parameter){
  for(;;){
      delay(10);
      if((digitalRead(Sensor) == HIGH))
        count1 += 1;
      else
        count1 = 0;
  }
    vTaskDelete(NULL);
}


void loop() {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
      if((digitalRead(Sensor) == HIGH) && isTouch){
        Serial.println("No Touch");
        bot.sendMessage(CHAT_ID, "No Touch", "");
        isTouch = false;
      }
      else if((digitalRead(Sensor) == LOW) && !isTouch){
        Serial.println("Touch");
        bot.sendMessage(CHAT_ID, "Touch", "");
        isTouch = true;
      }

      if((digitalRead(Sensor) == HIGH)){
        Serial.print(count1); Serial.print(" / "); Serial.println(delayBetweanAlerts);
        if(count1 > delayBetweanAlerts){
          Serial.println("Alert");
          noTouchAlert();
          count1 = 0;
        }
      }
      else
        count1 = 0;

      if(hour == restartTime && minute == 0 && second <= 10)
        ESP.restart();
      
      delay(10);
}

void noTouchAlert() {
    bot.sendMessage(CHAT_ID, "Alert", "");
    digitalWrite(SPIN, HIGH);
    for(int i=0;i<15;i++){
      ledcWriteTone(channel, 2000);
      digitalWrite(LED, HIGH);
      delay(500);
      ledcWriteTone(channel, 0);
      digitalWrite(LED, LOW);
      delay(500);
    }
    digitalWrite(SPIN, LOW);
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
      preferences.putInt("dba", delayBetweanAlerts);
      bot.sendMessage(chat_id, "Done", "");
    }
    else if (text.indexOf("/alert") >= 0) {
      noTouchAlert();
      bot.sendMessage(chat_id, "Done", "");
    }
    else
     bot.sendMessage(chat_id, "I can't understand you.", "");
  }
}
