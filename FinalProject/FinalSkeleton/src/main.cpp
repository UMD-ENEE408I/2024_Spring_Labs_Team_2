
#include "motorFunctions.h"
#include "lineFollow.h"
#include "Networking.h"
#include "Maze.h"
#include "SectionFunctions.h"
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
  delay(2000);
}

void loop() {
  
  Encoder enc1(M1_ENC_A, M1_ENC_B); // Allocate Encoders Manually To Prevent Memory Leak
  Encoder enc2(M2_ENC_A, M2_ENC_B);
  
  while(true){
    solveMaze(enc1,enc2);
    delay(5000);
    //move_forward(500,enc1,enc2);
    //audioTurn(enc1,enc2);
    /*
    transitionRight(0,enc1,enc2); // Line of the Republic and Transition into Maze
    lineFollowExit(1,enc1,enc2); // Position at Node 0 of Maze
    solveMaze(enc1,enc2); // Solve Maze of Mandalore

    transitionLeft(0,enc1,enc2); // Transition into Kessel Run 
    lineFollowExit(0,enc1,enc2); // Kessel Run

    transitionLeft(0,enc1,enc2); // Transition to Hoth Asteroid Field

    lineFollowExit(0,enc1,enc2); // Hoth Asteroid Field

    transitionRight(1,enc1,enc2); // Transition to Sound Detection

    audioTurn(enc1,enc2); // Audio Detection

    transitionLeft(0,enc1,enc2); // Transitioning into Endor Dash
    transitionLeft(0,enc1,enc2);
    
    move_forward_until_interrupt(enc1,enc2); // Endor Dash
    
    delay(10000);
    */
    

  }
  
} 

