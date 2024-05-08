#include "SectionFunctions.h"


//Defining Global Encoders
void setupMotors(){
  const int freq = 5000;
  const int resolution = 10;
  ledcSetup(M1_IN_1_CHANNEL, freq, resolution);
  ledcSetup(M1_IN_2_CHANNEL, freq, resolution);
  ledcSetup(M2_IN_1_CHANNEL, freq, resolution);
  ledcSetup(M2_IN_2_CHANNEL, freq, resolution);

  ledcAttachPin(M1_IN_1, M1_IN_1_CHANNEL);
  ledcAttachPin(M1_IN_2, M1_IN_2_CHANNEL);
  ledcAttachPin(M2_IN_1, M2_IN_1_CHANNEL);
  ledcAttachPin(M2_IN_2, M2_IN_2_CHANNEL);

  pinMode(M1_I_SENSE, INPUT);
  pinMode(M2_I_SENSE, INPUT);
}

void M1_backward(int pwm_value) {
  ledcWrite(M1_IN_1_CHANNEL, pwm_value);
  ledcWrite(M1_IN_2_CHANNEL, 0);
}

void M1_forward(int pwm_value) {
  
  ledcWrite(M1_IN_1_CHANNEL, 0);
  ledcWrite(M1_IN_2_CHANNEL, pwm_value);
}

void M1_stop() {
  ledcWrite(M1_IN_1_CHANNEL, PWM_VALUE);
  ledcWrite(M1_IN_2_CHANNEL, PWM_VALUE);
}

void M2_backward(int pwm_value) {
  ledcWrite(M2_IN_1_CHANNEL, pwm_value);
  ledcWrite(M2_IN_2_CHANNEL, 0);
}

void M2_forward(int pwm_value) {
  
  ledcWrite(M2_IN_1_CHANNEL, 0);
  ledcWrite(M2_IN_2_CHANNEL, pwm_value);
}

void M2_stop() {
  ledcWrite(M2_IN_1_CHANNEL, PWM_VALUE);
  ledcWrite(M2_IN_2_CHANNEL, PWM_VALUE);
}

// Move Forward in Straight-Line, and Consistent Turn PID Control Loops
void move_forward(int distance, Encoder & enc1, Encoder & enc2) {
  enc1.readAndReset();
  enc2.readAndReset();
  long enc1_value = -1; // Initialize as -1 to Handle Indefinite Case
  long enc2_value = abs(enc2.read());
  long error = 0; // PID Control Values
  long last_error = 0;
  long total_error = 0;

  float Kp_s = 10; 
  float Kd_s =  0; // 1, 0.5, 0  On Operational Heltec
  float Ki_s = 0;
  
  while((enc1_value <distance)) {
    enc1_value = abs(enc1.read());     
    enc2_value = abs(enc2.read());
    error = enc1_value - enc2_value; // Left - Right
    // With a positive error the left motor is moving faster than the right motor so veering right
    // With a negative error the right motor is moving faster than the left motor so veering left
    int pid_value = Kp_s*error + Kd_s*(error-last_error) + Ki_s*total_error;
    last_error = error;
    total_error += error; // questionable about including Ki, Seems to have negative effects
    // Hence, If error is negative, right motor will decrease, with positive, left motor will decrease
  
    int left_motor = base_pid - pid_value; // Ensure All Robots Have Same Motor Configuration Or Else Divergence Will Occur
    int right_motor = base_pid + pid_value;
    M1_forward(left_motor); // M1 is Left Motor, M2 is Right Motor, Rename/Recomment
    M2_forward(right_motor);
    if (distance == 0) { // When distance set to 0, move_forward toggled to go forever.
      enc1_value = -1;
    }
  }
  M1_stop();
  M2_stop();
}

// 225 ~ 90 Degrees to the right
// 500 ~ 180 Degrees to the right
// 225 ~ 90 Degrees to the left
// 480 ~ 180 Degrees to the right
// Right = true, left = false, enumerate in .h file
void turnConsistent(int angle, bool direction, Encoder& enc1, Encoder& enc2) {
  enc1.readAndReset();
  enc2.readAndReset();
  long enc1_value = abs(enc1.read());
  long enc2_value = abs(enc2.read());
  long error = 0;
  long last_error = 0;
  long total_error = 0;
  int base_pid1 = 375; 
  // Kickstart 100 ms delay
  if (direction) {
    M1_forward(base_pid1);
    M2_backward(base_pid1);
  } else {
    M1_backward(base_pid1);
    M2_forward(base_pid1);
  }
  delay(100);
  float Kp_s = 1; // Adjust based on whatever surface you are working on 
  float Kd_s =  0.5;
  float Ki_s = 0.05;
  while(enc1_value<angle) { // Averaging the two values
    enc1_value = abs(enc1.read());     
    enc2_value = abs(enc2.read()); 
    error = enc1_value - enc2_value;
    int pid_value = Kp_s*error + Kd_s*(error-last_error) + Ki_s*total_error; // Positive Error = right moving faster, Negative = left
    last_error = error;
    //total_error += error; // questionable about including Ki, Seems to have negative effects
    // Hence, If error is negative, right motor will decrease, with positive, left motor will decrease
    int right_motor = base_pid1 - pid_value;
    int left_motor = base_pid1 + pid_value;
    if (direction) {
      M1_forward(right_motor);
      M2_backward(left_motor);
  } else { // Turning Left
      M1_backward(right_motor);
      M2_forward(left_motor);
  }
  }
  M1_stop();
  M2_stop();
  // Delay to prevent Motor Stop SHA pin Bug
  
}


 

