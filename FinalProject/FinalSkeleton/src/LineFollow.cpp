#include "LineFollow.h"
#include "Networking.h"
Adafruit_MCP3008 adc1;
Adafruit_MCP3008 adc2;

void setupLineFollow() {
  adc1.begin(ADC_1_CS);  
  adc2.begin(ADC_2_CS);
}
uint8_t* readADCConvert() {
  static uint8_t lineArray[13];
  int adc1_buf[8];
  int adc2_buf[8];

  for (int i = 0; i < 8; i++) {
    adc1_buf[i] = adc1.readADC(i);
    adc2_buf[i] = adc2.readADC(i);
  }

  for (int i = 0; i < 7; i++) {
    if (adc1_buf[i]<500) {
      lineArray[2*i] = 1; 
    } else {
      lineArray[2*i] = 0;
    }

    if (i<6) {
      if (adc2_buf[i]<500){
        lineArray[2*i+1] = 1;
      } else {
        lineArray[2*i+1] = 0;
      }
      //Serial.print(adc2_buf[i]);
      //Serial.print("   ");
      //delay(100);
      
    } 
  }
  
   //Serial.println();
  
  return lineArray;
}


LineVal getPosition(float previousPosition) {
  uint8_t* lineArray = readADCConvert();
  float pos = 0;
  uint8_t white_count = 0;
  for (int i = 0; i < 13; i++) {
    if (lineArray[i] == 1) {
      pos += i;
      white_count+=1;
    
    } 
  }
  if (white_count == 0) {
    return {lineArray, previousPosition };
  }
  return {lineArray, pos/white_count};

}


// case_val: 0 = Continuous Line Follow Until Exit
// case_val: 1 = Interrupt Line Follow Eery 
// Definition: Line-Follow PID Control Loop, case = 0 -> Non-Exit Line-folow, 1 -> Detect Colored Block, 2-> Detect Obstruction
int lineFollowExit(int case_val, Encoder& enc1,Encoder& enc2) {

    M1_stop();
    M2_stop();
    float Kp = 20; // 20, 11, 0 at 325 straight on soft surface
    float Kd = 11;  // Move Locals to Global To reduce Memory Strain
    float Ki = 0;
    int flag_vertex = 0;
    float mid = 7.5;
    int base_pid1 = 350;
    float previousPosition = 7.5;
    float error = 0.0;
    float last_error = 0.0;
    float total_error = 0.0;
    auto start_time = std::chrono::steady_clock::now();
    auto current_time = std::chrono::steady_clock::now();
    int rightMVal = 0;
    int leftMVal = 0;
    int pid_valueLF = 0;
    //Serial.println("Following The Line");
    std::chrono::seconds interval(1);
    int check = 0;
    while (true) {
        LineVal rVal = getPosition(previousPosition);
        //current_time = std::chrono::steady_clock::now();
        //auto duration = std::chrono::duration_cast<std::chrono::seconds>(current_time-start_time);
        if (check>200){
        if (case_val == 1) {
          if (rVal.lineValues[0] == 1 || rVal.lineValues[12] == 1) {
          //Serial.println("Ending Line Follow");
          flag_vertex = 1; // Nvidia Send Signal
          M1_stop();
          M2_stop();
          move_forward(185,enc1,enc2); // 175 ~ Corresponds to 3-4 Inches (Center on Vertex)
          return 0;
          }    
        }  else {
        if (rVal.lineValues[0] == 1 && rVal.lineValues[12] == 1) {
          flag_vertex = 1; // Nvidia Send Signal
          M1_stop();
          M2_stop();
          delay(500);
          if (case_val!=0){
          move_forward(175,enc1,enc2); // 175 ~ Corresponds to 3-4 Inches (Center on Vertex)
          }
          return 0;
        }
        }
        }
        check ++;
        // Continual Adjustment
        float pos = rVal.pos;
        previousPosition = pos;

        error = pos - mid;
        total_error += error;
        pid_valueLF = Kp*error + Kd*(error-last_error) + Ki*total_error;
        
        
        rightMVal= base_pid1 - pid_valueLF;
        leftMVal= base_pid1 + pid_valueLF;

        M1_forward(rightMVal);
        M2_forward(leftMVal);
        last_error = error;
      

    }

    // 0 signifies edge succesfully reached, no cube encountered.
    return 0;
    }

  void move_forward_until_interrupt(Encoder& enc1, Encoder& enc2) {
    enc1.readAndReset();
    enc2.readAndReset();
    //Serial.println("After Motor Declaration");
    long enc1_value = abs(enc1.read()); // Initialize as -1 to Handle Indefinite Case
    long enc2_value = abs(enc2.read());
    long error = 0; // PID Control Values
    long last_error = 0;
    long total_error = 0;
    float Kp_s = 10; 
    float Kd_s =  0; // 1, 0.5, 0  On Operational Heltec
    float Ki_s = 0;
    int right_motor = 0;
    int left_motor = 0;
    while(true) {
      LineVal rVal = getPosition(0);
      if (rVal.lineValues[0]==1 && rVal.lineValues[12]==1){
        M1_stop();
        M2_stop();
        move_forward(500,enc1,enc2); // CAPSTONE COMPLETE
        return;
      }
      enc1_value = abs(enc1.read());     
      enc2_value = abs(enc2.read());
      error = enc1_value - enc2_value; // Left - Right
      // With a positive error the left motor is moving faster than the right motor so veering right
      // With a negative error the right motor is moving faster than the left motor so veering left
      int pid_value = Kp_s*error + Kd_s*(error-last_error) + Ki_s*total_error;
      last_error = error;
      total_error += error; // questionable about including Ki, Seems to have negative effects
      left_motor = base_pid - pid_value; // Ensure All Robots Have Same Motor Configuration Or Else Divergence Will Occur
      right_motor = base_pid + pid_value;
      M1_forward(left_motor); // M1 is Left Motor, M2 is Right Motor, Rename/Recomment
      M2_forward(right_motor);
    }
    M1_stop();
    M2_stop();
    right_motor = 0;
    left_motor = 0;
    //delay(1000); // Preventing SHA bug
  }
  

