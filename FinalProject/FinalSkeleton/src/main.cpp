
#include "motorFunctions.h"
#include "lineFollow.h"
#include "Networking.h"

const bool Right = true;
const bool Left = false;
const int turn90R = 250; // Adjust these values based on surface
const int turn180R = 500;
const int turn90L = 200;
const int turn180L = 450;

const int RightSignal = 1;
const int LeftSignal = 2;
const int StopSignal = 3;

void setup() {

  // Preventing Encoder Bug
  pinMode(14, OUTPUT);
  digitalWrite(14, LOW);
  delay(100);
  Serial.begin(115200);
 
  // Fundamental Setup Functions for 1. Motors, 2. Linefollow-ADC's, 3. Networking
  setupMotors();
  setupLineFollow();
  setupNetworking();
  delay(5000);
}

void loop() {
    // Ex. Move Forward Indefinitely
    move_forward(0);
    // Ex. Networking Test
    /*
    delay(2000);
    recieveUntilStop();
    */
    // Ex. Vertex Handling Test
    /*
    delay(2000);
    lineFollowExit();
    delay(2000);
    move_forward(175);
    delay(2000);
    turnConsistent(turn90R,Right);
    delay(2000);
    */
  
}

