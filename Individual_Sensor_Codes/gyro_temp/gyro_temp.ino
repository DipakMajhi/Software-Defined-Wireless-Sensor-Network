#include<Wire.h>
#include<LiquidCrystal.h>
#include<MPU6050.h>
const int MPU=0x68; //I2C address of MPU
float temp;
void setup()
{
Wire.begin();
Wire.beginTransmission(MPU);
Wire.write(0x6B); //power management register 1
Wire.write(0);
Wire.endTransmission(true);
Serial.begin(9600);
}

void loop()
{
Wire.beginTransmission(MPU);
Wire.write(0x3B); //starts with MPU register 41(TEMP_OUT_H)
Wire.endTransmission(false);
Wire.requestFrom(MPU,14,true); //requests 2 registers

temp=Wire.read()<<8|Wire.read();
temp=(temp/340)+36.53; //equation for temperature from datasheet
Serial.print("$ Temperature = "); Serial.print(temp);
Serial.println(" C #");
delay(1000);
}
