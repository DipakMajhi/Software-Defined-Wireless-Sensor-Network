
/*
Date   :      14/01/2016
Author :      DIPAK MAJHI.

&02*02#       Sensor change (Flame)
&02*01#       Sensor change (Motion PIR)
!02*10#       Set Delay
$02*01#       Set Destination
@02*01#       Drop Packet
@02*00#       Forward Packet
*/

#include <ArduinoJson.h>
#include <Arduino.h>
#include <Time.h>

const int analogInPin = A0; 
const int analogOutPin = 13; 
int sensorValue = 0;       
int outputValue = 0;  
bool PIRValue;                                  // variable to read the PIR sensor value 
int i;

String inputString = "";                        // a string to hold incoming data
boolean stringComplete = false; 
unsigned long int timeout;
unsigned char rfdata[200];                      //array to hold the data packet
unsigned int sourceadd16b,myadd16b;
unsigned char MY[10];
unsigned static int dest=0,del=0;             //setting default values for destination address and delay
boolean rfsendok,rfok;
unsigned char RFFailure,count,maxresponse,myadd[3];
unsigned char check;
unsigned char sent=0;
float temp;
unsigned char flag=0;
String nodeID,state,activity,destADD,sensorID,startID,delaytime,checkst;   //Variable used to extract the neccesary information from the received command
unsigned static int flagch=0, amtsend=2, reqcount=0;
String hr="",minit="",sec="";

void setup() {                      //setupcode to run only once
  
  pinMode(10,INPUT);                //define pin 10 as input pin  
  myadd16b = 2;                     //Setting its own address as 2
  Serial.begin(9600);               // Initialize serial and wait for port to open for communication with baudrate of 9600
  delay(100);                       //delay of 0.1 sec
  Serial1.begin(9600);              // Initialize serial1 and wait for port to open for communication with baudrate of 9600
   delay(100);                      //delay of 0.1 sec
   inputString.reserve(500);        //allocate a buffer of size 500 bytes in memory for manipulating strings . 
  Initialize_RF();                  //Function to enable API format in zigbee 
  delay(100);                       //delay of 0.1 sec
  Set_My_Addr();                    // Function to set my address as 2
  while (!Serial) {
    ;                                // wait for serial port to connect. Needed for native USB port only
  }
}



void loop() {                                   //main code to run repeatedly

  char sent=0;
  char charBuff[6]={'s','e','n','s','o','#'};   //Initialization of a Character array 
  sent=Send_data(0,&charBuff[0]);               //sending the character array to establish connection with the controller which is at address 0.
  delay(1000);                                  //delay of 1.8 sec.
    if(stringComplete==true)                    //Indicates that we received a string/command that needs to be processed.
      choose();                                 // Function to process the received string

    hr=String(hour());
    minit=String(minute());
    sec=String(second());
    
    if(flag==1)                                 //a check to excecute the PIR Motion sensor function
      {
        motionPIR();                            //excecute the PIR Motion sensor function
      }

    if(flag==2)                                 //a check to excecute the Flame sensor function
      {
        flame();                                //excecute the Flame sensor function
      }    
}



void serialEvent1() {                           //Function to receive a character or string as input
  while (Serial1.available()) {                 //if any incoming data is available in Serial1
                                                
    char inChar = (char)Serial1.read();           // get the new byte:
    if(inChar=='&'||inChar=='@'||inChar=='!'||inChar=='$'||inChar=='0'||inChar=='1'||inChar=='2'||inChar=='3'||inChar=='4'||inChar=='*'||inChar=='#'||inChar=='5'||inChar=='6'||inChar=='7'||inChar=='8'||inChar=='9')    //if we receive any character in {@,#,$,!,&,0,1,2,3} we store it
    inputString += inChar;                        // add it to the inputString:
    
    if (inChar == '#') {         
      //inputString += '#';                         //concatinate a '#' with the string
      stringComplete = true;                      // if the incoming character is a newline, set a flag so the main loop can do something about it:
      reqcount++;
    }
  }
}


void choose()
{
    if (stringComplete) 
    {
    Serial.print(reqcount);
    Serial.print("  ");
    Serial.println(inputString);
    startID = inputString.substring(0,1);         //storing the first character at 0th position from the string to get its category
    checkst = inputString.substring(1,2);
    if(startID=="#"||startID=="0"||startID=="1"||startID=="2"||startID=="3"||startID=="4"||startID=="*"||checkst=="&"||checkst=="@"||checkst=="!"||checkst=="$")  
    {
      startID = inputString.substring(1,2);
      nodeID=inputString.substring(2,4);            //storing the next characters at 1st and 2nd position from the string to get its node ID
      sensorID=inputString.substring(5,7);          //storing the next characters at 3rd and 4th position from the string to get its sensor ID
      destADD=inputString.substring(5,7);           //extracting destination address to set/change.
      delaytime=inputString.substring(5,7);         //extracting delay time to set/change.
    }
    else
    {
      nodeID=inputString.substring(1,3);            //storing the next characters at 1st and 2nd position from the string to get its node ID
      sensorID=inputString.substring(4,6);          //storing the next characters at 3rd and 4th position from the string to get its sensor ID
      destADD=inputString.substring(4,6);           //extracting destination address to set/change.
      delaytime=inputString.substring(4,6);         //extracting delay time to set/change.
    }
    inputString = "";                             //setting input sting blank again
    stringComplete = false;                       //setting flag to indicate new string could be accepted.

    if(startID=="&")                              // to perform switching between multiple sensor nodes.
          sread();

          
    if(startID=="$")                              // to change the destination address of a node
     {
         if(nodeID =="02" ||nodeID=="00")                        //to check if its for NODE 2
         {
          dest=destADD.toInt();                   //to convert the string to integer and storing the destination address
         }
      }


    if(startID=="@")						// used to check for packet drop/ forward
    {
    if(nodeID=="02"||nodeID=="00")
      {
        if(sensorID=="01")                                      //check to enable drop mode
           { 
             flagch=1;
           } 
        else if(sensorID=="00")                                  //check to enable forward mode
            { 
             flagch=0;
            }
      }
    } 


      if(startID=="!")                            // for user input Delay time.
      {
         if(nodeID=="02" ||nodeID=="00")                         //to check if its for NODE 2
          {
             del=delaytime.toInt();               //to convert the string to integer and storing the delay time
             del=del*100;                         // multiply by 100 to convert it into seconds.
          }
       }
    }
}
  


void sread()                                      //Function to initiate or change sensor
{
    if(nodeID=="02" ||nodeID=="00")
    {
        amtsend--;
        if(sensorID=="01")                        //check for Motion PIR sensor
           { 
             flag=1;
           } 
        else if(sensorID=="02")                   //check for flame sensor
            { 
             flag=2;
            }
    }
}



//Initiate Motion PIR sensor

char motionPIR()
{
      PIRValue=digitalRead(10);                                 //reading sensor vaue
      
      String s1 = "${"+String(amtsend)+","+String(reqcount)+", \"ID\":" + String(myadd16b);               //forming a Json string of data
      String s2 = ",1, "+hr+":"+minit+":"+sec+", \"Data \":["+String(PIRValue)+"]}##";          
      String jsonString = s1+s2;                                //concatenating the individual strings
      int len = jsonString.length();                            //finding strig length
      char charBuff[len];
      jsonString.toCharArray(charBuff,len-1);                    //converting string to array of characters
      if (flagch==0)
      {
        sent=Send_data(dest,&charBuff[0]);                         //sending destiation address with data packet to form a API packet
        amtsend++;
      }
      delay(del);
}


//Initiate Flame sensor

char flame()
{
            sensorValue = analogRead(analogInPin);                    //reading sensor vaue
            outputValue = map(sensorValue, 0, 1023, 0, 255);  
            analogWrite(analogOutPin, outputValue);
            
            String s1 = "${"+String(amtsend)+","+String(reqcount)+", \"ID\":" + String(myadd16b);               //forming a Json string of data
            String s2 = ",2, "+hr+":"+minit+":"+sec+", \"Data\":["+String(sensorValue)+"]}##";
            String jsonString = s1+s2;                                //concatenating the individual strings
            int len = jsonString.length();                            //finding strig length
            char charBuff[len];
            jsonString.toCharArray(charBuff,len-1);                   //converting string to array of characters
            if (flagch==0)
            {
              sent=Send_data(dest,&charBuff[0]);                        //sending destiation address with data packet to form a API packet
              amtsend++;
            }
            delay(del);
}



//Formation of API packets with Header, Data and Checksum

char Send_data(unsigned int dest_addr,char* mypacket)
{
     unsigned char i,length;
     unsigned int checksum;
     //Serial.println("Data sending.........");
     //delay(1000);
     for(i=9;i<=100;i++) rfdata[i]=0;   //initialize total packet to zero.
     rfdata[1]=126;                     //Hex form for 7E i.e starting delimiter
     rfdata[2]=0;                       //MSB of length
     rfdata[4]=1;                       //API identifier
     rfdata[5]='M';                     //Frame ID
     rfdata[6]=dest_addr>>8;            //MSB and LSB of destnation address
     rfdata[7]=dest_addr;
     rfdata[8]=0;
     
     for(i=0;i<70;i++)
     {
          if(mypacket[i]=='#')                                    
         {
           rfdata[9+i]=mypacket[i];
           i=i+1;
           break;
         }
          rfdata[9+i]=mypacket[i];                                //Store data in API packet untill we receive '#'
     }
     length=i+5;   
     rfdata[3]=length;                                            //storing length of the packet
     checksum=0;
     for(i=4;i<(length+4);i++)  checksum=checksum+rfdata[i];      //Finding Checksum
      check=checksum;
     rfdata[length+4]=0xFF-check;                                 //Storing Checksum
     for(i=1;i<=(length+4);i++) Serial1.write(rfdata[i]);         //Sending the API packet to controller through zigbee
     //Serial.println("");
      //sending finally the TX packet
      timeout=0;
      rfsendok=0;
       while(timeout<500 && rfsendok==0) timeout++;                //Condition for Time out
       if(timeout>=500) {RFFailure++;return(0);}
       else  return(rfsendok);
     
}




// RF configuration

void Initialize_RF(void)
{
     //xbee_status_dir_input;
     //xbee_status_pullup_on;
     //xbee_reset_dir_output;
     //xbee_reset_high;
     //Delay_ms(500);
     Serial.println("**********Start xbee initialization**************");
     delay(1500);Serial1.print("+++");delay(1500);
      //Guard Time to enter into command mode
     Serial1.print("ATAP 1\r"); delay(100);
     // Configure in API mode
     Serial1.print("ATWR\r"); delay(100);
     // Write permanently in NV memory
     Serial1.print("ATCN\r"); delay(100);
     // Out of command mode
     Serial.println("**********End xbee initialization**************");
}

//Set it's own address

void Set_My_Addr(void)
{
     Serial.println("**********Start set address**************");
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
     Serial.println("**********End set address**************");
}
