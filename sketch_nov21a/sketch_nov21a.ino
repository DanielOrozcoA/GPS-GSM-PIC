#include <SoftwareSerial.h>

SoftwareSerial sim(11,12);

char mensj[50];
int cc = 0;

void setup() {
  Serial.begin(9600);
  sim.begin(9600);
  delay(2000);
}

void loop() {
  while(Serial.available()){
      byte dato = Serial.read();
      sim.write(dato);
    }
    
  while(sim.available()){
      byte dato = sim.read();
      Serial.write(dato);
    }
}
