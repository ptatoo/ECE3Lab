#include <ECE3.h>
#include "PIDController.h"
String dummy;

uint16_t sensorValues[8];
void donut();
//Sensor Fusion Values
int minValues[] = {805, 664, 665, 573, 664, 687, 735, 804};
int maxValues[] = {2500, 2500, 2500, 1514, 2107, 2500, 2500, 2500};
int weights[] = {-15, -14, -12, -8, 8, 12, 14, 15};
int divideNum = 8;
int number_samples = 5;

//motor values
int motorNotSleep = false;
int initialSpeed = 100;
int minSpeed = 0;
int maxSpeed = 150;

int leftSpeed = initialSpeed;
int rightSpeed = initialSpeed;

//motor pin values
const int left_nslp_pin=31;
const int left_dir_pin=29;
const int left_pwm_pin=40;
const int right_nslp_pin = 11;
const int right_dir_pin = 30;
const int right_pwm_pin = 39;

PIDController leftPID = PIDController(0.01, 0.000001, 0.0001, 0);
PIDController rightPID = PIDController(1, 0, 0, 0);

//hehe red amogus sus :)
void setup()
{
  ECE3_Init();
  Serial.begin(9600); // set the data rate in bits per second for serial data transmission
  delay(1000);
 
  pinMode(left_nslp_pin,OUTPUT);
  pinMode(left_dir_pin,OUTPUT);
  pinMode(left_pwm_pin,OUTPUT);

  digitalWrite(left_dir_pin,LOW);
  digitalWrite(left_nslp_pin,HIGH);

  pinMode(right_nslp_pin,OUTPUT);
  pinMode(right_dir_pin,OUTPUT);
  pinMode(right_pwm_pin,OUTPUT);

  digitalWrite(right_dir_pin,LOW);
  digitalWrite(right_nslp_pin,HIGH);
}

void loop()
{


  // analogWrite(left_pwm_pin, leftSpeed);
  // analogWrite(right_pwm_pin, rightSpeed);
  int summed_values[8] = {0};
  int normalizedValues[8] = {0};
  int weightedSum = 0;

  // Take the average of 5 consecutive values for each sensor
  for (int j = 0; j < number_samples; j++){
    // Read raw sensor values
    ECE3_read_IR(sensorValues);

    // Add the current sensor values using a for loop
    for (unsigned char i = 0; i < 8; i++)
    {
      summed_values[i] += sensorValues[i];
    }
  }
  int turnAroundSum;
  // Print average values (average value = summed_values / number_samples
  for (unsigned char i = 0; i < 8; i++) {
    int avg = summed_values[i] / number_samples;
    int normalizedValue= (summed_values[i] / number_samples - minValues[i])*(1.0)/(maxValues[i] - minValues[i]) * 1000.0;
    weightedSum += normalizedValue * weights[i] / divideNum;
    turnAroundSum += normalizedValue / divideNum;

    //Serial.print(normalizedValue);
    //Serial.print('\t');
  }
  Serial.print(turnAroundSum);
  Serial.print(" | ");
  Serial.print(weightedSum);
  Serial.print(" | ");
  int  PIDNumber = leftPID.calculate(weightedSum);
Serial.print(PIDNumber);
  Serial.print(" | ");
  Serial.print(leftSpeed);
  Serial.print(" | ");
  Serial.print(rightSpeed);
  Serial.println();

  leftSpeed = initialSpeed + (PIDNumber);
  rightSpeed = initialSpeed - (PIDNumber);
  if (turnAroundSum >= 1000)
  {
    donut(turnAroundSum);
  }
  


  leftSpeed  = constrain(leftSpeed, minSpeed, maxSpeed);
  rightSpeed = constrain(rightSpeed, minSpeed, maxSpeed);
  delay(5);

}

void donut(int &turnAroundSum)
{
  while(turnAroundSum >= 1000)
  {
  analogWrite(left_pwm_pin, 90);
  analogWrite(right_dir_pin , HIGH);
  analogWrite(right_pwm_pin, 90);
  delay(10);
  }
  analogWrite(right_dir_pin , LOW);

}
