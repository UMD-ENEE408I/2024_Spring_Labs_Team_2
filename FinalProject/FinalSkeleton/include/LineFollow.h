#ifndef LINE_FOLLOW
#define LINE_FOLLOW

#include "motorFunctions.h"

// PIN Assignments
const unsigned int ADC_1_CS = 2;
const unsigned int ADC_2_CS = 17;

struct LineVal {
    uint8_t* lineValues;
    float pos;
};

void setupLineFollow();

void readADC();

void digitalConvert();

LineVal getPosition(float previousPosition);

int lineFollowExit();




#endif