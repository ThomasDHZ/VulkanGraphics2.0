#pragma once
typedef unsigned char byte;
struct Pixel
{
    byte Red = 0x00;
    byte Green = 0x00;
    byte Blue = 0x00;
    byte Alpha = 0xFF;

    Pixel()
    {

    }

    Pixel(byte Byte)
    {
        Red = Byte;
        Green = Byte;
        Blue = Byte;
    }
    Pixel(byte RedGreen, byte blue)
    {
        Red = RedGreen;
        Green = RedGreen;
        Blue = blue;
    }

    Pixel(byte red, byte green, byte blue)
    {
        Red = red;
        Green = green;
        Blue = blue;
    }

    Pixel(byte red, byte green, byte blue, byte alpha)
    {
        Red = red;
        Green = green;
        Blue = blue;
        Alpha = alpha;
    }

    ~Pixel()
    {

    }
};