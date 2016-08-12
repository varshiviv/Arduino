/*
Copyright (c) 2015, Intel Corporation

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

//This example reads the temperature sensor on the Galileo's
//onboard ADC, AD7298, using the iio (Industrial I/O) subsystem and 
//sends the observed data to the Intel IoTkit Cloud

//Prework:
//You need iotkit-agent installed and running.
//Device should be activated. (by running: iotkit-admin activate ACTIVATION_CODE)
//Following components should be registered (by running: iotkit-admin register NAME TYPE):
//temperature of type temperature.v1.0
//You can also use IoTkitRegisterExample to activate and register new components

#include <IoTkit.h>    // include IoTkit.h to use the Intel IoT Kit
#include <Ethernet.h>  // must be included to use IoTkit
#include <aJSON.h>
#include <Wire.h>
#include <math.h>
#include "rgb_lcd.h"

rgb_lcd lcd;

const int colorR = 255;
const int colorG = 192;
const int colorB = 203;
// create an object of the IoTkit class
IoTkit iotkit;        
float temp;

/*
 "therm_file" - SoC temperature sensor:
 On Galileo Gen2 - use thermal_zone0
 On Edison       - use thermal_zone1 (ambient temp)
                       thermal_zone3 (core0)
                       thermal_zone4 (core1)
*/
char* therm_file = "/sys/devices/virtual/thermal/thermal_zone1/temp";


void setup() {
  //Serial.begin(115200);
  // set up the LCD's number of columns and rows:
    lcd.begin(16, 2);
    lcd.setRGB(colorR, colorG, colorB);
    
    // Print a message to the LCD.
    lcd.print("Temperature");
  // call begin on the IoTkit object before calling any other methods
  iotkit.begin();
}


float temperature;
float socTemp;
int B=3975;                  //B value of the thermistor
float resistance;

void loop() {
 
  //Serial.println("Reading temperature");
  temp = getTemp(); 
  //Serial.print("Temperature is ");
  //Serial.print(temp);
  //Serial.println(" degrees celcius.");
  
  // call send to generate one observation.
  // parm1 - the name of the measurement. It must have been previously registered.
  // parm2 - the value to send as the measurement observation
  // you can also generate your own JSON and send multiple keys and values
  // in this format:
  //
  // {
  //   “n”: “temperature”,
  //   “v”: “27.2"
  // }
  //
  // you need to escape the quotations to pass it directly to iotkit.send:
  // iotkit.send("{\"n\": \"temperature\",\"v\":\"27.2\"}");
  //
  // you can also send a full JSON string with your own variables:
  //
  // aJsonObject* root = aJson.createObject();
  // if (root != NULL) {
  //    aJson.addItemToObject(root, "n", aJson.createItem(metric));
  //    aJson.addItemToObject(root, "v", aJson.createItem(value));
  //    iotkit.send(aJson.print(root)); // this sends your full json
  //    aJson.deleteItem(root);
  // }
  //
  //to display temp on LCD
  lcd.setCursor(0, 1);
  lcd.print(temp);
  iotkit.send("temperature", temp);
  delay(5000);
}

// Generic Edison/Galileo function to read hardware temp sensor
float getTemp()
{
    int a=analogRead(0);
    resistance=(float)(1023-a)*10000/a; //get the resistance of the sensor;
    socTemp=1/(log(resistance/10000)/B+1/298.15)-273.15;//convert to temperature via datasheet ;
    //to display temp on LCD
    //lcd.setCursor(0, 1);
    //lcd.print(socTemp);

    //delay(5000);


  //if(successful) {
    //socTemp = atoi(rawTemp)/1000;
    return socTemp;
  //}
  //return 0;
}
