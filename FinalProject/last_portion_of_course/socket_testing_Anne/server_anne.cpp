  #include <stdio.h>
  #include <unistd.h>
  #include <stdlib.h>
  #include <string.h>
  #include <sys/types.h> 
  #include <sys/socket.h>
  #include <Arduino.h>



void setup() {
    // Serial.begin(115200);
    // SERVER SOCKET SECTION *************************
    // Used when Nvidia Sends Messages -> Heltec
    // create socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0); 
    //define socket address
    sockaddr_in serverAddress; 
    serverAddress.sin_family = AF_INET; 
    serverAddress.sin_port = htons(8000); 
    
    serverAddress.sin_addr.s_addr = inet_addr("172.20.10.2"); 
    //printf("server address = %s", &serverAddress.sin_addr.s_addr);
    //printf("Server Address = %s",inet_ntop(AF_INET,&serverAddress.sin_addr.s_addr);
   
    // binding socket. 
    bind(serverSocket, (struct sockaddr*)&serverAddress, 
         sizeof(serverAddress)); 
    listen(serverSocket, 5); 
    printf("LISTENING");
    // accepting connection request 
    int clientSocket = accept(serverSocket, nullptr, nullptr); 
  
    // recieving data from client (heltec)
    char buffer[1024] = { 0 }; 
    recv(clientSocket, buffer, sizeof(buffer), 0); 
    Serial.println(buffer);

    // closing the socket. 
    close(serverSocket);

  
}

void loop() {
 
}
