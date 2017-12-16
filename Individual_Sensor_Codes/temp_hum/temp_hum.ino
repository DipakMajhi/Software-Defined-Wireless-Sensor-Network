#include <dht.h>
#define dht_dpin A1 
dht DHT;
void setup(){
  Serial.begin(9600);
  delay(500);
  //Serial.println("Humidity and temperature\n\n");
  //delay(1000);
}
void loop()
{
  DHT.read11(dht_dpin);

    Serial.print("$ Current humidity is = ");
    Serial.print(DHT.humidity+35.5);
    Serial.println(" #");
    //Serial.print("temperature = ");
  //  Serial.print(DHT.temperature+26.5); 
  //  Serial.println("C  ");
  delay(50);
}
