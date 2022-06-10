#include <EEPROM.h>

void setup() {
  Serial.begin(115200);
  Serial.println("Started");

  
  int a = EEPROM.read(1);
  Serial.println("Get a");
  //EEPROM.get(1, a);
  Serial.println(a);

  EEPROM.write(1, 2);
  EEPROM.commit();
  
  int b = EEPROM.read(1);
  Serial.println("Get b");
  //EEPROM.get(1, b);
  Serial.println(b);
}

void loop() {
  // put your main code here, to run repeatedly:

}
