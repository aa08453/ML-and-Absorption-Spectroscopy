#include <Wire.h>
#include <Adafruit_AS7341.h>

// Multiplexer control pins
#define S0 2
#define S1 3
#define S2 4
#define S3 5
//#define led 

Adafruit_AS7341 sensor1;
Adafruit_AS7341 sensor2;

long n_samples=0;
long avg_f1=0;
long avg_f2=0;
long avg_f3=0;
long avg_f4=0;
long avg_f5=0;
long avg_f6=0;
long avg_f7=0;
long avg_f8=0;
long avg_clear=0;
long avg_nir=0;

long sum_f1=0;
long sum_f2=0;
long sum_f3=0;
long sum_f4=0;
long sum_f5=0;
long sum_f6=0;
long sum_f7=0;
long sum_f8=0;
long sum_clear=0;
long sum_nir=0;

long f1, f2, f3, f4, f5, f6, f7, f8, clear, nir;

const int channels = 10;
int readings[channels][2];  

void setup() {
  Serial.begin(115200);

  
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);

  selectMuxChannel(1);  
  initSensor(sensor1, "Sensor#1");
  sensor1.enableLED(false);

  selectMuxChannel(3);  
  initSensor(sensor2, "Sensor#2");
  sensor2.enableLED(true);

//   for (int i = 0; i < channels; i++) {
//     readings[i][0] = i;  // channel number
//     readings[i][1] =;  
//   }

//   // Print the values
//   for (int i = 0; i < channels; i++) {
//     Serial.print("Channel ");
//     Serial.print(readings[i][0]);
//     Serial.print(": ");
//     Serial.println(readings[i][1]);
//   }
// }


}


void selectMuxChannel(int channel) {
  digitalWrite(S0, bitRead(channel, 0));
  digitalWrite(S1, bitRead(channel, 1));
  digitalWrite(S2, bitRead(channel, 2));
  digitalWrite(S3, bitRead(channel, 3));
  delay(150); // wait for sensor to power up
}

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


void initSensor(Adafruit_AS7341 &sensor, const char* label){
  if (!sensor.begin()) {
    Serial.print("Could not find ");
    Serial.println(label);
    return false;
  }
  sensor.setATIME(74); // integration cycles
  sensor.setASTEP(499); //steps per cycle
  sensor.setGain(AS7341_GAIN_64X); 
  sensor.setLEDCurrent(50);

  //Integration Time (ms)=(ASTEP+1)×(ATIME+1)×2.78μs = (999+1)×(100+1)×2.78μs = 280ms delay!!!! //clock cycle duration per step
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

int shift=4;

void loop() {
  
   
  if (!sensor1.readAllChannels()) {
    Serial.print("Error reading\n ");
    //Serial.println(label);
    return false;
  }
  
    f1 = sensor1.getChannel(AS7341_CHANNEL_415nm_F1);
    f2 = sensor1.getChannel(AS7341_CHANNEL_445nm_F2);
    f3 = sensor1.getChannel(AS7341_CHANNEL_480nm_F3);
    f4 = sensor1.getChannel(AS7341_CHANNEL_515nm_F4);
    f5 = sensor1.getChannel(AS7341_CHANNEL_555nm_F5);
    f6 = sensor1.getChannel(AS7341_CHANNEL_590nm_F6);
    f7 = sensor1.getChannel(AS7341_CHANNEL_630nm_F7);
    f8 = sensor1.getChannel(AS7341_CHANNEL_680nm_F8);
    clear = sensor1.getChannel(AS7341_CHANNEL_CLEAR);
    nir = sensor1.getChannel(AS7341_CHANNEL_NIR);
 
  
  // Serial.print("values: ");
  // Serial.print("415nm: "); Serial.print(f1); Serial.print(", ");
  // Serial.print("445nm: "); Serial.print(f2); Serial.print(", ");
  // Serial.print("480nm: "); Serial.print(f3); Serial.print(", ");
  // Serial.print("515nm: "); Serial.print(f4); Serial.print(", ");
  // Serial.print("555nm: "); Serial.print(f5); Serial.print(", ");
  // Serial.print("590nm: "); Serial.print(f6); Serial.print(", ");
  // Serial.print("630nm: "); Serial.print(f7); Serial.print(", ");
  // Serial.print("680nm: "); Serial.print(f8); Serial.print(", ");
  // Serial.print("Clear: "); Serial.print(clear); Serial.print(", ");
  // Serial.print("NIR: "); Serial.println(nir);

 sum_f1+=f1;
 sum_f2+=f2;
 sum_f3+=f3;
 sum_f4+=f4;
 sum_f5+=f5;
 sum_f6+=f6;
 sum_f7+=f7;
 sum_f8+=f8;
 sum_clear+=clear;
 sum_nir+=nir;

n_samples++;
  

  if(n_samples==16){

    avg_f1=sum_f1>>shift;
    avg_f2=sum_f2>>shift;
    avg_f3=sum_f3>>shift;
    avg_f4=sum_f4>>shift;
    avg_f5=sum_f5>>shift;
    avg_f6=sum_f6>>shift;
    avg_f7=sum_f7>>shift;
    avg_f8=sum_f8>>shift;
    avg_clear=sum_clear>>shift;
    avg_nir=sum_nir>>shift;

  Serial.print("Averaged sensor#1: ");
  Serial.print("415nm: "); Serial.print(avg_f1); Serial.print(", ");
  Serial.print("445nm: "); Serial.print(avg_f2); Serial.print(", ");
  Serial.print("480nm: "); Serial.print(avg_f3); Serial.print(", ");
  Serial.print("515nm: "); Serial.print(avg_f4); Serial.print(", ");
  Serial.print("555nm: "); Serial.print(avg_f5); Serial.print(", ");
  Serial.print("590nm: "); Serial.print(avg_f6); Serial.print(", ");
  Serial.print("630nm: "); Serial.print(avg_f7); Serial.print(", ");
  Serial.print("680nm: "); Serial.print(avg_f8); Serial.print(", ");
  Serial.print("Clear: "); Serial.print(avg_clear); Serial.print(", ");
  Serial.print("NIR: "); Serial.println(avg_nir);

    n_samples=0;
    sum_f1 = sum_f2 = sum_f3 = sum_f4 = sum_f5 = sum_f6 = sum_f7 = sum_f8 = sum_clear = sum_nir = 0;
    

  }
  
  
}
