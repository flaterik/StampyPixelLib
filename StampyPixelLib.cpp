//
//  StampyPixelLib.cpp
//
//
//  Created by Erik Nelson on 8/16/13.
//
//

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include <Adafruit_9DOF.h>
#include <Kalman.h>
#include "StampyPixelLib.h"
#include "math.h"

//#include <Adafruit_Sensor.h>
//#include <Adafruit_LSM303_U.h>
//#include <Adafruit_9DOF.h>

StampyStrip::StampyStrip(uint16_t pix, uint16_t ledPin, bool useInput, uint16_t pixCenter, uint16_t loopInterval, int intervalOffset) {
    _strip = new Adafruit_NeoPixel(pix, ledPin, NEO_GRB + NEO_KHZ800);
    _ledPin = ledPin;
    _pix = pix;
    _pixCenter = pixCenter;
    _loopCount = intervalOffset;
    _useInput = useInput;
    _currentRoll = 0;
    _maxRoll = -90;
    _minRoll = 90;
    _loopInterval = loopInterval;
    _brightness = 1.0;
}

void StampyStrip::begin() {
    if(_useInput) {
        Serial.println("trying to start sensors");
        _dof = Adafruit_9DOF();
        _accel = Adafruit_LSM303_Accel_Unified(30301);
        _gyro = Adafruit_L3GD20_Unified(30301);
        if(_accel.begin() && _gyro.begin()) {
            _useInput = true;
            Serial.println("using input");
        }
        else {
            _useInput = false;
            Serial.println("accel or gyro didn't start");
        }
    }
    else {
        Serial.println("not using input");
    }
    _timer = micros();
    _strip->begin();
    _strip->show();
    //rainbowWipeUp(1);
}

uint32_t StampyStrip::getColor(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;
}

void StampyStrip::loop() {
    
    if(_sampleIndex > _sampleBufferSize) {
        //        int32_t sampleSum = 0;
        //        for(int i = 0 ; i < _sampleBufferSize ; i++) {
        //            sampleSum += _sampleBuffer[i];
        //            _sampleBuffer1[i] = 0;
        //        }
        //        int32_t averaged = (sampleSum / _sampleBufferSize);
        //
        //        if(averaged1 < _readMin1) {
        //            _readMin1 = averaged1;
        //            //printRanges();
        //        }
        //        if(averaged1 > _readMax1) {
        //            _readMax1 = averaged1;
        //            //printRanges();
        //        }
        //        if(averaged2 < _readMin2) {
        //            _readMin2 = averaged2;
        //            //printRanges();
        //        }
        //        if(averaged2 > _readMax2) {
        //            _readMax2 = averaged2;
        //            //printRanges();
        //        }
        //        _xAccel = map(averaged1, _readMin1, _readMax1, -1000, 1000);
        //        _yAccel = map(averaged2, _readMin2, _readMax2, -1000, 1000);
        ////        Serial.print(_xAccel);
        ////        Serial.print(" x:y ");
        ////        Serial.println(_yAccel);
        _sampleIndex = 0;
        //show();
    }
    //
    //    int inputRead1 = analogRead(_inputPin1);
    //    int inputRead2 = analogRead(_inputPin2);
    //
    //    _sampleBuffer[_sampleIndex] = (uint16_t) inputRead1;
    //    _sampleBuffer2[_sampleIndex] = (uint16_t) inputRead2;
    sensors_event_t accel_event;
    sensors_event_t gyro_event;
    sensors_vec_t   orientation;
    if(_useInput) {
        _accel.getEvent(&accel_event);
        _gyro.getEvent(&gyro_event);
        if (_dof.accelGetOrientation(&accel_event, &orientation))
        {
            double dt = (double)(micros() - _timer) / 1000000; // Calculate delta time
            _kalmanX.setAngle(orientation.roll); // Set starting angle
            //_kalmanY.setAngle(orientation.pitch);
            double gyroXrate = gyro_event.gyro.roll / 131.0; // Convert to deg/s
            //double gyroYrate = gyro_event.pitch / 131.0; // Convert to deg/s
            double kalAngleX = _kalmanX.getAngle(orientation.roll, gyroXrate, dt);
            _currentRoll = kalAngleX;
            
            if(_currentRoll > _maxRoll) {
                _maxRoll = _currentRoll;
                Serial.print(F("Max: "));
                Serial.println(_maxRoll);
            }
            if(_currentRoll < _minRoll) {
                _minRoll = _currentRoll;
                Serial.print(F("Min: "));
                Serial.println(_minRoll);
            }
        }
    }
    
    _sampleIndex++;
    //
    show();
    _loopCount++;
    
    if(_loopCount > _loopInterval) _loopCount = 0;
}

void StampyStrip::show() {
    //Serial.println(index);
    float brightness = getBrightness(0);
    //if(brightness != _brightness) {
        _brightness = brightness;
      //  Serial.print("brightness: ");
        //Serial.println(brightness);
    //}

    for(uint16_t i=0; i < _pix; i++) {
        uint32_t thisColor = getLightColor(i);
        _strip->setPixelColor(i, thisColor);
    }
    _strip->show();
}

void StampyStrip::printRanges() {
    //    Serial.print(_readMin1);
    //    Serial.print(" - ");
    //    Serial.print(_readMax1);
    //    Serial.print(" & ");
    //    Serial.print(_readMin2);
    //    Serial.print(" - ");
    //    Serial.println(_readMax2);
}

//double StampyStrip::getAngle(uint32_t input1, uint32_t input2) {
//    return atan2((double)input1, (double)input2);
//}

uint32_t StampyStrip::getLightColor(int lightIndex) {
    int lightOffset = ((float)lightIndex / (float)_pix) * (float)_loopInterval;
    
    int colorLoopIndex = (_loopCount + lightOffset) % _loopInterval;
    //Serial.print("using color loop index ");Serial.print(colorLoopIndex);Serial.print(" for light " ); Serial.println(lightIndex);
    uint32_t color = getLoopedColor(colorLoopIndex, _loopInterval);
    return color;
}

float StampyStrip::getBrightness(int index) {
   return dmap(_currentRoll, _minRoll, _maxRoll, 0.10, 1);
}

uint8_t StampyStrip::getScaled(uint8_t num, float by) {
    return (uint8_t)((float)num * by);
}

uint32_t StampyStrip::getLoopedColor(int index, int interval)
{
    float rIndex = (float)(index % interval);
    float gIndex = (float)((index + (interval/3)) % interval);
    float bIndex = (float)((index + (2 * (interval/3))) % interval);
    
    float rRads = getRadians(rIndex, interval);
    float gRads = getRadians(gIndex, interval);
    float bRads = getRadians(bIndex, interval);
    
    uint8_t r = getScaled(sinMap(rRads), _brightness);
    uint8_t g = getScaled(sinMap(gRads), _brightness);
    uint8_t b = getScaled(sinMap(bRads), _brightness);
    
    //    Serial.print("index :");
    //    Serial.print(index);
    //    Serial.print(" r: ");
    //    Serial.print(r);
    //    Serial.print(" g: ");
    //    Serial.print(g);
    //    Serial.print(" b: ");
    //    Serial.println(b);
    //return getColor(r,g,b);
    return _strip->Color(r, g, b);
}

void StampyStrip::colorWipeDown(uint32_t c, uint8_t wait) {
    int8_t i, j;
    for(i = _pixCenter, j = _pixCenter + 1; i >= 0; i--, j++) {
        _strip->setPixelColor(i, c);
        if(j < _pix) _strip->setPixelColor(j, c);
        _strip->show();
        delay(wait);
    }
}

void StampyStrip::colorWipeUp(uint32_t c, uint8_t wait) {
    int8_t i, j;
    for(i = 0, j = _pix - 1; i < _pixCenter ; i++, j--) {
        _strip->setPixelColor(i, c);
        _strip->setPixelColor(j, c);
        _strip->show();
        delay(wait);
    }
}

void StampyStrip::rainbowWipeDown(uint8_t wait) {
    int8_t i, j;
    for(i = _pixCenter, j = _pixCenter + 1; i >= 0; i--, j++) {
        _strip->setPixelColor(i, getLoopedColor(i, _pix));
        if(j < _pix) _strip->setPixelColor(j, getLoopedColor(j, _pix));
        _strip->show();
        delay(wait);
    }
}

void StampyStrip::rainbowWipeUp(uint8_t wait) {
    int8_t i, j;
    for(i = 0, j = _pix - 1; i < _pixCenter ; i++, j--) {
        _strip->setPixelColor(i, getLoopedColor(i, _pix));
        _strip->setPixelColor(j, getLoopedColor(j, _pix));
        _strip->show();
        delay(wait);
    }
}

/* here be math helpers */
float StampyStrip::fmap(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

double StampyStrip::dmap(double x, double in_min, double in_max, double out_min, double out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float StampyStrip::getRadians(float index, int interval) {
    return fmap(index, 0.0, interval, 0.0, 6.28);
}

uint8_t StampyStrip::sinMap(float rads) {
    
    return (uint8_t)fmap(sin(rads), -1, 1, _bgMinBrightness, _bgMaxBrightness);
}