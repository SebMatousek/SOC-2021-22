#include <Arduino.h>
#include "display.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET  -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int atLane = 1;

Display::Display()
{
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;); // Don't proceed, loop forever
    }

    display.setTextColor(SSD1306_WHITE);

    // Clear the buffer
    display.clearDisplay();
    delay(1000);
}

void Display::display_clear(int lane)
{
    if(lane < 0)
    {
        display.clearDisplay();
        display.display();
        
        atLane = 1;
    }
    else
    {
        print("                       ", lane, 1);
        atLane -= lane;
    }
}

void Display::printTest()
{
    display.setTextSize(2);
    display.setCursor(10, 10);
    display.println("Hello,");
    atLane++;
    display.println("  World!");
    atLane++;

    display.display();
}

void Display::print(String toPrint, int lane, int textSize, bool clear)
{
    if(clear)//if supposed to clear display, do so
        display.clearDisplay();

    //set text size
    display.setTextSize(textSize);

    //if no custom lane, print it on next available lane
    if(lane <= 0)
    {
        display.setCursor(5, getLane());
    }//else print on custom lane
    else
        display.setCursor(5, lane * 10);
    
    //print out the input
    display.print(toPrint);

    // count up all the \n there are and adjust lane
    while(true)
    {
        if(toPrint.indexOf("\n") > -1)
        {
            atLane++;
            toPrint = toPrint.substring(toPrint.indexOf("\n") + 1, toPrint.length());
        }
        else
            break;
    }

    //send it to display
    display.display();
}

void Display::println(String toPrint, int lane, int textSize, bool clear)
{
    if(clear)//if supposed to clear display, do so
        display.clearDisplay();

    //set text size
    display.setTextSize(textSize);

    //if no custom lane, print it on next available lane
    if(lane <= 0)
    {
        display.setCursor(5, getLane() * 10);
    }//else print on custom lane
    else
        display.setCursor(5, lane * 10);
    
    //print out the input
    display.print(toPrint + "\n");
    atLane++;//there is automaticly a new lane because println

    // count up all the \n there are and adjust lane
    while(true)
    {
        if(toPrint.indexOf("\n") > -1)
        {
            atLane++;
            toPrint = toPrint.substring(toPrint.indexOf("\n") + 1, toPrint.length());
        }
        else
            break;
    }

    //send it to display
    display.display();
}

int Display::getLane()
{
    return atLane;
}