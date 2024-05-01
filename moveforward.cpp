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
