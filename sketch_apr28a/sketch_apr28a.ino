#include <ECE3.h>
//EASIER TRACK

uint16_t sensorValues[8];
//Sensor Fusion Values
int minValues[] = {805, 664, 665, 573, 664, 687, 735, 804};
int maxValues[] = {2500, 2500, 2500, 1514, 2107, 2500, 2500, 2500};
int weights[] = {-15, -14, -12, -8, 8, 12, 14, 15};
int divideNum = 8;

//motor values
int initialSpeed = 200;
int leftSpeed = initialSpeed;
int rightSpeed = initialSpeed;

int weightedSum = 0;
int prevWeight = 0;

//motor pin values
const int left_nslp_pin=31;
const int left_dir_pin=29;
const int left_pwm_pin=40;
const int right_nslp_pin = 11;
const int right_dir_pin = 30;
const int right_pwm_pin = 39;

//PID
//TODO MOVE CONST UP HERE
int barCounter = 0;
int turnCounter = 0;
int barMissTimer = 0;

//hehe red amogus sus :)
void setup()
{
  ECE3_Init();
  Serial.begin(9600); // set the data rate in bits per second for serial data transmission
  delay(2000);
 
  pinMode(left_nslp_pin,OUTPUT);
  pinMode(left_dir_pin,OUTPUT);
  pinMode(left_pwm_pin,OUTPUT);


  pinMode(right_nslp_pin,OUTPUT);
  pinMode(right_dir_pin,OUTPUT);
  pinMode(right_pwm_pin,OUTPUT);

  digitalWrite(left_dir_pin,LOW);
  digitalWrite(left_nslp_pin,HIGH);
  digitalWrite(right_dir_pin,LOW);
  digitalWrite(right_nslp_pin,HIGH);
}

void loop()
{
  if (barCounter > 1) {
    barCounter = 0;
    turnCounter++;
    barMissTimer = 0;
    if (turnCounter > 1) {
      analogWrite(left_pwm_pin, 0);
      analogWrite(right_pwm_pin, 0);
      return;
    } else {
      digitalWrite(left_dir_pin,HIGH);
      analogWrite(left_pwm_pin, 0);
      analogWrite(right_pwm_pin, 0);
      delay(75);
      
      analogWrite(left_pwm_pin, 255);
      analogWrite(right_pwm_pin, 255);
      delay(345 - 67);
      digitalWrite(left_dir_pin,LOW);
      analogWrite(left_pwm_pin, 0);
      analogWrite(right_pwm_pin, 0);
      delay(75);
    }
  }
  if (turnCounter > 1) {
    analogWrite(left_pwm_pin, 0);
    analogWrite(right_pwm_pin, 0);
    return;
  } else {
    analogWrite(left_pwm_pin, leftSpeed);
    analogWrite(right_pwm_pin, rightSpeed);
  }
  
  prevWeight = weightedSum;
  weightedSum = 0;

  ECE3_read_IR(sensorValues);
  int turnAroundSum = 0; //detects big black bar

  for (unsigned char i = 0; i < 8; i++) {
    int normalizedValue = (sensorValues[i] - minValues[i])*(1.0)/(maxValues[i] - minValues[i]) * 1000.0;
    weightedSum += normalizedValue * weights[i] / divideNum;
    turnAroundSum += normalizedValue / divideNum;
  }
  int PIDNumber = - (weightedSum) * 0.09 - (weightedSum - prevWeight) * 0.9;  

  leftSpeed = initialSpeed + (PIDNumber);
  rightSpeed = initialSpeed - (PIDNumber);
  //Serial.println(turnAroundSum);
  if (abs(turnAroundSum) >= 420) {
    if (barMissTimer > 600) {
      barCounter++;
    }
  } else {
    barCounter = 0;
  }  
  barMissTimer++;
  if (initialSpeed < 255) {
    initialSpeed++;
  }

  leftSpeed = constrain(leftSpeed, 0, 255);  
  rightSpeed = constrain(rightSpeed, 0, 255);
}