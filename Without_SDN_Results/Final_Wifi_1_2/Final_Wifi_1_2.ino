
/*********xbee serial port 1*************/
/*********See incoming data to normal serial port*************/

#include<Arduino.h>

String inputString = "";                // a string to hold incoming data
boolean stringComplete = false;         // whether the string is complete
String rinputString = "";               // a string to hold incoming data
boolean rstringComplete = false;        // whether the string is complete
unsigned long int timeout;
char rfdata[100];                       //array od size 100 used to store data packets
unsigned int sourceadd16b,myadd16b; 
unsigned char MY[10];
boolean rfsendok,rfok;
unsigned char RFFailure,count,maxresponse,myadd[3];
unsigned char check;
unsigned static int nodeID=0;           //Variable used to store node ID
unsigned static int checknodeID=0;      //Variable used to compare node ID with command
unsigned static int stateID=0;          //Variable used to store commands for drop packet
unsigned static int castID=0, flag=0, reqcount=-1;
String readString;
char mypkt_batt[15];        //broadcast packet array
char charBuff[9];


void setup() {                        //setupcode to run only once
  
  pinMode (3,OUTPUT);                 // Reset Wifi: connect 3 pin of arduino with WiFly reset pin 
  digitalWrite (3,LOW);               //no power at pin 3
  delay(3000);                        //a delay of 3 seconds
  digitalWrite(3,HIGH);               //giving power at pin 3 
 
 
  myadd16b=0;                         // setting its own address as 0
  Serial.begin(9600);                 // Initialize serial port and wait for it to open for communication with baudrate of 9600
  delay(100);                         //delay of 0.1 second
  Serial1.begin(9600);                // Initialize serial1 port and wait for it to open for communication with baudrate of 9600 used for zigbee
  delay(100);                         //delay of 0.1 second
  Serial2.begin(9600);                // Initialize serial2 port and wait for it to open for communication with baudrate of 9600 used for WiFly
  delay(100);                         //delay of 0.1 second
 
  Initialize_RF();                    //Function to enable API format in zigbee 
  delay(1000);                        //delay of 1 second
  Set_My_Addr();                       // Function to set my address as 0
                                      
  inputString.reserve(500);            //allocate a buffer of size 500 bytes in memory for manipulating strings
  rinputString.reserve(500);           //allocate a buffer of size 500 bytes in memory for manipulating strings
  
  Serial.println("Starting connection rn-xv module");    
  Serial2.print("$$$");                             //command use by WiFly to enter into command mode
  delay(4000);                                      //delay of 4 sec
  Serial2.println("open 10.14.18.10 1346");         //to configure/change ip and port number of the server.
  
}


void loop() {                                       //main code to run repeatedly

   if (stringComplete) {                            //Indicates that we received a string/command that needs to be processed.
    inputString = inputString.substring(9,70);      //removing the header part of the received packet to obtain data
      Serial2.println(inputString);                 //print/send the data to the server through WiFly            
      Serial.println(inputString);
      //Serial2.println("sensor");
    inputString = "";                                 // clear the string:
    stringComplete = false;                           //setting flag to indicate new string could be accepted.
  }
  
}




/*
  SerialEvent occurs whenever a new data comes in the
hardware serial RX.  This routine is run between each
time loop() runs, so using delay inside loop can delay
response.  Multiple bytes of data may be available.
*/
void serialEvent1() {                                   //Function to receive a character or string as input from Zigbee
  while (Serial1.available()) {                         //if any incoming data is available in Serial1
 
    char inChar = (char)Serial1.read();                 // get the new byte
    if(inChar!='#')                                     // add it to the inputString:
    inputString += inChar;
    
    if (inChar == '#') {                                  //concatinate a '#' with the string
       stringComplete = true;                             // if the incoming character is a newline, set a flag so the main loop can do something about it:
       
    }
  }
}




//Set it's own address

void Set_My_Addr(void)
{
    // Serial.println("**********Start set address**************");
     unsigned char i;
     unsigned int checksum;
     MY[0]=0x7E;MY[1]=0x00;MY[2]=0x06;MY[3]=0x08;MY[4]=0x4D;
     MY[5]=0x4D;MY[6]=0x59;
     MY[7]=myadd16b>>8;MY[8]=myadd16b;
     checksum=0;
     for(i=3;i<=8;i++) checksum=checksum+MY[i];
     checksum=0xFF-(unsigned char)checksum;
     MY[9]=checksum;
     rfok=0;
     for(i=0;i<10;i++) Serial1.print(MY[i]);   //set MY address
     delay(50);
     if(rfok==0) RFFailure++;
     MY[0]=0x7E;MY[1]=0x00;MY[2]=0x04;MY[3]=0x08;MY[4]=0x4D;
     MY[5]=0x57;MY[6]=0x52;MY[7]=0x01;   //ATWR
     for(i=0;i<8;i++) Serial1.print(MY[i]);
     delay(50);
     if(rfok==0)   RFFailure++;
    // Serial.println("**********End set address**************");
}



// RF configuration

void Initialize_RF(void)
{
     //xbee_status_dir_input;
     //xbee_status_pullup_on;
     //xbee_reset_dir_output;
     //xbee_reset_high;
     //Delay_ms(500);
     delay(1500);Serial1.print("+++");delay(1500);
      //Guard Time to enter into command mode
     Serial1.print("ATAP 1\r"); delay(100);
     // Configure in API mode
     Serial1.print("ATWR\r"); delay(100);
     // Write permanently in NV memory
     Serial1.print("ATCN\r"); delay(100);
     // Out of command mode
}





