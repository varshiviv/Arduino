
/*
Intel Edison Location and movement logger by Chip McClelland
BSD license, Please keep my name and the required Adafruit text in any redistribution


Credits: 

  - The Adarfruit libraries were modified for Edison here: https://github.com/enableiot/iotkit-samples/tree/master/arduino/gps

Requirements:
  - Follow the setup instructions here: https://communities.intel.com/docs/DOC-23148
  - Get a free account on Ubidots.  http://www.ubidots.com
  - Adafruit Ultimate GPS: https://www.adafruit.com/product/746
  - I used a 3.3V 8MHz Arduino Pro Mini from Sparkfun - https://www.sparkfun.com/products/11114
  - I also used the Sparkfun MMA8452 Accelerometer - https://www.sparkfun.com/products/12756

*/
// Includes, Prototypes and instantiations
#include <WiFi.h>
#include<stdlib.h>
#include <Adafruit_GPS.h>      // Using s modified version of the Adafruit libraries
Adafruit_GPS GPS(&Serial1);    // Can't use SoftwareSerial

// WiFi Constants
char ap_ssid[] = "xxxxxxx";          // SSID of network
char ap_password[] = "xxxxxxx";               // Password of network
unsigned int TimeOut = 10000;                  // Milliseconds

// Ubidots Information
String token = "xxxxxxxxx";  // For your account
String idvariable = "xxxxxxx";  // For your variable

// Initialize the client library
WiFiClient client;

// Global Variables
int value = 0;
unsigned int ReportingInterval = 20000;  // How often do you want to send to Ubidots (in millis)
unsigned long LastReport = 0;            // Keep track of when we last sent data
char c;                                  // Used to relay input from the GPS serial feed
String Location = "";                    // Will build the Location string here

// Setup code - runs once
void setup() {
  Serial.begin(19200);       // This is the Serial port that communicates with the Arduino IDR Serial monitor
  GPS.begin(9600);           // Serial1 where we communicate with the Adafruit GPS on Arduino pins 0 and 1 - default baud rate
  Serial.println("Connected Location Logger");
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA); // RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);    // 1 Hz update rate - Adafruit does not suggest using anything higher than 1 Hz
  //GPS.sendCommand(PGCMD_ANTENNA);            // Request updates on antenna status, comment out to keep quiet
  if (WiFi.begin(ap_ssid,ap_password) != WL_CONNECTED) { // if you are connected, print out info about the connection:
    Serial.println("Couldn't get a wifi connection");
    while(true);
  } 
  Serial.print("Connected to Wifi at IP Address:");
  Serial.println(WiFi.localIP()); 
  GetConnected();  // Leave this here if you want the Edison to stay connected (for testing)
}

// Main loop runs continuosly
void loop() 
{
  
  if (millis() >= LastReport + ReportingInterval) {
    //GetConnected();  // Uncomment if you plan to have the Edison disconnect between reports
    Serial.print("Sending to Ubidots:");
    Serial.println(GPS.lastNMEA());   // Use this line for debugging to see raw feed
    Serial.print("Altitude: "); Serial.println(GPS.altitude);
    value = GPS.altitude;    // Altitude as starting point
    if(Send2Ubidots(String(value))) {
      Serial.println("Data successfully sent to Ubidots");
      LastReport = millis();  // Reset the timer
    }
    else {
      Serial.println("Data not accepted by Ubidots - try again");
    }
    //client.stop();  // Uncomment if you plan to have the Edison disconnect between reports
  }
  char c = GPS.read();        // Receive output from the GPS one character at a time
  if (GPS.newNMEAreceived()) {  // if a sentence is received, we can check the checksum, parse it...
    Serial.println(GPS.lastNMEA());   // Use this line for debugging to see raw feed
    if (!GPS.parse(GPS.lastNMEA()))   // check to ensure we succeeded with parsing
      return;                        // If we fail to parse a sentence we just wait for another
  }
}


// Here is where we send the information to Ubidots
boolean Send2Ubidots(String value)
{
  char replybuffer[64];          // this is a large buffer for replies
  int count = 0;
  int complete = 0;
  String var = "";
  String le = "";
  ParseLocation();              // Update the location value from the GPS feed
  var="{\"value\":"+ value + ", \"context\":"+ Location + "}";
  int num=var.length();                                       // How long is the payload
  le=String(num);                                             //this is to calcule the length of var
  // Make a TCP connection to remote host
  Serial.println("Sending Data to Ubidots");
  if (!client.connect("things.ubidots.com", 80)) {
    Serial.println("Error: Could not make a TCP connection");
  }   
  // Make a HTTP GET request
  client.print("POST /api/v1.6/variables/");
  client.print(idvariable);
  client.println("/values HTTP/1.1");
  client.println("Content-Type: application/json");
  client.print("Content-Length: ");
  client.println(le);
  client.print("X-Auth-Token: ");
  client.println(token);
  client.println("Host: things.ubidots.com");
  client.println();
  client.println(var);
  client.println();
  // See if Ubidots acknowledges the creation of a new "dot" with a "201" code
  unsigned long commandClock = millis();                      // Start the timeout clock
  while(!complete && millis() <= commandClock + TimeOut)         // Need to give the modem time to complete command 
  {
    while(!client.available() &&  millis() <= commandClock + TimeOut);  // Keep checking to see if we have data coming in
    while (client.available()) {
      replybuffer[count] = client.read();
      count++;
      if(count==63) break;
    }
    //Serial.print("count=");  // Uncomment if needed to debug
    //Serial.print(count);
    Serial.print(" - Reply: ");
    for (int i=0; i < count; i++) {
      if (replybuffer[i] != '\n') Serial.write(replybuffer[i]);
    }
    Serial.println("");                           // Uncomment if needed to debug
    for (int i=0; i < count; i++) {
      if(replybuffer[i]=='2' && replybuffer[i+1]=='0' && replybuffer[i+2] == '1') {  // here is where we parse "201"
        complete = 1;
        break;
      }
    }
  }
  if (complete ==1) return 1;            // Returns "True"  if we get the 201 response
  else return 0;
}


// Connection to Wifi 
void GetConnected()
{
  if (WiFi.begin(ap_ssid,ap_password) != WL_CONNECTED) { // if you are connected, print out info about the connection:
    Serial.println("Couldn't get a wifi connection");
    while(true);
  } 
  Serial.print("Connected to Wifi at IP Address:");
  Serial.println(WiFi.localIP()); 
}

boolean ParseLocation() 
// Refer to http://www.gpsinformation.org/dale/nmea.htm#GGA
// Sample data: $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47
{
  char Latitude[10];
  char Longitude[10];
  float Lat = GPS.latitude;
  float Lon = GPS.longitude;
  float Latcombo = (int)Lat/100 + (((Lat/100 - (int)Lat/100)*100)/60);
  float Loncombo = (int)Lon/100 + (((Lon/100 - (int)Lon/100)*100)/60);
  sprintf(Latitude, "%'.3f", Latcombo);
  sprintf(Longitude, "%'.3f", Loncombo);
  Location = "{\"lat\":" + String(Latitude) + ",\"lng\":-" + String(Longitude) + "}";
  Serial.println(Location);
}

