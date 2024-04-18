
#include "motorFunctions.h"
#include "lineFollow.h"
#include "Networking.h"
#include "Maze.h"

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
  //setupNetworking();
  delay(2000);
  
}

void loop() {

  solveMaze();

} 

