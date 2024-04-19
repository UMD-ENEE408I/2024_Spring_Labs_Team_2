// this program runs on the heltec and sends a string message to the nvida successfully
//pid loop for both wheels
//reaches target but takes too long
// M1 is left motor, facing forward, M2 is right motor
#include <Arduino.h>
#include <stdio.h> 
#include <Encoder.h>
//#include "../.pio/libdeps/heltec_wifi_kit_32_v2/Encoder/Encoder.h"
#include <Adafruit_MCP3008.h>
//#include "../.pio/libdeps/heltec_wifi_kit_32_v2/Adafruit MCP3008/Adafruit_MCP3008.h"
#include <WiFi.h>
#include <sys/socket.h>

//encoder pins
const unsigned int M1_ENC_A = 39;
const unsigned int M1_ENC_B = 38;
const unsigned int M2_ENC_A = 37;
const unsigned int M2_ENC_B = 36;

long previous_time;
long previous_error; //should these two go into void loop?
float error_integral;
int flag;
//float error_derivative;
 
//motor pins
const unsigned int M1_IN_1 = 13; //pins that singals appear on
const unsigned int M1_IN_2 = 12;
const unsigned int M2_IN_1 = 25;
const unsigned int M2_IN_2 = 14;
const unsigned int M1_IN_1_CHANNEL = 8; //channels that generate the pwm signals
const unsigned int M1_IN_2_CHANNEL = 9;
const unsigned int M2_IN_1_CHANNEL = 10;
const unsigned int M2_IN_2_CHANNEL = 11;

const unsigned int PWM_VALUE = 512; // 512 Max PWM given 8 bit resolution

const int freq = 5000;
const int ledChannel = 0;
const int resolution = 10;

const float Kp_1 = 7; //unstable at kp = 9
const float Kd_1 = 0;
const float Ki_1 = 10; //ki =10

const float Kp_2 = 9;
const float Kd_2 = 0;
const float Ki_2 = 10;

const char* ssid = "iPhone"; // Name of Network
const char* password = "soccer101"; // Network Password
int clientSocket;


void Send( int sendData ) {
    send(clientSocket, &sendData, sizeof(sendData), 0); 
}

int writetextfile(float Kp, float Ki, float Kd, int speed, int enc1_value, float error, long current_time ) {
    /* Open a plain file for writing */
     FILE *fp = fopen("C:\\Anne\\UMD\\2024_Spring_Labs_Team_2-main\\Tutorial_1\\Encoders\\src\\f.txt", "a"); //date and current time file name 
 
    if (fp == NULL) { 
        printf("Error opening file!\n"); 
        return 1; 
    } 
     if (fp != NULL) { 
        printf("Kp, Ki, Kd, speed, enc1_value, current_time \n"); 
        
    } 
 
    int result = fprintf(fp, "%f , %f ,%f , %d , %d, %f, %lli \n", Kp, Ki, Kd, speed, enc1_value, error, current_time); // write to file
    if (result == EOF) { 
        printf("Error writing to file!\n"); 
        fclose(fp); 
        return 1; 
    } 
 
    printf("Successfully wrote text to file.\n"); 
    fclose(fp); 
 
    return 0; 
}
//motor pwm need to be set currently above 300pwm, speed input into lcd write has to be positive
void M2_set_speed(int speed) { //right motor facing forward
  if(speed<0) { //turn foward 
     if(abs(speed)>512){
      speed=-512;
     }
     speed = abs(speed);
     ledcWrite(M2_IN_1_CHANNEL, 0);
     ledcWrite(M2_IN_2_CHANNEL, speed);
  }else if(speed>0){  //speed is positive
    if (abs(speed)>512){
      speed =512;
    }
    ledcWrite(M2_IN_1_CHANNEL, speed); //turn backward
    ledcWrite(M2_IN_2_CHANNEL, 0);
  }
  else { //shut off motor
   ledcWrite(M2_IN_1_CHANNEL, speed); //turn backward
   ledcWrite(M2_IN_2_CHANNEL,speed);
  }

}

void M1_set_speed(int speed) { //left motor facing forward

  if(speed<0) { //turn backward left motor used for left turn
    if(abs(speed)>512){
       speed=-512;
    }
    speed = abs(speed);
    ledcWrite(M1_IN_1_CHANNEL, speed); //ledcwrite(channel, duty cycle)
    ledcWrite(M1_IN_2_CHANNEL, 0);
  
  }else if(speed>0){  //turn forward usef for right turn
    if(abs(speed)>512){
        speed=512;
    }  
    ledcWrite(M1_IN_1_CHANNEL, 0);
    ledcWrite(M1_IN_2_CHANNEL, speed);
  }else { //shut off motor
   ledcWrite(M1_IN_1_CHANNEL, speed); //turn backward
   ledcWrite(M1_IN_2_CHANNEL, speed);
  }
  
}


//pass in encoder count for M1 and the desired turn count for M1 as well as parameters Kp,Ki,Kd,this returns the speed value to intput inot the motors
int PID_controller_turn_speed(int enc_value, float Kp, float Kd,float Ki,int ending_encoder_count) {
 // referenrece is the desired_turn_count
  //enc_value instead of specifically enc_valueM1 and M2
  // int amount_to_finish_turn = amount_to_finish_turn;
  long current_time = micros(); //get current time
  float delta_t = (current_time - previous_time)/(1.0e6); //convert time to seconds
  float error = ending_encoder_count - enc_value; // -200 - 0, -200 - -20 = -180ending encoder count = initialenc1valuel+90, currnetl reading this is 0 when there is an error of 200
  float error_derivative = (error - previous_error)/delta_t;
  error_integral = error_integral + error*delta_t; // is it posible that the second term is 0 because of too small of a delta t
  //should be global
  int speed = Kp*error + Kd*error_derivative + Ki*error_integral;
 //ramp input for position encoder
  
  previous_error = error; //should be global
  previous_time = current_time; //should be global

  //Send(enc_value); //send to server (NVIDIA)

  // Serial.print("M1 values");
  // Serial.println();
  // Serial.print(current_time);
  // Serial.print(",");
  // Serial.print(Kp);
  // Serial.print(",");
  // Serial.print(Kd); 
  // Serial.print(",");
  // Serial.print(Ki);
  // Serial.print(",");
  // Serial.print(speed);
  // Serial.print(",");
  // Serial.print(enc_value);
  // Serial.print(",");
  // Serial.print(error);
  // Serial.print(",");
  // Serial.print(error_integral);
  // Serial.print(",");

  return speed;
}


//this function will turn the mouse 90 degrees to the left, motors are in opposite directions so their encoder count will be the same sign depending on the direction you wish to turn
void set_both_motors_speed(int ending_encoder_count_M1, int ending_encoder_count_M2, int enc_value_M1,int enc_value_M2, float Kp_1, float Kd_1, float Ki_1,float Kp_2, float Kd_2, float Ki_2) {
  int speedM1 = PID_controller_turn_speed(enc_value_M1, Kp_1, Kd_1, Ki_1,ending_encoder_count_M1);
  int speedM2 = PID_controller_turn_speed(enc_value_M2, Kp_2, Kd_2, Ki_2,ending_encoder_count_M2);
  //add speedM2, encvaluem2, endingencodervaluem2, kpm2,kdm2,kim2,
  M1_set_speed(speedM1);
  //M2_set_speed(speedM1); //if you want both motors to be controlled by same pid loop
  M2_set_speed(speedM2); //now controlled by its own pid loop
}

//turn left 

 void turn_right_90()
 {
  Encoder enc1(M1_ENC_A, M1_ENC_B); //built in function in encoder.h library sets pins to be read
  Encoder enc2(M2_ENC_A, M2_ENC_B);
  int set_90_count = 280; // -250positive value for turning both wheels one cw the other ccw,approximate count for turning 90 degrees to the left
  //Either Set encoder to zero or add desired to encodervalue to set desired
  int tempvalue =0;
  //ramp input for set_90_count 
 
  //starting encoder values
   long enc1_value = enc1.read();
   long enc2_value = enc2.read();
   long ending_encoder_count_M1= enc1_value;// made long instead of int
   long ending_encoder_count_M2 = enc2_value; 
  while(tempvalue < set_90_count ){ //< for turn right
      ending_encoder_count_M1 = enc1_value + tempvalue; //-20, -40, -60, -80, -100.. -200 set90 count
      ending_encoder_count_M2 = enc2_value + tempvalue;
      tempvalue = 0.9*set_90_count + tempvalue; //both have same encoder reference input value, its possible I will need to make a sepearte 1 for each motor

      set_both_motors_speed(ending_encoder_count_M1, ending_encoder_count_M2, enc1_value, enc2_value, Kp_1, Kd_1, Ki_1, Kp_2, Kd_2, Ki_2); //ending encoder count instead of enc1_value
        enc1_value = enc1.read();
        enc2_value = enc2.read();

    }
      bool cont = true;
      while(cont) {
          
          set_both_motors_speed(ending_encoder_count_M1, ending_encoder_count_M2, enc1_value, enc2_value,Kp_1, Kd_1, Ki_1, Kp_2, Kd_2, Ki_2); //ending encoder count instead of enc1_value
        
           // if(enc1_value == ending_encoder_count_M1)
            if (abs(enc1_value - ending_encoder_count_M1) < 7)
           
            {
              Serial.print("exitclause");
              Serial.println(); 
              M1_set_speed(0);
              M2_set_speed(0);
              cont =false;
            }
            // M1_stop();
            // M2_stop();
            // delay(3000);
            enc1_value = enc1.read();
            enc2_value = enc2.read();
       
        }
 }

 void turn_left_90()
 {
  Encoder enc1(M1_ENC_A, M1_ENC_B); //built in function in encoder.h library sets pins to be read
  Encoder enc2(M2_ENC_A, M2_ENC_B);
  int set_90_count = -250; // 260positive value for turning both wheels one cw the other ccw,approximate count for turning 90 degrees to the left
  //Either Set encoder to zero or add desired to encodervalue to set desired
  int tempvalue =0;
  //ramp input for set_90_count 
 
  //starting encoder values
   long enc1_value = enc1.read();
   long enc2_value = enc2.read();
    long ending_encoder_count_M1= enc1_value;// made long instead of int
    long ending_encoder_count_M2 = enc2_value; 

  while(tempvalue > set_90_count ){
      
      ending_encoder_count_M1 = enc1_value + tempvalue; //-20, -40, -60, -80, -100.. -200 set90 count
      ending_encoder_count_M2 = enc2_value + tempvalue;
      tempvalue = 0.9*set_90_count + tempvalue; //both have same encoder reference input value, its possible I will need to make a sepearte 1 for each motor

      
      set_both_motors_speed(ending_encoder_count_M1, ending_encoder_count_M2, enc1_value, enc2_value, Kp_1, Kd_1, Ki_1, Kp_2, Kd_2, Ki_2); //ending encoder count instead of enc1_value
        enc1_value = enc1.read();
        enc2_value = enc2.read();
    }
      bool cont = true;
      while(cont) {
          
          set_both_motors_speed(ending_encoder_count_M1, ending_encoder_count_M2, enc1_value, enc2_value,Kp_1, Kd_1, Ki_1, Kp_2, Kd_2, Ki_2); //ending encoder count instead of enc1_value
     
            if (abs(enc1_value- ending_encoder_count_M1) < 7)
            {
              Serial.print("exitclause");
              Serial.println(); 
              M1_set_speed(0);
              M2_set_speed(0);
              cont =false;
            }
          
            enc1_value = enc1.read();
            enc2_value = enc2.read();
        }
 }

void setup() {

  Serial.begin(115200);
  Serial.println("Before The connection");
  WiFi.mode(WIFI_STA); //Optional
  WiFi.begin(ssid, password);
  Serial.println("\nConnecting");

  while(WiFi.status() != WL_CONNECTED){
      Serial.print(".");
      delay(100);
   
  }
  clientSocket = socket(AF_INET, SOCK_STREAM, 0);
      Serial.println("\nConnected to the WiFi network");
      Serial.print("Local ESP32 IP: ");
      Serial.println(WiFi.localIP());
      
      // specifying address 
      sockaddr_in serverAddress; 
      serverAddress.sin_family = AF_INET; 
      serverAddress.sin_port = htons(8000); 
      serverAddress.sin_addr.s_addr = inet_addr("172.20.10.5"); // from ifconfig on nvidia
      
      connect(clientSocket, (struct sockaddr*)&serverAddress, 
              sizeof(serverAddress)); 
      
     

  // Stop the right motor by setting pin 14 low
  // this pin floats high or is pulled
  // high during the bootloader phase for some reason
  pinMode(14, OUTPUT);
  digitalWrite(14, LOW);
  delay(100);
 
// set up motors

  // we can control the pwm of indepenent motors 
  // esp32 has 16 possible channels to generate independent waveforms // create pwm sources
  ledcSetup(M1_IN_1_CHANNEL, freq, resolution); //configure LED PWM functionalities
  ledcSetup(M1_IN_2_CHANNEL, freq, resolution);
  ledcSetup(M2_IN_1_CHANNEL, freq, resolution);
  ledcSetup(M2_IN_2_CHANNEL, freq, resolution);
 //attach the channel (pwm sources) to the GPIO to be controlled these are the output pins
  ledcAttachPin(M1_IN_1, M1_IN_1_CHANNEL); //ledcattachpin(GPIO,CHannel)
  ledcAttachPin(M1_IN_2, M1_IN_2_CHANNEL);
  ledcAttachPin(M2_IN_1, M2_IN_1_CHANNEL);
  ledcAttachPin(M2_IN_2, M2_IN_2_CHANNEL);
}


void loop() {
 
  // delay (3000);// delay start
  // Serial.print(" turn right 90");
  // Serial.println();
  // turn_right_90();
  // Serial.print(" turn left 90");
  // Serial.println();
  // delay(3000);
  // turn_left_90();

     const char* message = "Hello, server!"; //14 bytes and null character because of double qhoes
     
      send(clientSocket, message, strlen(message), 0); 
      delay(1000);
      close(clientSocket); 



  exit(0);
}
