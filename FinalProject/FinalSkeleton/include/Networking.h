#ifndef NETWORKING
#define NETWORKING

#include <WiFi.h>
#include <sys/socket.h>

void setupNetworking();

int recieveUntilStop();

int sendRecvSingleMessage( int message);

#endif