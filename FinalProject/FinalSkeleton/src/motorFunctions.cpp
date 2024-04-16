#include "motorFunctions.h"

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

void move_forward(int distance) {
  Encoder enc1(M1_ENC_A, M1_ENC_B); 
  Encoder enc2(M2_ENC_A, M2_ENC_B);
  long enc1_value = -1;
  long enc2_value = abs(enc2.read());
  long error = 0;
  long last_error = 0;
  long total_error = 0;
  // Kickstart 100 ms delay
  M1_forward(base_pid);
  M2_forward(base_pid);
  delay(100);
  float Kp_s = 1; 
  float Kd_s =  0.5;
  float Ki_s = 0;
  while(enc1_value<distance) {
    enc1_value = enc1.read();     
    enc2_value = abs(enc2.read()); 
    error = enc1_value - enc2_value;
    // With a positive error the left motor is moving faster than the right motor so veering right
    // With a negative error the right motor is moving faster than the left motor so veering left
    int pid_value = Kp_s*error + Kd_s*(error-last_error) + Ki_s*total_error;
    last_error = error;
    total_error += error; // questionable about including Ki, Seems to have negative effects
    // Hence, If error is negative, right motor will decrease, with positive, left motor will decrease
    int right_motor = base_pid + pid_value;
    int left_motor = base_pid - pid_value;
    M1_forward(left_motor);
    M2_forward(right_motor);
    if (distance == 0) { // When distance set to 0, move_forward toggled to go forever.
      enc1_value = -1;
    }
  }
  M1_stop();
  M2_stop();
  delay(500); // Preventing SHA bug
}

// 225 ~ 90 Degrees to the right
// 500 ~ 180 Degrees to the right
// 225 ~ 90 Degrees to the left
// 480 ~ 180 Degrees to the right
// Right = true, left = false, enumerate in .h file
void turnConsistent(int angle, bool direction) {
  Encoder enc1(M1_ENC_A, M1_ENC_B); 
  Encoder enc2(M2_ENC_A, M2_ENC_B);
  long enc1_value = abs(enc1.read());
  long enc2_value = abs(enc2.read());
  long error = 0;
  long last_error = 0;
  long total_error = 0;
  // Kickstart 100 ms delay
  if (direction) {
    M1_forward(base_pid);
    M2_backward(base_pid);
  } else {
    M1_backward(base_pid);
    M2_forward(base_pid);
  }
  delay(100);
  float Kp_s = 1; // Adjust based on whatever surface you are working on 
  float Kd_s =  0.5;
  float Ki_s = 0.05;
  enc1.write(0);
  enc2.write(0);
  while(enc1_value<angle) {
    enc1_value = abs(enc1.read());     
    enc2_value = abs(enc2.read()); 
    error = enc1_value - enc2_value;
    int pid_value = Kp_s*error + Kd_s*(error-last_error) + Ki_s*total_error; // Positive Error = right moving faster, Negative = left
    last_error = error;
    //total_error += error; // questionable about including Ki, Seems to have negative effects
    // Hence, If error is negative, right motor will decrease, with positive, left motor will decrease
    int right_motor = base_pid + pid_value;
    int left_motor = base_pid - pid_value;
    if (direction) {
      M1_forward(base_pid);
      M2_backward(base_pid);
  } else {
      M1_backward(base_pid);
      M2_forward(base_pid);
  }
  }
  M1_stop();
  M2_stop();
  delay(500);
  // Delay to prevent Motor Stop SHA pin Bug
}


 

