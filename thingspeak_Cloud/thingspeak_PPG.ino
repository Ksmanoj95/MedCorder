/*
  WiFi Web Server

 A simple web server that a repeated counter

 Change the macro WIFI_AP, WIFI_PASSWORD and WIFI_AUTH accordingly.

 created 13 July 2010
 by dlf (Metodo2 srl)
 modified 31 May 2012
 by Tom Igoe
 modified 20 Aug 2014
 by MediaTek Inc.

Thingspeak code

Created: October 17, 2011 by Hans Scharler (http://www.nothans.com)


 */
#include <LTask.h>
#include <LWiFi.h>
#include <LWiFiServer.h>

#include <LWiFiClient.h>

#define WIFI_AP "Indix-Event-2"
#define WIFI_PASSWORD "guest@123"
#define WIFI_AUTH LWIFI_WPA  // choose from LWIFI_OPEN, LWIFI_WPA, or LWIFI_WEP according to your WiFi AP configuration
int BPM;
char str[3];

// ThingSpeak Settings
char thingSpeakAddress[] = "api.thingspeak.com";
String writeAPIKey = "LIJA7LV497BRVDXL";
const int updateThingSpeakInterval = 3 * 1000;      // Time interval in milliseconds to update ThingSpeak (number of seconds * 1000 = interval)

long lastConnectionTime = 0; 
boolean lastConnected = false;
int failedCounter = 0;

LWiFiClient client; 

void setup()
{
  LWiFi.begin();
  Serial.begin(115200);
  Serial1.begin(9600);

  // keep retrying until connected to AP
  Serial.println("Connecting to AP");
  while (0 == LWiFi.connect(WIFI_AP, LWiFiLoginInfo(WIFI_AUTH, WIFI_PASSWORD)))
  {
   // delay(1000);
  }

  startWifi();

 
}

void loop()
{
  int i=0;
  if (Serial1.available()) {
    delay(100); //allows all serial sent to be received together
    while(Serial1.available() && i<3) {
      str[i++] = Serial1.read();
    }
    str[i++]='\0';
  }

  // Read value from Analog Input Pin 0
  String analogValue0 = String(analogRead(A0), DEC);
 
  // Print Update Response to Serial Monitor
  if (client.available())
  {
    char c = client.read();
    Serial.print(c);
  }

  // Disconnect from ThingSpeak
  if (!client.connected() && lastConnected)
  {
    Serial.println("...disconnected");
    Serial.println();
    
    client.stop();
  }
  
  // Update ThingSpeak
  if(!client.connected() && (millis() - lastConnectionTime > updateThingSpeakInterval))
  {
    updateThingSpeak("field1="+analogValue0+"&field2="+str);
  }
  
  // Check if Arduino Ethernet needs to be restarted
  if (failedCounter > 3 ) {startWifi();}
  
  lastConnected = client.connected();
}

void updateThingSpeak(String tsData)
{
  if (client.connect(thingSpeakAddress, 80))
  {         
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: "+writeAPIKey+"\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(tsData.length());
    client.print("\n\n");

    client.print(tsData);
    
    lastConnectionTime = millis();
    
    if (client.connected())
    {
      Serial.println("Connecting to ThingSpeak...");
      Serial.println();
      
      failedCounter = 0;
    }
    else
    {
      failedCounter++;
  
      Serial.println("Connection to ThingSpeak failed ("+String(failedCounter, DEC)+")");   
      Serial.println();
    }
    
  }
  else
  {
    failedCounter++;
    
    Serial.println("Connection to ThingSpeak Failed ("+String(failedCounter, DEC)+")");   
    Serial.println();
    
    lastConnectionTime = millis(); 
  }
}

void startWifi()
{
  
  client.stop();

  Serial.println("Connecting Arduino to network...");
  Serial.println();  

  //delay(1000);
  

  
}
