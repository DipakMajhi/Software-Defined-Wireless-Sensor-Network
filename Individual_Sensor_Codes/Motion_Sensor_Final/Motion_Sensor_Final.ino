
void setup()
  {
    //pinMode(13,OUTPUT);//define pin 13 as output LED pin
    pinMode(10,INPUT);//define pin 10 as input pin
    pinMode(4, OUTPUT);
    digitalWrite(4,HIGH);
    Serial.begin(9600);// initialize serial communications:
  }
  bool PIRValue;// variable to read the PIR sensor value
  void loop()
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
  }
