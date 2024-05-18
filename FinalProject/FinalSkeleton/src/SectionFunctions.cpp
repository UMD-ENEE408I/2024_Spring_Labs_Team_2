#include "motorFunctions.h"
#include "LineFollow.h"
#include "Networking.h"

// Definition: Transition 
void transitionRight(int direction, Encoder& enc1, Encoder& enc2){
  if (!direction){
  lineFollowExit(100,0, enc1, enc2); // 0 = No Interrupt + Sensor 1 and 13 Necessary for Interrupt
  }
  // 1 Circumfrence = 3.92 Inches
  // We Want to Travel Roughly 9 Inches, 1 revoluti
  int rev = 30*12;
  int dist = rev*(9/4)-100; // Error of -100
  move_forward(dist, enc1,enc2);
  delay(150);
  turnConsistent(turn90R, Right, enc1,enc2);
  delay(150);
  move_forward_until_interrupt(1,enc1,enc2);
  move_forward(50,enc1,enc2);
  delay(150);
}

void transitionLeft(int direction, Encoder& enc1, Encoder& enc2){
  if (!direction){
  lineFollowExit(85,0, enc1, enc2); // 0 = No Interrupt + Sensor 1 and 13 Necessary for Interrupt
  }
  // 1 Circumfrence = 3.92 Inches
  // We Want to Travel Roughly 9 Inches, 1 revoluti
  int rev = 30*12;
  int dist = rev*(9/4); // Error of -100
  move_forward(dist-100, enc1,enc2);
  delay(150);
  turnConsistent(turn90L, Left, enc1,enc2);
  delay(150);
  //move_forward(dist+50, enc1,enc2);
  move_forward_until_interrupt(1,enc1,enc2);
  move_forward(50,enc1,enc2);
  delay(150);
}

void audioTurn(Encoder& enc1, Encoder & enc2){
  lineFollowExit(90,1,enc1,enc2);
  delay(150);
  turnConsistent(turn90R,Right,enc1,enc2);
  delay(150);
  lineFollowExit(90,1,enc1,enc2);
  delay(150);
  
  /*
  // First Turn to the Left
  turnConsistent(turn90L,Left,enc1,enc2);
  delay(500);
  lineFollowExit(90,1,enc1,enc2);
  delay(500);
  turnConsistent(turn90R,Right,enc1,enc2);
  delay(250);
  int result_left = sendRecvSingleMessage(3);
  // Turn to the Right
  delay(500);
  turnConsistent(turn90R,Right,enc1,enc2);
  delay(500);
  lineFollowExit(90,1,enc1,enc2);
  delay(500);
  lineFollowExit(90,1,enc1,enc2);
  delay(500);
  turnConsistent(turn90R,Right,enc1,enc2);
  int result_right = sendRecvSingleMessage(3);
  delay(500);
  turnConsistent(turn90R,Right,enc1,enc2);
  delay(500);
  lineFollowExit(90,1,enc1,enc2);
  delay(500);
  bool result = false;
  if (result_left < result_right){
    result = true;
    turnConsistent(turn180R,Right,enc1,enc2);
  }
  */
  int result = sendRecvSingleMessage(3);

  if (result){
    turnConsistent(turn90R,Right,enc1,enc2);
    delay(250);
    lineFollowExit(90,1,enc1,enc2);
    delay(250);
    turnConsistent(turn90L,Left,enc1,enc2);
    delay(250);
    lineFollowExit(90,1,enc1,enc2);
    delay(250);
    turnConsistent(turn90L,Left,enc1,enc2);
    delay(250);
    lineFollowExit(90,1,enc1,enc2);
    delay(250);
    turnConsistent(turn90R,Right,enc1,enc2);
    delay(250);
  } else {
    turnConsistent(turn90L,Left,enc1,enc2);
    delay(250);
    lineFollowExit(90,1,enc1,enc2);
    delay(250);
    turnConsistent(turn90R,Right,enc1,enc2);
    delay(250);
    lineFollowExit(90,1,enc1,enc2);
    delay(250);
    turnConsistent(turn90R,Right,enc1,enc2);
    delay(250);
    lineFollowExit(90,1,enc1,enc2);
    delay(250);
    turnConsistent(turn90L,Left,enc1,enc2);
  } 
}