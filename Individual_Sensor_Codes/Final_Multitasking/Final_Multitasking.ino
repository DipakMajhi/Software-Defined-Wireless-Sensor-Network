/* Version 1.0 - 22/12/15 */
/* Dipak Majhi */


#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include<Wire.h>
/*#include <dht.h>
#define dht_dpin A1 
dht DHT;*/
        
                   // variable for reading the pin status
char incomingByte = 0;

const int MPU=0x68; //I2C address of MPU
int GyX,GyY,GyZ;
float pitch=0,roll=0,yaw=0,v_pitch,v_roll,v_yaw,a_pitch,a_roll,a_yaw;
float temp;
int scount=0;
const int analogInPin = A0; 
const int analogOutPin = 13; 
int sensorValue = 0;       
int outputValue = 0;       

bool PIRValue;
void setup()   /****** SETUP: RUNS ONCE ******/
{
  
  pinMode(10,INPUT); 
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B); //power management register 1
  Wire.write(0);
  Wire.endTransmission(true);
  Serial.begin(9600);
  
}//--(end setup )---


void loop()   /****** LOOP: RUNS CONSTANTLY ******/
{
  
    sread();
  
}


void sread()
{
    if(Serial.available()>0)
        {
        incomingByte=Serial.read();
         switch(incomingByte)
          {
            case '3':
              motionPIR();
           case '4':
             flame();
           case '5':
             gyro();
           case '6':
             gyro_tem();
           /*case '7':
            temp_humi();*/
           case '8':
             Serial.println("****Sleep Mode****");
              delay(200);
              sleep();
              Serial.println("****Awake Mode****");
              break;
           default:
             Serial.println("**Invalid option**");
             
         }
       }
}



void sleep()
{
    
    attachInterrupt(0, wake, CHANGE);
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    sleep_mode();
    sleep_disable();   
    detachInterrupt(0);
    
}

void wake() {}

void motionPIR()
{
      for(;;)
      {
        PIRValue=digitalRead(10);// read value from pin no:10
         Serial.print("$ ");
         Serial.print(PIRValue);// print value from pin no:10
         Serial.println(" #");
        delay(500);
        if(PIRValue==1)//check the condition for LED blinking
        {
          //digitalWrite(13,HIGH);
          Serial.print("$ ");
          Serial.print(PIRValue);// print value from pin no:10
          Serial.println(" #");//set pin 13 to HIGH for switch on LED
          delay(2000);
         }
        sread();
       }
}

void gyro()
{
         for(;;)
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
          delay(500);
          Serial.print("$ "); 
          Serial.print(pitch); 
          Serial.print(" "); 
          Serial.print(roll); 
          Serial.print(" "); 
          Serial.print(yaw);
          Serial.println(" #");
          sread();
        } 
}

void gyro_tem()
{
        for(;;)
         {
            Wire.beginTransmission(MPU);
            Wire.write(0x41); //starts with MPU register 41(TEMP_OUT_H)
            Wire.endTransmission(false);
            Wire.requestFrom(MPU,6,true); //requests 2 registers
            
            temp=Wire.read()<<8|Wire.read();
            temp=(temp/340)+36.53; //equation for temperature from datasheet
            Serial.print("$ Temperature = "); Serial.print(temp);
            Serial.println(" C #");
            delay(200);
            sread();
        } 
}
/*
void temp_humi()
{
        for(;;)
        {
          
          DHT.read11(dht_dpin);

          Serial.print("$ Current humidity is = ");
          Serial.print(DHT.humidity+35.5);
          Serial.println(" #");
          //Serial.print("temperature = ");
        //  Serial.print(DHT.temperature+26.5); 
        //  Serial.println("C  ");
           delay(100);
        }      
}
*/
void flame()
{
        for(;;)
        {
          
          sensorValue = analogRead(analogInPin);            
          outputValue = map(sensorValue, 0, 1023, 0, 255);  
          analogWrite(analogOutPin, outputValue);           
          Serial.print("$ sensor " );                       
          Serial.print(sensorValue);
          Serial.println(" #");  
          delay(200); 
         sread();   
       }
}

/*-----( Declare User-written Functions )-----*/


//*********( THE END )***********

