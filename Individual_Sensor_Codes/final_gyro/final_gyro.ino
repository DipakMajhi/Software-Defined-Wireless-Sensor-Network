#include<Wire.h>
#include<LiquidCrystal.h>
const int MPU=0x68; //I2C address of MPU
int GyX,GyY,GyZ;
float pitch=0,roll=0,yaw=0,v_pitch,v_roll,v_yaw,a_pitch,a_roll,a_yaw;
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
Wire.write(0x43); //starts with MPU register 43(GYRO_XOUT_H) 
Wire.endTransmission(false); 
Wire.requestFrom(MPU,6,true); //requests 6 registers 
GyX=Wire.read()<<8|Wire.read(); 
GyY=Wire.read()<<8|Wire.read(); 
GyZ=Wire.read()<<8|Wire.read(); 
v_pitch=(GyX/131); 
if(v_pitch==-1) //error filtering 
{
  v_pitch=0;
} 
v_roll=(GyY/131); 

if(v_roll==1) //error filtering 
{
v_roll=0;
}
v_yaw=GyZ/131; 
a_pitch=(v_pitch*0.046); 
a_roll=(v_roll*0.046); 
a_yaw=(v_yaw*0.045); 
pitch= pitch + a_pitch; 
roll= roll + a_roll; 
yaw= yaw + a_yaw; 
delay(200);
Serial.print("$ "); 
Serial.print(pitch); 
Serial.print(" "); 
Serial.print(roll); 
Serial.print(" "); 
Serial.print(yaw);
Serial.println(" #");
}
