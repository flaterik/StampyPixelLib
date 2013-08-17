//
//  StampyPixelLib.cpp
//  
//
//  Created by Erik Nelson on 8/16/13.
//
//

#include "StampyPixelLib.h"

StampyStrip::StampyStrip(uint16_t pix, uint16_t ledPin, uint16_t inputPin, uint16_t pixCenter) {
    _strip = new Adafruit_NeoPixel(pix, ledPin, NEO_GRB + NEO_KHZ800);
    _inputPin = inputPin;
    _ledPin = ledPin;
    _pix = pix;
    _pixCenter = pixCenter;
    _bgColor = getColor(50,50,50);
}

void StampyStrip::begin() {
    _strip->begin();
    _strip->show();
}

uint32_t StampyStrip::getColor(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;
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

void StampyStrip::loop() {
    
    int16_t currentIndex;
    int16_t currentSpeed;

    if(_sampleIndex >= _sampleBufferSize - 1) {
        uint32_t sampleSum = 0;
        for(int i = 0 ; i < _sampleBufferSize ; i++) {
            sampleSum += _sampleBuffer[i];
            _sampleBuffer[i] = 0;
        }
        //Serial.println(sampleSum);
        uint32_t averaged = sampleSum / _sampleBufferSize;
        if(averaged < _readMin) {
            _readMin = averaged;
        }
        if(averaged > _readMax) {
            _readMax = averaged;
        }
        currentIndex = (uint16_t) map(averaged, _readMin, _readMax, 0, _pix - 1);
        currentSpeed = abs(currentIndex - _lastIndex);
        _lastIndex = currentIndex;
        //Serial.println(currentSpeed);
        if(currentSpeed > _maxSpeed) {
            _maxSpeed = currentSpeed;
            //Serial.print("New max speed!"); Serial.println(_maxSpeed);
        }
        _lastSpeed = currentSpeed;
        //Serial.println(_loopCount);
        _sampleIndex = 0;
        //Serial.print("current index "); Serial.println(currentIndex);
        //_bgColor = getLoopedColor();
        showPosition(currentIndex, currentSpeed, getLoopedColor());
        //Serial.print("got bg color "); Serial.println(_bgColor);
    }
    else {
        currentIndex = _lastIndex;
        currentSpeed = _lastSpeed;
    }
//
    int inputRead = analogRead(_inputPin);
    if(inputRead < _readLimitLow) inputRead = _readLimitLow;
    if(inputRead > _readLimitHigh) inputRead = _readLimitHigh;
    
    _sampleBuffer[_sampleIndex++] = (uint16_t) inputRead;
    _sampleIndex++;
        
    //showPosition(currentIndex, currentSpeed);
    
    _loopCount++;
    
    if(_loopCount > _loopInterval) _loopCount = 0;
}

void StampyStrip::showPosition(int index, int16_t currentSpeed, uint32_t bgColor) {
//    Serial.print("Setting current position "); Serial.print(index);
//    Serial.print(" with speed "); Serial.print(currentSpeed);
    Serial.print("bg color is "); Serial.println(bgColor);
    for(uint16_t i=0; i < _pix; i++) {
        _strip->setPixelColor(i, bgColor);
    }

    uint8_t red = 125;
    uint8_t blue = 125;
    uint8_t green = 125;

    uint32_t color = getColor(red, green, blue);

    Serial.print("display color is "); Serial.println(color);
    for(int j=index - (_displaySize/2); j < index + (_displaySize/2); j++) {
        if(j > 0 && j < _pix)
            _strip->setPixelColor(j, color);
    }

    _strip->show();
}

uint32_t StampyStrip::getLoopedColor()
{
    return getLoopedColor(_loopCount, _loopInterval);
}

uint32_t StampyStrip::getLoopedColor(int index, int interval)
{
    float rIndex = (float)(index % interval);
    float gIndex = (float)((index + (interval/3)) % interval);
    float bIndex = (float)((index + (2 * (interval/3))) % interval);
    
    float rRads = getRadians(rIndex, interval);
    float gRads = getRadians(gIndex, interval);
    float bRads = getRadians(bIndex, interval);
    
    uint8_t r = sinMap(rRads);
    uint8_t g = sinMap(gRads);
    uint8_t b = sinMap(bRads);
    
    return getColor(r,g,b);
}

/* here be math helpers */
float StampyStrip::fmap(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float StampyStrip::getRadians(float index, int interval) {
    return fmap(index, 0.0, interval, 0.0, 6.28);
}

uint8_t StampyStrip::sinMap(float rads) {
    return (uint8_t)fmap(sin(rads), -1, 1, _bgMinBrightness, _bgMaxBrightness);
}