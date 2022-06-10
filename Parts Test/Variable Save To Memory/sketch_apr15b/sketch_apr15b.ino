#include <Preferences.h>

Preferences preferences;

void setup() {
  Serial.begin(115200);
  Serial.println();

  preferences.begin("my-app", false);

  int a = preferences.getInt("a", 12000);
  Serial.println(a);

  preferences.putInt("a", 42524543);
  Serial.println("Putted new value");
}

void loop() {
  // put your main code here, to run repeatedly:

}
