#include <Wire.h>
#include <Adafruit_AS7341.h>

// Multiplexer control pins
#define S0 2
#define S1 3
#define S2 4
#define S3 5

// LED control pins (optional)
#define B 13
#define G 12
#define R 11

Adafruit_AS7341 sensor1;
Adafruit_AS7341 sensor2;

void setup() {
  Serial.begin(115200);

  // Set mux control pins as outputs
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);

  // Set optional RGB LED pins
  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);

  digitalWrite(R, HIGH);
  digitalWrite(G, HIGH);
  digitalWrite(B, LOW);
}

// Select one of the 16 mux channels (0–15)
void selectMuxChannel(int channel) {
  digitalWrite(S0, bitRead(channel, 0));
  digitalWrite(S1, bitRead(channel, 1));
  digitalWrite(S2, bitRead(channel, 2));
  digitalWrite(S3, bitRead(channel, 3));
  delay(150); // wait for sensor to power up
}

// Initialize and read a sensor
bool readSensor(Adafruit_AS7341 &sensor, const char* label) {
  
  if (!sensor.readAllChannels()) {
    Serial.print("Error reading ");
    Serial.println(label);
    return false;
  }

  Serial.print(label);
  Serial.print(": ");
  Serial.print("415nm: ");Serial.print(sensor.getChannel(AS7341_CHANNEL_415nm_F1)); Serial.print(", ");
  Serial.print("445nm: ");Serial.print(sensor.getChannel(AS7341_CHANNEL_445nm_F2)); Serial.print(", ");
  Serial.print("480nm: ");Serial.print(sensor.getChannel(AS7341_CHANNEL_480nm_F3)); Serial.print(", ");
  Serial.print("515nm: ");Serial.print(sensor.getChannel(AS7341_CHANNEL_515nm_F4)); Serial.print(", ");
  Serial.print("555nm: ");Serial.print(sensor.getChannel(AS7341_CHANNEL_555nm_F5)); Serial.print(", ");
  Serial.print("590nm: ");Serial.print(sensor.getChannel(AS7341_CHANNEL_590nm_F6)); Serial.print(", ");
  Serial.print("630nm: ");Serial.print(sensor.getChannel(AS7341_CHANNEL_630nm_F7)); Serial.print(", ");
  Serial.print("680nm: ");Serial.print(sensor.getChannel(AS7341_CHANNEL_680nm_F8)); Serial.print(", ");
  Serial.print("CLEAR: ");Serial.print(sensor.getChannel(AS7341_CHANNEL_CLEAR));    Serial.print(", ");
  Serial.print("NIR: ");Serial.println(sensor.getChannel(AS7341_CHANNEL_NIR));

  return true;
}

long f1, f2, f3, f4, f5, f6, f7, f8, clear, nir;

void average(Adafruit_AS7341 &sensor){
  int count = 20;
  f1 = f2 = f3 = f4 = f5 = f6 = f7 = f8 = clear = nir = 0;

  for (int i = 0; i < count; i++) {
    if (!sensor.readAllChannels()) {
      Serial.println("Error reading sensor during averaging.");
      continue; // skip this reading if it fails
    }

    f1 += sensor.getChannel(AS7341_CHANNEL_415nm_F1);
    f2 += sensor.getChannel(AS7341_CHANNEL_445nm_F2);
    f3 += sensor.getChannel(AS7341_CHANNEL_480nm_F3);
    f4 += sensor.getChannel(AS7341_CHANNEL_515nm_F4);
    f5 += sensor.getChannel(AS7341_CHANNEL_555nm_F5);
    f6 += sensor.getChannel(AS7341_CHANNEL_590nm_F6);
    f7 += sensor.getChannel(AS7341_CHANNEL_630nm_F7);
    f8 += sensor.getChannel(AS7341_CHANNEL_680nm_F8);
    clear += sensor.getChannel(AS7341_CHANNEL_CLEAR);
    nir += sensor.getChannel(AS7341_CHANNEL_NIR);

    
  }

  f1 /= count;
  f2 /= count;
  f3 /= count;
  f4 /= count;
  f5 /= count;
  f6 /= count;
  f7 /= count;
  f8 /= count;
  clear /= count;
  nir /= count;

  Serial.print("Averaged values: ");
  Serial.print("415nm: "); Serial.print(f1); Serial.print(", ");
  Serial.print("445nm: "); Serial.print(f2); Serial.print(", ");
  Serial.print("480nm: "); Serial.print(f3); Serial.print(", ");
  Serial.print("515nm: "); Serial.print(f4); Serial.print(", ");
  Serial.print("555nm: "); Serial.print(f5); Serial.print(", ");
  Serial.print("590nm: "); Serial.print(f6); Serial.print(", ");
  Serial.print("630nm: "); Serial.print(f7); Serial.print(", ");
  Serial.print("680nm: "); Serial.print(f8); Serial.print(", ");
  Serial.print("Clear: "); Serial.print(clear); Serial.print(", ");
  Serial.print("NIR: "); Serial.println(nir);
}


void initSensor(Adafruit_AS7341 &sensor, const char* label){
  if (!sensor.begin()) {
    Serial.print("Could not find ");
    Serial.println(label);
    return false;
  }
  sensor.setATIME(100);
  sensor.setASTEP(999);
  sensor.setGain(AS7341_GAIN_256X);
  sensor.setLEDCurrent(4);
}

void difference(Adafruit_AS7341 &sensor1, Adafruit_AS7341 &sensor2){
  Serial.print("Difference:");
  Serial.print(abs((int)sensor1.getChannel(AS7341_CHANNEL_415nm_F1) - (int)sensor2.getChannel(AS7341_CHANNEL_415nm_F1)));
  Serial.print(",");
  Serial.print(abs((int)sensor1.getChannel(AS7341_CHANNEL_445nm_F2) - (int)sensor2.getChannel(AS7341_CHANNEL_445nm_F2)));
  Serial.print(",");
  Serial.print(abs((int)sensor1.getChannel(AS7341_CHANNEL_480nm_F3) - (int)sensor2.getChannel(AS7341_CHANNEL_480nm_F3)));
  Serial.print(",");
  Serial.print(abs((int)sensor1.getChannel(AS7341_CHANNEL_515nm_F4) - (int)sensor2.getChannel(AS7341_CHANNEL_515nm_F4)));
  Serial.print(",");
  Serial.print(abs((int)sensor1.getChannel(AS7341_CHANNEL_555nm_F5) - (int)sensor2.getChannel(AS7341_CHANNEL_555nm_F5)));
  Serial.print(",");
  Serial.print(abs((int)sensor1.getChannel(AS7341_CHANNEL_590nm_F6) - (int)sensor2.getChannel(AS7341_CHANNEL_590nm_F6)));
  Serial.print(",");
  Serial.print(abs((int)sensor1.getChannel(AS7341_CHANNEL_630nm_F7) - (int)sensor2.getChannel(AS7341_CHANNEL_630nm_F7)));
  Serial.print(",");
  Serial.print(abs((int)sensor1.getChannel(AS7341_CHANNEL_680nm_F8) - (int)sensor2.getChannel(AS7341_CHANNEL_680nm_F8)));
  Serial.print(",");
  Serial.print(abs((int)sensor1.getChannel(AS7341_CHANNEL_CLEAR) - (int)sensor2.getChannel(AS7341_CHANNEL_CLEAR)));
  Serial.print(",");
  Serial.print(abs((int)sensor1.getChannel(AS7341_CHANNEL_NIR) - (int)sensor2.getChannel(AS7341_CHANNEL_NIR)));
  Serial.println("");
  }


void loop() {
  selectMuxChannel(14);  
  initSensor(sensor1, "Sensor#1");
  average(sensor1);
  sensor1.enableLED(false);

  // selectMuxChannel(15);  
  // initSensor(sensor2, "Sensor#2");
  // sensor2.enableLED(true);

  // difference(sensor1, sensor2);

  Serial.println("-----------------------------");
  
}