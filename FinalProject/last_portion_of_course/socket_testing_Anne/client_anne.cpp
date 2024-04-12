  #include <stdio.h>
  #include <unistd.h>
  #include <stdlib.h>
  #include <string.h>
  #include <sys/types.h> 
  #include <sys/socket.h>
  #include <Arduino.h>
  #include <WiFi.h>

// const char* ssid = "iPhone"; // Name of Network
// const char* password = "soccer101"; // Network Password



void setup() {
  Serial.begin(115200);
//   Serial.println("Before The connection");
//   WiFi.mode(WIFI_STA); //Optional
//   WiFi.begin(ssid, password);
//   Serial.println("\nConnecting");

//   while(WiFi.status() != WL_CONNECTED){
//       Serial.print(".");
//       delay(100);
 // }
 //send info from heltec to server which is nvida 
 //create client socket (heltec) which is connected to the hotspot wifi
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    // Serial.println("\nConnected to the WiFi network");
    // Serial.print("Local ESP32 IP: ");
    // Serial.println(WiFi.localIP());
  

    
    // CLIENT SOCKET SECTION *************************
     // Used when Heltec Sends Messages -> Nvidia
    //int clientSocket = socket(AF_INET, SOCK_STREAM, 0);   
    // specifying address 
    sockaddr_in serverAddress; 
    serverAddress.sin_family = AF_INET; 
    serverAddress.sin_port = htons(8000); 
    serverAddress.sin_addr.s_addr = inet_addr("172.20.10.2");

    connect(clientSocket, (struct sockaddr*)&serverAddress, 
            sizeof(serverAddress)); 
    const char* message = "Hello, server!"; 
    send(clientSocket, message, strlen(message), 0); 
  
    // closing socket 
    close(clientSocket); 
    //*************************************************
    
}

void loop() {
 
}
