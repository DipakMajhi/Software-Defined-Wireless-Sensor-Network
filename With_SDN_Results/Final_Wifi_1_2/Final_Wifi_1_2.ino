
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
char mypkt_batt[15];  			//broadcast packet array
char charBuff[9];
String conId = "\",CID: 0,\"}";

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
     inputString+= conId;
     Serial2.println(inputString);                 //print/send the data to the server through WiFly            
      //Serial2.println("sensor");
     inputString = "";                                 // clear the string:
     stringComplete = false;                           //setting flag to indicate new string could be accepted.
  }

   if(rstringComplete)                                //Indicates that we received a string/command that needs to be processed.
  {
      
      Serial.print(rinputString);
      Serial.print("  ");
      Serial.println(reqcount);
      charBuff[0]=rinputString[0];                    //storing the 0th character for further processing
      charBuff[1]=rinputString[1];                    //storing the 1st character for further processing
      charBuff[2]=rinputString[2];                    //storing the 2nd character for further processing
      charBuff[3]=rinputString[3];                    //storing the 3rd character for further processing
      charBuff[4]=rinputString[4];                    //storing the 4th character for further processing
      charBuff[5]=rinputString[5];                    //storing the 5th character for further processing
      charBuff[6]=rinputString[6];                    //storing the 6th character for further processing
      charBuff[7]=rinputString[7];                    //storing the 7th character for further processing
      
      unsigned int value1 = charBuff[2]- '0';         // to convert character to integer
      unsigned int value2 = charBuff[3]- '0';         // to convert character to integer
      nodeID = value1*10 + value2;                    //combining the two integer to get the node ID for further checking
      
      String nID=rinputString.substring(2,4); 		    //extracting the node ID
      if(nID=="00")					                          // used to check for broadcast operation
      {
        flag=1;						                              // setting flag to indicate broadcaste mode
      }
      if(flag==1)
      {
        pkt_broadcast(&mypkt_batt[0]);			              //Building the Broadcast packet
        char sent=Send_data(0xFFFF,&mypkt_batt[0]);	      //Sending the Packet in Broadcaste mode
        
      }
      if(flag==0)					//used for unicast mode
      {
        char sent=Send_data(nodeID,&charBuff[0]);          // destination is set on receiving the node ID.
      }
      flag=0;
    rinputString = "";                                    // clear the string so that further strings could be received
    rstringComplete = false;                              //setting flag to indicate new string could be accepted.
  }
 
  
}


// Function for broadcast operation

void pkt_broadcast(char* mypkt_all)
{
    unsigned char i;
    
    for(i=0;i<15;i++) mypkt_all[i]=0;
    i=0;
    mypkt_all[i]=myadd16b>>8;i++;
    mypkt_all[i]=myadd16b;i++;
    mypkt_all[i]=0xFF;i++;
    mypkt_all[i]=0xFF;i++;

    mypkt_all[i]=1;i++;  				//association not required
    mypkt_all[i]=0;i++;   				//reserved
    mypkt_all[i]=charBuff[1];i++; 			// Setting the commands in the packet
    mypkt_all[i]=charBuff[2];i++;
    mypkt_all[i]=charBuff[3];i++;
    mypkt_all[i]=charBuff[4];i++;
    mypkt_all[i]=charBuff[5];i++;
    mypkt_all[i]=charBuff[6];i++;
    mypkt_all[i]='#';i++;
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

void serialEvent2() {                                     //Function to receive a character or string as input from WiFly
  while (Serial2.available()) {                           //if any incoming data is available in Serial1
    char rinChar = (char)Serial2.read();                  // get the new byte
    
    if(rinChar!='#')
    rinputString += rinChar;                              // add it to the inputString:
    
    if (rinChar == '#') {                                 //concatinate a '#' with the string
      rinputString += '#';                                //append '#' to the end of the input string
      rstringComplete = true;                             // if the incoming character is a newline, set a flag so the main loop can do something about it:
      reqcount++;
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



//Formation of API packets with Header, Data and Checksum

char Send_data(unsigned int dest_addr,char* mypacket)
{
     unsigned int i,leng;
     unsigned int checksum;
     Serial.println("Data sending.........");
     for(i=9;i<=50;i++) rfdata[i]=0;                            //initialize total packet to zero.
     rfdata[1]=126;                                             //Hex form for 7E i.e starting delimiter
     rfdata[2]=0;                                               //MSB of length
     rfdata[4]=1;                                               //API identifier
     rfdata[5]='M';                                             //Frame ID
     rfdata[6]=dest_addr>>8;                                    //MSB and LSB of destnation address
     rfdata[7]=dest_addr;
     rfdata[8]=0;
     
     for(i=0;i<50;i++)
     {
          Serial.println(mypacket[i]);
          if(mypacket[i]=='#')  
         {
           rfdata[9+i]=mypacket[i];
           i=i+1;
           break;
         }
          rfdata[9+i]=mypacket[i];                              //Store data in API packet untill we receive '#'
     }
     leng=i+5;   
     rfdata[3]=leng;                                            //storing length of the packet
     checksum=0;
     for(i=4;i<(leng+4);i++)  checksum=checksum+rfdata[i];      //Finding Checksum
      check=checksum;
     rfdata[leng+4]=0xFF-check;                                 //Storing Checksum
     for(i=1;i<=(leng+4);i++) Serial1.write(rfdata[i]);         //Sending the API packet to controller through zigbee
     //Serial.println("");
      //sending finally the TX packet
      timeout=0;
      rfsendok=0;
       while(timeout<500 && rfsendok==0) timeout++;             //Condition for Time out
       if(timeout>=500) {RFFailure++;return(0);}
       else  return(rfsendok);
     
}



