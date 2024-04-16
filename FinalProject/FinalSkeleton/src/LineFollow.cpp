#include "LineFollow.h"
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
    if (adc1_buf[i]<670) {
      lineArray[2*i] = 1; 
    } else {
      lineArray[2*i] = 0;
    }

    if (i<6) {
      if (adc2_buf[i]<670){
        lineArray[2*i+1] = 1;
      } else {
        lineArray[2*i+1] = 0;
      }
    }
    
  }
  Serial.println();
  
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

int lineFollowExit() {
    float Kp = 20; // 20, 11, 0 at 325 straight on soft surface
    float Kd = 11;
    float Ki = 0;
    int flag_vertex = 0;
    float mid = 6.5;
    int base_pid = 325;
    float previousPosition = 6;
    float error;
    float last_error;
    float total_error;
    while (true) {
        LineVal rVal = getPosition(previousPosition);
        // Edge Handling (Reached a Non-Cube Vertex)
        if (rVal.lineValues[0] == 1 && rVal.lineValues[12] == 1) {
            flag_vertex = 1; // Nvidia Send Signal
            M1_stop();
            M2_stop();
            return 0;
        }
        // Continouse LineFollow
        if( flag_vertex == 0 ) {
            float pos = rVal.pos;
            previousPosition = pos;

            error = pos - mid;
            total_error += error;
            int pid_value = Kp*error + Kd*(error-last_error) + Ki*total_error;
            
            int right_motor = base_pid + pid_value;
            int left_motor = base_pid - pid_value;
            
            M1_forward(left_motor);
            M2_forward(right_motor);

            last_error = error;
        } 

    }

}