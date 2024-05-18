
#include "motorFunctions.h"
#include "lineFollow.h"
#include "Networking.h"
#include "Maze.h"
#include "SectionFunctions.h"

const int HELTEC_ID = 1;

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
  setupNetworking(HELTEC_ID);
  delay(2000);
}

void loop() {
  
  Encoder enc1(M1_ENC_A, M1_ENC_B); // Allocate Encoders Manually To Prevent Memory Leak
  Encoder enc2(M2_ENC_A, M2_ENC_B);
  
  while(true){
    //lineFollowExit(95,2,enc1,enc2);
    /*
    delay(1000);
    move_forward_until_interrupt(1,enc1,enc2);
    delay(300);
    move_forward(750,enc1,enc2);
    delay(300);
    turnConsistent(turn90L,Left,enc1,enc2);
    delay(300);
    move_forward_until_interrupt(0,enc1,enc2);
    delay(300);
    move_forward(200,enc1,enc2);
    delay(300);
    turnConsistent(turn90R,Right,enc1,enc2);
    delay(5000);
    lineFollowExit(0,100,enc1,enc2);
  */

    delay(2000);
    move_forward_until_interrupt(1,enc1,enc2);
    lineFollowExit(90,0,enc1,enc2);
    transitionRight(1,enc1,enc2); // Line of the Republic and Transition into Maze
    lineFollowExit(90,1,enc1,enc2); // Position at Node 0 of Maze
    if (HELTEC_ID ==1){
      solveMaze(HELTEC_ID,enc1,enc2);
    } else {
      delay(100);
      turnConsistent(turn90R,Right,enc1,enc2);
      delay(100);
      lineFollowExit(90,1,enc1,enc2);
      delay(100);
      lineFollowExit(90,1,enc1,enc2);
      delay(100);
      lineFollowExit(90,1,enc1,enc2);
      delay(250);
      lineFollowExit(90,1,enc1,enc2);
      delay(250);
      lineFollowExit(90,1,enc1,enc2);
    }
    
    transitionLeft(0,enc1,enc2); // Transition into Kessel Run 
    lineFollowExit(79,0,enc1,enc2); // Kessel Run
    //transitionLeft(1,enc1,enc2); // Transition to Hoth Asteroid Field
    //lineFollowExit(95,2,enc1,enc2); // Hoth Asteroid Field

    move_forward_until_interrupt(1,enc1,enc2);
    delay(100);
    move_forward(600,enc1,enc2);
    delay(100);
    turnConsistent(turn90L+10,Left,enc1,enc2);
    delay(100);
    move_forward_until_interrupt(0,enc1,enc2);
    delay(100);
    move_forward(180,enc1,enc2);
    delay(100);
    turnConsistent(turn90R,Right,enc1,enc2);
    delay(100);
    
    //transitionRight(1,enc1,enc2); // Transition to Sound Detection

    audioTurn(enc1,enc2); // Audio Detection
    delay(100);
    transitionLeft(0,enc1,enc2); // Transitioning into Endor Dash
    lineFollowExit(80,0,enc1,enc2);
    delay(100);
    transitionLeft(1,enc1,enc2);
    delay(1000);
    move_forward_until_interrupt(0,enc1,enc2); // Endor Dash
    move_forward(300,enc1,enc2);
    delay(10000);
    
    
   //lineFollowExit(100,2,enc1,enc2);
    
  }
  
} 

