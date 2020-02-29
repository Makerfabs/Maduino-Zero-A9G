#include <stdio.h>
#include <string.h>

//Description: GPS Test for A9G only, that is get the location.
//version:v1.0
//Date:2019/11/23
//Author:Charlin
//web: http://www.makerfabs.com
//


#define DEBUG true
int PWR_KEY = 9;
int RST_KEY = 6;
int LOW_PWR_KEY = 5;

bool ModuleState=false;
unsigned long timeCount;

void setup()
{
    pinMode(PWR_KEY, OUTPUT);
    pinMode(RST_KEY, OUTPUT);
    pinMode(LOW_PWR_KEY, OUTPUT);
    digitalWrite(RST_KEY, LOW);
    digitalWrite(LOW_PWR_KEY, HIGH);
    digitalWrite(PWR_KEY, HIGH);

    SerialUSB.begin(115200);
    while (!SerialUSB)
    {
        ; // wait for serial port to connect
    }
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
    timeCount=millis();
    SerialUSB.println("Maduino A9/A9G GPS Test Begin!");
}

void loop()
{
  
  if(millis()-timeCount>5000)
  {
    sendData("AT+LOCATION=2", 1000, DEBUG);
    timeCount=millis();//refresh
  }
  while (Serial1.available() > 0) {
    SerialUSB.write(Serial1.read());
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
