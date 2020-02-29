#include<stdio.h>
#include<string.h>

//Description: A9 or A9G SMS test: send message to control the board to turn on/off R/G/B LED.
//version:v1.0
//Date:2020/02/28
//
//Modified by:Charlin
//WEB: http://www.makerfabs.com
//SIM Card must support SMS Service



#define DEBUG true

int PWR_KEY=9;
int RST_KEY=6;
int LOW_PWR_KEY=5;

int LED_R	= 2;  //D2	RED
int LED_G = 3;	//D3	GREEN
int LED_B	=	4;	//D4	BLUE

String msg = String("");
int SmsContentFlag = 0;

void setup()
{

	  pinMode(PWR_KEY, OUTPUT);
	  pinMode(RST_KEY, OUTPUT);
	  pinMode(LOW_PWR_KEY, OUTPUT);
	  digitalWrite(RST_KEY, LOW);
	  digitalWrite(LOW_PWR_KEY, HIGH);   
	  digitalWrite(PWR_KEY, HIGH);

	  pinMode(LED_R, OUTPUT); 
	  pinMode(LED_G, OUTPUT);
	  pinMode(LED_B, OUTPUT);
	  digitalWrite(LED_R, HIGH);
	  digitalWrite(LED_G, HIGH);
	  digitalWrite(LED_B, HIGH);
	  
//		String msg = String("");
//	  int SmsContentFlag = 0;

	  SerialUSB.begin(115200);
		while (!SerialUSB) {
	    ; // wait for Arduino serial port to connect
	  }
	  Serial1.begin(115200);
		modulePowerOn();
	  GprsTextModeSMS();
}

void loop()
{
		char SerialInByte;
    if(SerialUSB.available())
    {
        Serial1.print((unsigned char) SerialUSB.read());
    }  
    else if(Serial1.available())
    {
        //char SerialInByte;
        SerialInByte = (unsigned char)Serial1.read();
        if( SerialInByte == 13 )//0x0D
        {
          ProcessGprsMsg();
				}
				if( SerialInByte == 10 )
				{
        }
				else
				{
           //EN: store the current character in the message string buffer
           msg += String(SerialInByte);
        }
     }
}
 
void modulePowerOn()
{
		int i=0;
		boolean result = false;
		digitalWrite(PWR_KEY, LOW);    
    delay(3000); 
    digitalWrite(PWR_KEY, HIGH);
    delay(15000); 

		result =  Serial1.find("OK");
		if(result)
			SerialUSB.println( "Please send sms to control your device!" );
    else
		{
				for(i=0;i<10;i++)
				{
						Serial1.println("AT");
						delay(500);
						result =  Serial1.find("OK");
						if(result)
						{
								SerialUSB.println( "Please send sms to control your device!" );
								return;
						}
         }
		}
}

void ProcessSms( String sms )
{
		SerialUSB.print( "ProcessSms for [" );
		SerialUSB.print( sms );
		SerialUSB.println( "]" ); 
		if( sms.indexOf("onr") >= 0)
		{
    		digitalWrite(LED_R, LOW);
				SerialUSB.println( "LED Red ON" );
				//return;
		}
		if( sms.indexOf("ong") >= 0)
		{
				digitalWrite( LED_G, LOW);
				SerialUSB.println( "LED Green ON");
				//return;
		}
    if( sms.indexOf("onb") >= 0 )
    {
    		digitalWrite(LED_B, LOW);
     		SerialUSB.println( "LED Blue ON" );
				//return;
		}
    if( sms.indexOf("offr") >= 0 )
    {
				digitalWrite(LED_R,HIGH);
				SerialUSB.println( "LED Red OFF" );
				//return;
		}
		if(sms.indexOf("offg") >= 0 )
		{
				digitalWrite(LED_G, HIGH );
				SerialUSB.println( "LED Green OFF" );
				//return;
		}
    if( sms.indexOf("offb") >= 0 )
		{
				digitalWrite(LED_B, HIGH );
				SerialUSB.println( "LED Blue OFF" );
				//return;
		}
}

// EN: Request Text Mode for SMS messaging
void GprsTextModeSMS()
{
		Serial1.println( "AT+CMGF=1" );
}

void GprsReadSmsStore( String SmsStorePos )
{
		// Serial.print( "GprsReadSmsStore for storePos " );
		// Serial.println( SmsStorePos ); 
		Serial1.print( "AT+CMGR=" );
		Serial1.println( SmsStorePos );
}
 
// EN: Clear the GPRS shield message buffer
void ClearGprsMsg()
{
		msg = "";
}
 
// EN: interpret the GPRS shield message and act appropiately
void ProcessGprsMsg() 
{
		SerialUSB.println("");
		//  Serial.print( "GPRS Message: [" );
		SerialUSB.print( msg );
		//    Serial.println( "]" );
		if( msg.indexOf( "Call Ready" ) >= 0 )
		{
				SerialUSB.println( "*** GPRS Shield registered on Mobile Network ***" );
				GprsTextModeSMS();
		}
 
		//EN: unsolicited message received when getting a SMS message
		if( msg.indexOf( "+CIEV" ) >= 0 )
		{
				SerialUSB.println( "*** SMS Received ***" );
		}
 
		//EN: SMS store readed through UART (result of GprsReadSmsStore request)  
		if( msg.indexOf( "+CMT:" ) >= 0 )
		{
				// EN: Next message will contains the BODY of SMS
				SmsContentFlag = 1;
				// EN: Following lines are essentiel to not clear the flag!
				ClearGprsMsg();
				return;
		}
 
		// EN: +CMGR message just before indicate that the following GRPS Shield message 
		//     (this message) will contains the SMS body
		if( SmsContentFlag == 1 )
		{
				SerialUSB.println( "*** SMS MESSAGE CONTENT ***" );
				SerialUSB.println( msg );
				SerialUSB.println( "*** END OF SMS MESSAGE ***" );
				ProcessSms( msg );
		}
 
		ClearGprsMsg();
		//EN: Always clear the flag
		SmsContentFlag = 0; 
}
