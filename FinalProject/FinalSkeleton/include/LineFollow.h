#ifndef LINE_FOLLOW
#define LINE_FOLLOW

#include "motorFunctions.h"
#include <chrono>

// PIN Assignments
const unsigned int ADC_1_CS = 2;
const unsigned int ADC_2_CS = 17;

struct LineVal {
    uint8_t* lineValues;
    float pos;
};
void move_forward_until_interrupt(int case_val,Encoder& enc1, Encoder &enc2);

void setupLineFollow();

void readADC();

void digitalConvert();

LineVal getPosition(float previousPosition);

int lineFollowExit(int pid, int case_val, Encoder& enc1, Encoder& enc2);




#endif