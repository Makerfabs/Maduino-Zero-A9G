#include <stdio.h>
#include <string.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <DHT.h>
#include <DHT_U.h>

//*******for senser **********
float humidity;//humidity
float temperature;//temperature

#define DHTPIN 3    //D3
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE);

String showString="";//

//Description: GPS Test for A9 or A9G, show the temperature and humidity on OLED.
//version:v1.0
//Date:2019/11/25
//Author:Charlin
//web: http://www.makerfabs.com
//

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void(* resetFunc) (void) = 0; //declare reset function at address 0

#define DEBUG true
int PWR_KEY = 9;
int RST_KEY = 6;
int LOW_PWR_KEY = 5;

bool ModuleState=false;
unsigned long timeCount;

void setup()
{
    dht.begin();//sensor begin
  
    pinMode(PWR_KEY, OUTPUT);
    pinMode(RST_KEY, OUTPUT);
    pinMode(LOW_PWR_KEY, OUTPUT);
    digitalWrite(RST_KEY, LOW);
    digitalWrite(LOW_PWR_KEY, HIGH);
    digitalWrite(PWR_KEY, HIGH);

    SerialUSB.begin(115200);
    //while (!SerialUSB)
    {
        ; // wait for serial port to connect
    }
    Serial1.begin(115200);
    SerialUSB.println("Start to run!");
    /**
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
   **/

    //GPS test
    //sendData("AT+GPS=1", 1000, DEBUG);//1: turn on GPS  0:Turn off GPS
    //sendData("AT+GPSRD=10", 1000, DEBUG);//Read NEMA information every 10 seconds 

    //sendData("AT+LOCATION=2", 1000, DEBUG);//AT+LOCATION=X  //1:Get base address, 2:get gps address
  

     
    //sendData("AT+CCID", 3000, DEBUG);
    //sendData("AT+CREG?", 3000, DEBUG);
    //sendData("AT+CGATT=1", 1000, DEBUG);
    //sendData("AT+CGACT=1,1", 1000, DEBUG);
    //sendData("AT+CGDCONT=1,\"IP\",\"CMNET\"", 1000, DEBUG);
    
    //sendData("AT+CIPSTART=\"TCP\",\"www.mirocast.com\",80", 2000, DEBUG);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
    {
        SerialUSB.println("Now reset the maduino zero");
        delay(1000);
        resetFunc();//restart 
       delay(20);
    }
  }
  delay(500);
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  //Display static text
  display.println("www.makerfabs.com!");
  display.println("Maduino Zero A9/A9G GPS Tracker!");
  display.display();
  delay(2000);
  
  timeCount=millis();
  SerialUSB.println("Maduino A9/A9G GPS Test Begin!");
  display.clearDisplay();

  
    
}

void loop()
{ 
  if(millis()-timeCount>1000)
  {
    
    humidity = dht.readHumidity();
                         
    temperature = dht.readTemperature();
    
    SerialUSB.println("humidity is " + (String)humidity);
    SerialUSB.println("temperature is " + (String)temperature);

  //if (!isnan(humidity) && !isnan(temperature)) 
  {
    //SerialUSB.println("Read from DHT sensor!");
    showString = "Humidity:" + (String)humidity + "%" + "\r\n" +"Temperature:"+(String)temperature + "C"; 
  }
      
    SerialUSB.println(showString);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    
    display.println(showString);
    display.display();
    timeCount=millis();//refresh
  }
  while (Serial1.available() > 0) {
    display.clearDisplay();
    String cstring = Serial1.readString();
    SerialUSB.print(cstring);//SerialUSB.write(Serial1.read());
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);

    display.println(cstring);
    display.display();
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
    bool moduleState=false;
    for (i = 0; i < 10; i++)
    {
        String msg = String("");
        msg = sendData("AT", 1000, DEBUG);
        if (msg.indexOf("OK") >= 0)
        {
            SerialUSB.println("A9/A9G Module had turned on.");
        		moduleState=true;
            return moduleState;
        }
        delay(500);
    }
  	return moduleState;
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
