//
//  StampyPixelLib.cpp
//  
//
//  Created by Erik Nelson on 8/16/13.
//
//

#include "StampyPixelLib.h"
#include "math.h"

StampyStrip::StampyStrip(uint16_t pix, uint16_t ledPin, uint16_t inputPin1, uint16_t inputPin2, uint16_t pixCenter) {
    _strip = new Adafruit_NeoPixel(pix, ledPin, NEO_GRB + NEO_KHZ800);
    _inputPin1 = inputPin1;
    _inputPin2 = inputPin2;
    _ledPin = ledPin;
    _pix = pix;
    _pixCenter = pixCenter;
    _readMin1 = 1024;
    _readMin2 = 1024;
    _readMax1 = 0;
    _readMax2 = 0;
}

void StampyStrip::begin() {
    _strip->begin();
    _strip->show();
    //rainbowWipeUp(50);
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
    double currentAngle;

    if(_sampleIndex >= _sampleBufferSize - 1) {
        uint32_t sampleSum1 = 0;
        uint32_t sampleSum2 = 0;
        for(int i = 0 ; i < _sampleBufferSize ; i++) {
            sampleSum1 += _sampleBuffer1[i];
            sampleSum2 += _sampleBuffer2[i];
            _sampleBuffer1[i] = 0;
            _sampleBuffer2[i] = 0;
        }
        uint32_t averaged1 = sampleSum1 / _sampleBufferSize;
        uint32_t averaged2 = sampleSum2 / _sampleBufferSize;
        if(averaged1 < _readMin1) {
            _readMin1 = averaged1;
            printRanges();
        }
        if(averaged1 > _readMax1) {
            _readMax1 = averaged1;
            printRanges();
        }
        if(averaged2 < _readMin2) {
            _readMin2 = averaged2;
            printRanges();
        }
        if(averaged2 > _readMax2) {
            _readMax2 = averaged2;
            printRanges();
        }
        long xScaled = map(averaged1, _readMin1, _readMax1, -1000, 1000);
        long yScaled = map(averaged2, _readMin2, _readMax2, -1000, 1000);
        float xAccel = xScaled / 1000.0;
        float yAccel = yScaled / 1000.0;
        Serial.print(xAccel);
        Serial.print(" x:y ");
        Serial.println(yAccel);
        
        _lastIndex = currentIndex;
        currentAngle = getAngle(averaged1, averaged2);
        if(currentAngle > _maxAngle) {
            _maxAngle = currentAngle;
        }
        _lastAngle = currentAngle;
        //Serial.println(currentAngle);
        _sampleIndex = 0;
        //showPosition(currentIndex, currentSpeed);
    }
    else {
        currentIndex = _lastIndex;
        //currentSpeed = _lastSpeed;
    }

    int inputRead1 = analogRead(_inputPin1);
    int inputRead2 = analogRead(_inputPin2);
//
//    Serial.print(inputRead1);
//    Serial.print(" : ")
//    Serial.print(inputRead2)
//    if(inputRead1 < _readLimitLow) inputRead1 = _readLimitLow1;
//    if(inputRead1 > _readLimitHigh) inputRead1 = _readLimitHigh1;
    
    _sampleBuffer1[_sampleIndex++] = (uint16_t) inputRead1;
    _sampleBuffer2[_sampleIndex++] = (uint16_t) inputRead2;
    _sampleIndex++;
        
    _loopCount++;
    
    if(_loopCount > _loopInterval) _loopCount = 0;
}

//void StampyStrip::showPosition(int index, int16_t currentSpeed) {
//    //Serial.println(index);
//    for(uint16_t i=0; i < _pix; i++) {
//       // Serial.println("settings a pixel color");
//        _strip->setPixelColor(i, getLightColor(i));
//        //_strip->show();
//    }
//    
//    _strip->show();
//
//    uint8_t red = 255;
//    uint8_t blue = 255;
//    uint8_t green = 255;
//
//    uint32_t color = getColor(red, green, blue);
//
//    ///Serial.print("display color is "); Serial.println(color);
//    
//    for(int j=index - (_displaySize/2); j < index + (_displaySize/2); j++) {
//        if(j > 0 && j < _pix)
//            _strip->setPixelColor(j, color);
//    }
//
//    _strip->show();
//}

void StampyStrip::printRanges() {
    Serial.print(_readMin1);
    Serial.print(" - ");
    Serial.print(_readMax1);
    Serial.print(" & ");
    Serial.print(_readMin2);
    Serial.print(" - ");
    Serial.println(_readMax2);
}

double StampyStrip::getAngle(uint32_t input1, uint32_t input2) {
    return atan2((double)input1, (double)input2);
}

uint32_t StampyStrip::getLightColor(int lightIndex) {
    int lightOffset = ((float)lightIndex / (float)_pix) * (float)_loopInterval;

    int colorLoopIndex = (_loopCount + lightOffset) % _loopInterval;
    //Serial.print("using color loop index ");Serial.print(colorLoopIndex);Serial.print(" for light " ); Serial.println(lightIndex);
    return getLoopedColor(colorLoopIndex, _loopInterval);
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