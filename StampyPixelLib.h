#ifndef _StampyPixelLib_h
#define _StampyPixelLib_h

#include "Adafruit_NeoPixel.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include <Adafruit_9DOF.h>
#include <Adafruit_L3GD20_U.h>
#include <Kalman.h>

class StampyStrip {
public:
    StampyStrip(uint16_t pix, uint16_t ledPin, bool useInput, uint16_t pixCenter, uint16_t loopInterval, int intervalOffset);
    void loop();
    void rainbowWipeUp(uint8_t wait);
    void rainbowWipeDown(uint8_t wait);
    void colorWipeUp(uint32_t c, uint8_t wait);
    void colorWipeDown(uint32_t c, uint8_t wait);
    uint32_t getColor(uint8_t r, uint8_t g, uint8_t b);
    void begin();
private:
    uint16_t _loopInterval;
    //static const uint8_t _displaySize = 4;
    
    static const uint8_t _bgMinBrightness = 0;
    static const uint8_t _bgMaxBrightness = 255;
    static const uint8_t _sampleBufferSize = 5;
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
    
    uint32_t _bgColor;
    
    bool _useInput;
    Adafruit_9DOF                _dof;
    Adafruit_LSM303_Accel_Unified _accel;
    Adafruit_L3GD20_Unified _gyro;
    Kalman _kalmanX; // Create the Kalman instances
    Kalman _kalmanY;
    uint32_t _timer;
    sensors_event_t _accel_event;
    sensors_event_t _gyro_event;
    sensors_vec_t   _orientation;
    float _currentRoll;
    float _maxRoll;
    float _minRoll;
    float _brightness;
    double _gyroXrate;
    double _dt;
    void printRanges();
    float fmap(float x, float in_min, float in_max, float out_min, float out_max);
    double dmap(double x, double in_min, double in_max, double out_min, double out_max);

    float getRadians(float index, int interval);
    uint8_t sinMap(float rads);
    uint32_t getLoopedColor(int index, int interval);
    void showPosition();
    uint32_t getLightColor(int lightIndex);
    float getBrightness(int index);
    uint8_t getScaled(uint8_t num, float by);
    void show();
};


#endif
