
#include <WiFi.h>
#include <sys/socket.h>

const char* ssid = "Elink"; // Name of Network
const char* password = "David!Kristina"; // Network Password

void setup() {
    Serial.begin(115200);
    delay(1000);
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
    // SERVER SOCKET SECTION *************************
    // Used when Nvidia Sends Messages -> Heltec
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0); 
    sockaddr_in serverAddress; 
    serverAddress.sin_family = AF_INET; 
    serverAddress.sin_port = htons(8000); 
    String ipString = WiFi.localIP().toString();
    serverAddress.sin_addr.s_addr = inet_addr(ipString.c_str()); 
    // binding socket. 
    bind(serverSocket, (struct sockaddr*)&serverAddress, 
         sizeof(serverAddress)); 
    listen(serverSocket, 5); 
    // accepting connection request 
    int clientSocket = accept(serverSocket, nullptr, nullptr); 
  
    // recieving data 
    char buffer[1024] = { 0 }; 
    recv(clientSocket, buffer, sizeof(buffer), 0); 
    Serial.println(buffer);

    // closing the socket. 
    close(serverSocket);

    /*
    // CLIENT SOCKET SECTION *************************
     // Used when Heltec Sends Messages -> Nvidia
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);   
    // specifying address 
    sockaddr_in serverAddress; 
    serverAddress.sin_family = AF_INET; 
    serverAddress.sin_port = htons(8000); 
    serverAddress.sin_addr.s_addr = inet_addr("192.168.1.169");

    connect(clientSocket, (struct sockaddr*)&serverAddress, 
            sizeof(serverAddress)); 
    const char* message = "Hello, server!"; 
    send(clientSocket, message, strlen(message), 0); 
  
    // closing socket 
    close(clientSocket); 
    //*************************************************
    */
}

void loop() {
 
}
