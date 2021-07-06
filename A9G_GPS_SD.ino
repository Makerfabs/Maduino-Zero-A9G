#include <stdio.h>
#include <string.h>

#include <SPI.h>
#include <SD.h>

//web: http://www.makerfabs.com

File myFile;

#define SD_CS 4

void(* resetFunc) (void) = 0; //declare reset function at address 0

#define DEBUG true
int PWR_KEY = 9;
int RST_KEY = 6;
int LOW_PWR_KEY = 5;

bool ModuleState=false;
unsigned long timeCount;

void setup()
{

    SerialUSB.begin(115200);
    while (!SerialUSB)
    {
        ; // wait for serial port to connect
    }
    
    pinMode(PWR_KEY, OUTPUT);
    pinMode(RST_KEY, OUTPUT);
    pinMode(LOW_PWR_KEY, OUTPUT);
    digitalWrite(RST_KEY, LOW);
    digitalWrite(LOW_PWR_KEY, HIGH);
    digitalWrite(PWR_KEY, HIGH);


    Serial1.begin(115200);
    digitalWrite(PWR_KEY, LOW);
    delay(3000);
    digitalWrite(PWR_KEY, HIGH);
    delay(5000);
		ModuleState=moduleStateCheck();
		if(ModuleState==false)//if it's off, turn on it.
		{
			digitalWrite(PWR_KEY, LOW);
			delay(3000);
			digitalWrite(PWR_KEY, HIGH);
			delay(5000);
      SerialUSB.println("Now turnning the A9/A9G on.");
		}

    //GPS test
    sendData("AT+GPS=1", 1000, DEBUG);//1: turn on GPS  0:Turn off GPS
    sendData("AT+GPSRD=10", 1000, DEBUG);//Read NEMA information every 10 seconds 

    sendData("AT+LOCATION=2", 1000, DEBUG);//AT+LOCATION=X  //1:Get base address, 2:get gps address
        
    //sendData("AT+CCID", 3000, DEBUG);
    //sendData("AT+CREG?", 3000, DEBUG);
    //sendData("AT+CGATT=1", 1000, DEBUG);
    //sendData("AT+CGACT=1,1", 1000, DEBUG);
    //sendData("AT+CGDCONT=1,\"IP\",\"CMNET\"", 1000, DEBUG);
    
    //sendData("AT+CIPSTART=\"TCP\",\"www.mirocast.com\",80", 2000, DEBUG);

    if (!SD.begin(SD_CS)) {
        Serial.println("initialization failed!");
        while (1);
        }
    Serial.println("initialization done.");
  
}

void loop()
{
  
  if(millis()-timeCount>5000)
  {
    sendData("AT+LOCATION=2", 1000, DEBUG);
    timeCount=millis();//refresh
  }
  while (Serial1.available() > 0) {    
    String cstring = Serial1.readString();
    SerialUSB.print(cstring);//SerialUSB.write(Serial1.read());
    
    myFile = SD.open("GPS_test.txt", FILE_WRITE);

    // if the file opened okay, write to it:
    if (myFile) {
      //SerialUSB.print("Writing to GPS_test.txt...");
      myFile.seek(-1); //set position to end of file
      myFile.println(cstring);
      // close the file:
      myFile.close();
      SerialUSB.println("done.");
    } else {
      // close the file:
      myFile.close();
      // if the file didn't open, print an error:
      SerialUSB.println("error opening GPS_test.txt");
    }
    yield();
  }
  while (SerialUSB.available() > 0) {
    Serial1.write(SerialUSB.read());
    yield();
  }
}

bool moduleStateCheck()
{
    int i = 0;
    bool state=false;
    for (i = 0; i < 10; i++)
    {
        String msg = String("");
        msg = sendData("AT", 1000, DEBUG);
        if (msg.indexOf("OK") >= 0)
        {
            SerialUSB.println("A9/A9G Module had turned on.");
        		state=true;
            return state;
        }
        delay(500);
    }
  	return state;
}

String sendData(String command, const int timeout, boolean debug)
{
    String response = "";
    Serial1.println(command);
    long int time = millis();
    while ((time + timeout) > millis())
    {
        while (Serial1.available())
        {
            char c = Serial1.read();
            response += c;
        }
    }
    if (debug)
    {
        SerialUSB.print(response);
    }
    return response;
}
