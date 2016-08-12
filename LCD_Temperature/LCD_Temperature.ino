/*
  Hello World.ino
  2013 Copyright (c) Seeed Technology Inc.  All right reserved.

  Author:Loovee
  2013-9-18

  Grove - Serial LCD RGB Backlight demo.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <Wire.h>
#include <math.h>
#include "rgb_lcd.h"

rgb_lcd lcd;

const int colorR = 0;
const int colorG = 192;
const int colorB = 203;

void setup() 
{
    // set up the LCD's number of columns and rows:
    lcd.begin(16, 2);
    
    lcd.setRGB(colorR, colorG, colorB);
    
    // Print a message to the LCD.
    lcd.print("Temperature");

    //delay(10);
}

//int a;
float temperature;
int B=3975;                  //B value of the thermistor
float resistance;

void loop() 
{
    // set the cursor to column 0, line 1
    // (note: line 1 is the second row, since counting begins with 0):
    int a=analogRead(1);
    resistance=(float)(1023-a)*10000/a; //get the resistance of the sensor;
    temperature=1/(log(resistance/10000)/B+1/298.15)-273.15;//convert to temperature via datasheet ;
    lcd.setCursor(0, 1);
    // print the number of seconds since reset:
    lcd.print(temperature);

    delay(5000);
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
