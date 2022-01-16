#ifndef DISPLAY_H
#define DISPLAY_H

#include <Wire.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


class Display
{
    public:
        Display();
        void display_clear(int lane = -1);
        void printTest();
        void print(String toPrint, int lane = 0, int textSize = 1, bool clear = false);
        void println(String toPrint, int lane = 0, int textSize = 1, bool clear = false);

    private:
        int getLane();
};

#endif