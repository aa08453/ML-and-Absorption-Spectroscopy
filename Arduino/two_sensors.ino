#include <Adafruit_AS7341.h>

Adafruit_AS7341 a_as7341;
Adafruit_AS7341 b_as7341;
Adafruit_AS7341 c_as7341;

#define B 13
#define G 12
#define R 11

int iter = 0;

int clamp(int x) {
  if (x == 0) {
    return LOW;
  }
  else {
    return HIGH;
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(B, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(R, OUTPUT);
  

  // Wait for communication with the host computer serial monitor
  while (!Serial) {
    delay(1);
  }
  
  if (!a_as7341.begin()){
    Serial.println("Could not find a_as7341");
    while (1) { delay(10); }
  }
   if (!b_as7341.begin()){
    Serial.println("Could not find b_as7341");
    while (1) { delay(10); }
  }
  // if (!c_as7341.begin()){
  //   Serial.println("Could not find c_as7341");
  //   while (1) { delay(10); }
  // }
  a_as7341.setATIME(100);
  a_as7341.setASTEP(999);
  a_as7341.setGain(AS7341_GAIN_256X);
  a_as7341.setLEDCurrent(4);
  b_as7341.setATIME(100);
  b_as7341.setASTEP(999);
  b_as7341.setGain(AS7341_GAIN_256X);
  // c_as7341.setLEDCurrent(4);
  // c_as7341.setATIME(100);
  // c_as7341.setASTEP(999);
  // c_as7341.setGain(AS7341_GAIN_256X);
  // c_as7341.setLEDCurrent(4);
}

void loop() {
  // Read all channels at the same time and store in a_as7341 object
  a_as7341.enableLED(false);
  b_as7341.enableLED(false);
  //c_as7341.enableLED(false);


  switch (iter) {
    case 0 : {digitalWrite(R, HIGH); digitalWrite(B, HIGH); digitalWrite(G, LOW); break;}
    //case 1 : {digitalWrite(R, HIGH); digitalWrite(B, LOW); digitalWrite(G, HIGH); break;}
    //case 2 : {digitalWrite(R, HIGH); digitalWrite(B, HIGH); digitalWrite(G, LOW); break;}
  }
  //delay(10);

 
   if (!a_as7341.readAllChannels()){
    Serial.println("Error reading all channels!");
    return;
  }
  if (!b_as7341.readAllChannels()){
    Serial.println("Error reading all channels!");
    return;}
  //   if (!c_as7341.readAllChannels()){
  //   Serial.println("Error reading all channels!");
  //   return;
  // }
  Serial.print("Sensor#1:");
  Serial.print(a_as7341.getChannel(AS7341_CHANNEL_415nm_F1));
  Serial.print(",");
  Serial.print(a_as7341.getChannel(AS7341_CHANNEL_445nm_F2));
  Serial.print(",");
  Serial.print(a_as7341.getChannel(AS7341_CHANNEL_480nm_F3));
  Serial.print(",");
  Serial.print(a_as7341.getChannel(AS7341_CHANNEL_515nm_F4));
  Serial.print(",");
  Serial.print(a_as7341.getChannel(AS7341_CHANNEL_555nm_F5));
  Serial.print(",");
  Serial.print(a_as7341.getChannel(AS7341_CHANNEL_590nm_F6));
  Serial.print(",");
  Serial.print(a_as7341.getChannel(AS7341_CHANNEL_630nm_F7));
  Serial.print(",");
  Serial.print(a_as7341.getChannel(AS7341_CHANNEL_680nm_F8));
  Serial.print(",");
  Serial.print(a_as7341.getChannel(AS7341_CHANNEL_CLEAR));
  Serial.print(",");
  Serial.print(a_as7341.getChannel(AS7341_CHANNEL_NIR));
  Serial.println("");

  Serial.print("Sensor#2:");
  Serial.print(b_as7341.getChannel(AS7341_CHANNEL_415nm_F1));
  Serial.print(",");
  Serial.print(b_as7341.getChannel(AS7341_CHANNEL_445nm_F2));
  Serial.print(",");
  Serial.print(b_as7341.getChannel(AS7341_CHANNEL_480nm_F3));
  Serial.print(",");
  Serial.print(b_as7341.getChannel(AS7341_CHANNEL_515nm_F4));
  Serial.print(",");
  Serial.print(b_as7341.getChannel(AS7341_CHANNEL_555nm_F5));
  Serial.print(",");
  Serial.print(b_as7341.getChannel(AS7341_CHANNEL_590nm_F6));
  Serial.print(",");
  Serial.print(b_as7341.getChannel(AS7341_CHANNEL_630nm_F7));
  Serial.print(",");
  Serial.print(b_as7341.getChannel(AS7341_CHANNEL_680nm_F8));
  Serial.print(",");
  Serial.print(b_as7341.getChannel(AS7341_CHANNEL_CLEAR));
  Serial.print(",");
  Serial.print(b_as7341.getChannel(AS7341_CHANNEL_NIR));
  Serial.println("");

  // Serial.print("Sensor#3:");
  // Serial.print(c_as7341.getChannel(AS7341_CHANNEL_415nm_F1));
  // Serial.print(",");
  // Serial.print(c_as7341.getChannel(AS7341_CHANNEL_445nm_F2));
  // Serial.print(",");
  // Serial.print(c_as7341.getChannel(AS7341_CHANNEL_480nm_F3));
  // Serial.print(",");
  // Serial.print(c_as7341.getChannel(AS7341_CHANNEL_515nm_F4));
  // Serial.print(",");
  // Serial.print(c_as7341.getChannel(AS7341_CHANNEL_555nm_F5));
  // Serial.print(",");
  // Serial.print(c_as7341.getChannel(AS7341_CHANNEL_590nm_F6));
  // Serial.print(",");
  // Serial.print(c_as7341.getChannel(AS7341_CHANNEL_630nm_F7));
  // Serial.print(",");
  // Serial.print(c_as7341.getChannel(AS7341_CHANNEL_680nm_F8));
  // Serial.print(",");
  // Serial.print(c_as7341.getChannel(AS7341_CHANNEL_CLEAR));
  // Serial.print(",");
  // Serial.print(c_as7341.getChannel(AS7341_CHANNEL_NIR));
  // Serial.println("");

Serial.print("Difference:");
 Serial.print(abs((int)a_as7341.getChannel(AS7341_CHANNEL_415nm_F1) - (int)b_as7341.getChannel(AS7341_CHANNEL_415nm_F1)));
 Serial.print(",");
Serial.print(abs((int)a_as7341.getChannel(AS7341_CHANNEL_445nm_F2) - (int)b_as7341.getChannel(AS7341_CHANNEL_445nm_F2)));
Serial.print(",");
Serial.print(abs((int)a_as7341.getChannel(AS7341_CHANNEL_480nm_F3) - (int)b_as7341.getChannel(AS7341_CHANNEL_480nm_F3)));
Serial.print(",");
Serial.print(abs((int)a_as7341.getChannel(AS7341_CHANNEL_515nm_F4) - (int)b_as7341.getChannel(AS7341_CHANNEL_515nm_F4)));
Serial.print(",");
Serial.print(abs((int)a_as7341.getChannel(AS7341_CHANNEL_555nm_F5) - (int)b_as7341.getChannel(AS7341_CHANNEL_555nm_F5)));
Serial.print(",");
Serial.print(abs((int)a_as7341.getChannel(AS7341_CHANNEL_590nm_F6) - (int)b_as7341.getChannel(AS7341_CHANNEL_590nm_F6)));
Serial.print(",");
Serial.print(abs((int)a_as7341.getChannel(AS7341_CHANNEL_630nm_F7) - (int)b_as7341.getChannel(AS7341_CHANNEL_630nm_F7)));
Serial.print(",");
Serial.print(abs((int)a_as7341.getChannel(AS7341_CHANNEL_680nm_F8) - (int)b_as7341.getChannel(AS7341_CHANNEL_680nm_F8)));
Serial.print(",");
Serial.print(abs((int)a_as7341.getChannel(AS7341_CHANNEL_CLEAR) - (int)b_as7341.getChannel(AS7341_CHANNEL_CLEAR)));
Serial.print(",");
Serial.print(abs((int)a_as7341.getChannel(AS7341_CHANNEL_NIR) - (int)b_as7341.getChannel(AS7341_CHANNEL_NIR)));
Serial.println("");

  //c_as7341.enableLED(false);
  
  }