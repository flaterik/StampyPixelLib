#ifndef _StampyPixelLib_h
#define _StampyPixelLib_h

#include "Adafruit_NeoPixel.h"

class StampyStrip {
public:
    StampyStrip(uint16_t pix, uint16_t ledPin, uint16_t inputPin, uint16_t pixCenter);
    void loop();
    void rainbowWipeUp(uint8_t wait);
    void rainbowWipeDown(uint8_t wait);
    void colorWipeUp(uint32_t c, uint8_t wait);
    void colorWipeDown(uint32_t c, uint8_t wait);
    uint32_t getColor(uint8_t r, uint8_t g, uint8_t b);
    void begin();
private:
    static const uint16_t _loopInterval = 1000;
    static const uint8_t _displaySize = 4;
    
    static const uint8_t _bgMinBrightness = 0;
    static const uint8_t _bgMaxBrightness = 200;
    
    static const uint8_t _sampleBufferSize = 50;
    static const int _readLimitHigh = 1023;
    static const int _readLimitLow = 0;
    
    Adafruit_NeoPixel* _strip;
    
    uint16_t _ledPin;
    uint16_t _pix;
    uint16_t _pixCenter;
    
    uint16_t _loopCount;
    
    uint16_t _inputPin;
    uint16_t _readMax;
    uint16_t _readMin;
    
    uint8_t _lastIndex;
    
    int16_t _maxSpeed;
    int16_t _lastSpeed;
    
    uint16_t _sampleBuffer[_sampleBufferSize];
    uint8_t _sampleIndex;
    
    uint32_t _bgColor;
    
    float fmap(float x, float in_min, float in_max, float out_min, float out_max);
    float getRadians(float index, int interval);
    uint8_t sinMap(float rads);
    uint32_t getLoopedColor(int index, int interval);
    void showPosition(int index, int16_t currentSpeed);
    uint32_t getLightColor(int lightIndex);
    
};


#endif
