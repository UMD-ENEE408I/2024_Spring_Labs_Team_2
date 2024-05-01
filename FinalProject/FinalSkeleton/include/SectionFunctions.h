#include "motorFunctions.h"
#include "LineFollow.h"
#include "Networking.h"

// Definition: Transition 
void transitionRight(int direction, Encoder& enc1, Encoder& enc2);

void transitionLeft(int direction, Encoder& enc1, Encoder& enc2);

void audioTurn(Encoder& enc1, Encoder & enc2);
