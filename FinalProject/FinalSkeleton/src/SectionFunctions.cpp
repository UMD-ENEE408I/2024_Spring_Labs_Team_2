#include "motorFunctions.h"
#include "LineFollow.h"
#include "Networking.h"

// Definition: Transition 
void transitionRight(int direction, Encoder& enc1, Encoder& enc2){
  if (!direction){
  lineFollowExit(0, enc1, enc2); // 0 = No Interrupt + Sensor 1 and 13 Necessary for Interrupt
  }
  // 1 Circumfrence = 3.92 Inches
  // We Want to Travel Roughly 9 Inches, 1 revoluti
  int rev = 30*12;
  int dist = rev*(9/4)-50; // Error of -100
  move_forward(dist, enc1,enc2);
  turnConsistent(turn90R, Right, enc1,enc2);
  move_forward(dist+50, enc1,enc2);
  //move_forward_until_interrupt(enc1,enc2);
}

void transitionLeft(int direction, Encoder& enc1, Encoder& enc2){
  if (!direction){
  lineFollowExit(0, enc1, enc2); // 0 = No Interrupt + Sensor 1 and 13 Necessary for Interrupt
  }
  // 1 Circumfrence = 3.92 Inches
  // We Want to Travel Roughly 9 Inches, 1 revoluti
  int rev = 30*12;
  int dist = rev*(9/4)-50; // Error of -100
  move_forward(dist, enc1,enc2);
  turnConsistent(turn90L, Left, enc1,enc2);
  move_forward(dist+50, enc1,enc2);
}

void audioTurn(Encoder& enc1, Encoder & enc2){
  lineFollowExit(1,enc1,enc2);
  turnConsistent(turn90R,Right,enc1,enc2);
  lineFollowExit(1,enc1,enc2);
  int result = sendRecvSingleMessage(3);
  if (result){
    turnConsistent(turn90R,Right,enc1,enc2);
    lineFollowExit(1,enc1,enc2);
    turnConsistent(turn90L,Left,enc1,enc2);
    lineFollowExit(1,enc1,enc2);
    turnConsistent(turn90L,Left,enc1,enc2);
    lineFollowExit(1,enc1,enc2);
    turnConsistent(turn90R,Right,enc1,enc2);
  } else {
    turnConsistent(turn90L,Left,enc1,enc2);
    lineFollowExit(1,enc1,enc2);
    turnConsistent(turn90R,Right,enc1,enc2);
    lineFollowExit(1,enc1,enc2);
    turnConsistent(turn90R,Right,enc1,enc2);
    lineFollowExit(1,enc1,enc2);
    turnConsistent(turn90L,Left,enc1,enc2);
  } 
}