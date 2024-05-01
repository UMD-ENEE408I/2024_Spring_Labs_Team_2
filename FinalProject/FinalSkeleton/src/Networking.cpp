 #include "Networking.h"
 #include <string.h>

const char* ssid = "DavidNetwork"; // Name of Network
const char* password = "Dina!3276"; // Network Password

// Constructs: 
// When nvida sends Stop signal, Heltec stops recieving
int clientSocket;

void setupNetworking() {
    // Connecting to Mutual WiFi Network
    WiFi.mode(WIFI_STA); //Optional
    WiFi.begin(ssid, password);
    Serial.println("\nConnecting");
    while(WiFi.status() != WL_CONNECTED){
        Serial.print(".");
        delay(100);
    }
    Serial.println("\nConnected to the WiFi network");
    Serial.print("Local ESP32 IP: ");
    Serial.println(WiFi.localIP());

    // Initialize Client/Server Side Connection
    const char * IPAddress = "172.20.10.7"; // Known Nvidia Ip address 
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);   
    sockaddr_in serverAddress; 
    serverAddress.sin_family = AF_INET; 
    serverAddress.sin_port = htons(8000); 
    serverAddress.sin_addr.s_addr = inet_addr(IPAddress);
    connect(clientSocket, (struct sockaddr*)&serverAddress, 
            sizeof(serverAddress)); 

} 

int sendRecvSingleMessage(int message){
    // Send Single Message
    send(clientSocket, &message, sizeof(message), 0); 
    // Recieve Single Message
    int recieved_integer = 0;
    int bytes_recieved = recv(clientSocket,&recieved_integer,sizeof(recieved_integer),0);
    //Serial.print("Message Recieved:  ");
    //Serial.println(recieved_integer);
    return recieved_integer;
}

int recieveUntilStop() {
    int returnval;
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0); 
    sockaddr_in serverAddress; 
    serverAddress.sin_family = AF_INET; 
    serverAddress.sin_port = htons(8000); 
    String ipString = WiFi.localIP().toString();
    serverAddress.sin_addr.s_addr = inet_addr(ipString.c_str()); 
    // Binding Socket for Permanent Listening
    bind(serverSocket, (struct sockaddr*)&serverAddress, 
         sizeof(serverAddress)); 
    listen(serverSocket, 5); 
    // Accept the incoming Connection
     int clientSocket = accept(serverSocket, nullptr, nullptr);
    boolean recvFlag = true;
    while (recvFlag) {
    // recieving data 
    char buffer[1024] = { 0 }; 
    
    ssize_t bytesRecieved = recv(clientSocket, buffer, sizeof(buffer), 0); 

    buffer[bytesRecieved] = '\0'; // Null terminating string
    Serial.println(buffer);

    if (strcmp(buffer,"right")==0) {
        recvFlag = false;
        Serial.println("##End Recieve##");
        returnval = 1;
    }

    if (strcmp(buffer,"left")==0) {
        recvFlag = false;
        Serial.println("##End Recieve##");
        returnval = 2;
    }

    if (strcmp(buffer,"stop")==0) {
        recvFlag = false;
        Serial.println("##End Recieve##");
        returnval = 3;
    }

    }
    // closing the socket. 
    close(serverSocket);
    return returnval;
}