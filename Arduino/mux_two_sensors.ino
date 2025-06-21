#include <Wire.h>
#include <Adafruit_AS7341.h>

#define S0 2
#define S1 3
#define S2 4
#define S3 5

Adafruit_AS7341 sensor;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
}

void selectMuxChannel(uint8_t channel) {
  digitalWrite(S0, bitRead(channel, 0));
  digitalWrite(S1, bitRead(channel, 1));
  digitalWrite(S2, bitRead(channel, 2));
  digitalWrite(S3, bitRead(channel, 3));
  delay(150);  // settle time
}

void readSensor(uint8_t muxChannel, const char* label) {
  selectMuxChannel(muxChannel);
  delay(100);

  if (!sensor.begin()) {
    Serial.print("Could not find ");
    Serial.println(label);
    return;
  }

  sensor.setATIME(100);
  sensor.setASTEP(999);
  sensor.setGain(AS7341_GAIN_256X);

  if (!sensor.readAllChannels()) {
    Serial.print("Error reading ");
    Serial.println(label);
    return;
  }

  Serial.print(label); Serial.print(": ");
  Serial.print(sensor.getChannel(AS7341_CHANNEL_415nm_F1)); Serial.print(", ");
  Serial.print(sensor.getChannel(AS7341_CHANNEL_445nm_F2)); Serial.print(", ");
  Serial.print(sensor.getChannel(AS7341_CHANNEL_480nm_F3)); Serial.print(", ");
  Serial.print(sensor.getChannel(AS7341_CHANNEL_515nm_F4)); Serial.print(", ");
  Serial.print(sensor.getChannel(AS7341_CHANNEL_555nm_F5)); Serial.print(", ");
  Serial.print(sensor.getChannel(AS7341_CHANNEL_590nm_F6)); Serial.print(", ");
  Serial.print(sensor.getChannel(AS7341_CHANNEL_630nm_F7)); Serial.print(", ");
  Serial.print(sensor.getChannel(AS7341_CHANNEL_680nm_F8)); Serial.print(", ");
  Serial.print(sensor.getChannel(AS7341_CHANNEL_CLEAR)); Serial.print(", ");
  Serial.println(sensor.getChannel(AS7341_CHANNEL_NIR));
}

void loop() {
  readSensor(14, "Sensor#1");  // C0
  readSensor(15, "Sensor#2");  // C1
  Serial.println("-------------------------");
  delay(1000);
}
