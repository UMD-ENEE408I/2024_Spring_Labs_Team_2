#ifndef MOTOR_FUNCTIONS
#define MOTOR_FUNCTIONS

#include <Arduino.h>
#include <Adafruit_MCP3008.h>
#include <Encoder.h>

const unsigned int PWM_VALUE = 512;
const unsigned int M1_IN_1 = 13;
const unsigned int M1_IN_2 = 12;
const unsigned int M2_IN_1 = 25;
const unsigned int M2_IN_2 = 14;

const unsigned int M1_IN_1_CHANNEL = 8;
const unsigned int M1_IN_2_CHANNEL = 9;
const unsigned int M2_IN_1_CHANNEL = 10;
const unsigned int M2_IN_2_CHANNEL = 11;

const unsigned int M1_I_SENSE = 35;
const unsigned int M2_I_SENSE = 34;

const unsigned int M1_ENC_A = 39;
const unsigned int M1_ENC_B = 38;
const unsigned int M2_ENC_A = 37;
const unsigned int M2_ENC_B = 36;

const unsigned int base_pid = 350;

const bool Right = true;
const bool Left = false;
const int turn90R = 200; // Adjust these values based on surface
const int turn180R = 500;
const int turn90L = 150;
const int turn180L = 450;


void setupMotors();

void M1_backward(int pwm_value);

void M1_forward(int pwm_value);

void M1_stop();

void M2_backward(int pwm_value);

void M2_forward(int pwm_value);

void M2_stop();

void move_forward(int distance, Encoder& enc1, Encoder& enc2);

void turnConsistent(int distance, bool direction, Encoder& enc1 , Encoder& enc2);

#endif