/* This example will read all channels from the AS7341 and print out reported values */

#include <Adafruit_AS7341.h>

Adafruit_AS7341 as7341;

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
  
  if (!as7341.begin()){
    Serial.println("Could not find AS7341");
    while (1) { delay(10); }
  }

  as7341.setATIME(100);
  as7341.setASTEP(999);
  as7341.setGain(AS7341_GAIN_256X);
  as7341.setLEDCurrent(4);
}

void loop() {
  // Read all channels at the same time and store in as7341 object
  as7341.enableLED(false);


  switch (iter) {
    case 0 : {digitalWrite(R, LOW); digitalWrite(B, HIGH); digitalWrite(G, HIGH); break;}
    //case 1 : {digitalWrite(R, HIGH); digitalWrite(B, LOW); digitalWrite(G, HIGH); break;}
    //case 2 : {digitalWrite(R, HIGH); digitalWrite(B, HIGH); digitalWrite(G, LOW); break;}
  }
  delay(10);

  //iter++;
 // iter = iter % 3;
   if (!as7341.readAllChannels()){
    Serial.println("Error reading all channels!");
    return;
  }

  Serial.print(as7341.getChannel(AS7341_CHANNEL_415nm_F1));
  Serial.print(",");
  Serial.print(as7341.getChannel(AS7341_CHANNEL_445nm_F2));
  Serial.print(",");
  Serial.print(as7341.getChannel(AS7341_CHANNEL_480nm_F3));
  Serial.print(",");
  Serial.print(as7341.getChannel(AS7341_CHANNEL_515nm_F4));
  Serial.print(",");
  Serial.print(as7341.getChannel(AS7341_CHANNEL_555nm_F5));
  Serial.print(",");
  Serial.print(as7341.getChannel(AS7341_CHANNEL_590nm_F6));
  Serial.print(",");
  Serial.print(as7341.getChannel(AS7341_CHANNEL_630nm_F7));
  Serial.print(",");
  Serial.print(as7341.getChannel(AS7341_CHANNEL_680nm_F8));
  Serial.print(",");
  Serial.print(as7341.getChannel(AS7341_CHANNEL_CLEAR));
  Serial.print(",");
  Serial.print(as7341.getChannel(AS7341_CHANNEL_NIR));
  Serial.println("");

  as7341.enableLED(false);
  
}