#ifndef _StampyPixelLib_h
#define _StampyPixelLib_h

#include "Adafruit_NeoPixel.h"

class StampyStrip {
public:
    StampyStrip(uint16_t pix, uint16_t ledPin, uint16_t inputPin1, uint16_t inputPin2, uint16_t pixCenter);
    void loop();
    void rainbowWipeUp(uint8_t wait);
    void rainbowWipeDown(uint8_t wait);
    void colorWipeUp(uint32_t c, uint8_t wait);
    void colorWipeDown(uint32_t c, uint8_t wait);
    uint32_t getColor(uint8_t r, uint8_t g, uint8_t b);
    void begin();
private:
    static const uint16_t _loopInterval = 1000;
    //static const uint8_t _displaySize = 4;
    
    static const uint8_t _bgMinBrightness = 0;
    static const uint8_t _bgMaxBrightness = 200;
    
    static const uint8_t _sampleBufferSize = 50;
//    static const int _readLimitHigh1 = 512; //accelerometers are 5v nominal, getting 3.3 without a reference, need to scale to account
//    static const int _readLimitLow1 = 325;
//    
//    static const int _readLimitHigh2 = 512;
//    static const int _readLimitLow2 = 325;
    
    Adafruit_NeoPixel* _strip;
    
    uint16_t _ledPin;
    uint16_t _pix;
    uint16_t _pixCenter;
    
    uint16_t _loopCount;
    
    uint16_t _inputPin1;
    uint32_t _inputPin2;
    uint16_t _readMax1;
    uint16_t _readMin1;
    uint16_t _readMax2;
    uint16_t _readMin2;
    uint8_t _lastIndex;
    
    double _maxAngle;
    double _lastAngle;
    
    uint16_t _sampleBuffer1[_sampleBufferSize];
    uint16_t _sampleBuffer2[_sampleBufferSize];
    uint8_t _sampleIndex;
    
    uint32_t _bgColor;
    void printRanges();
    float fmap(float x, float in_min, float in_max, float out_min, float out_max);
    float getRadians(float index, int interval);
    uint8_t sinMap(float rads);
    uint32_t getLoopedColor(int index, int interval);
    double getAngle(uint32_t input1, uint32_t input2);
    //void showPosition(int index, int16_t currentSpeed);
    uint32_t getLightColor(int lightIndex);
    
};


#endif
