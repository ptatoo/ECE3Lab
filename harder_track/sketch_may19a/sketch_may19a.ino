#include <ECE3.h>
//HARDER TRACK

uint16_t sensorValues[8];
//Sensor Fusion Values
int minValues[] = {805, 664, 665, 573, 664, 687, 735, 804};
int maxValues[] = {2500, 2500, 2500, 1514, 2107, 2500, 2500, 2500};
int rightBiasedWeights[] = {-18, -16, -14, -8, 8, 9, 10, 11};
int leftBiasedWeights[] = {-11, -10, -9, -8, 8, 14, 16, 18};

int divideNum = 8;

//motor values
int initialSpeed = 40;
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
int timer = 0;
bool turned = false;

//hehe red amogus sus :)
void setup()
{
  timer = 0;
  barMissTimer = 0;
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
    turned = true;
    timer = 0;
    //make 180
    if (turnCounter > 1) {
      analogWrite(left_pwm_pin, 0);
      analogWrite(right_pwm_pin, 0);
      return;
    } else {
      digitalWrite(right_dir_pin, LOW);
      digitalWrite(left_dir_pin,HIGH);
      analogWrite(left_pwm_pin, 0);
      analogWrite(right_pwm_pin, 0);
      delay(300);
      
      analogWrite(left_pwm_pin, 100);
      analogWrite(right_pwm_pin, 100);
      delay(560);
      digitalWrite(left_dir_pin,LOW);
      analogWrite(left_pwm_pin, 0);
      analogWrite(right_pwm_pin, 0);
      delay(300);
    }
  }
  //End Sleep
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
    //Left vs Right Biased Weights
    if (timer < 50 && !turned) {
      weightedSum = 0;
      digitalWrite(GREEN_LED, HIGH);
      digitalWrite(RED_LED, HIGH);
    } else if (!turned && (timer < 250 || timer > 650)) {
      weightedSum += normalizedValue * rightBiasedWeights[i] / divideNum;
      digitalWrite(BLUE_LED, LOW);
      digitalWrite(GREEN_LED, LOW);
      digitalWrite(RED_LED, HIGH);
    } else if (turned && (timer > 200 && timer < 600)) {
      weightedSum += normalizedValue * rightBiasedWeights[i] / divideNum;
      digitalWrite(BLUE_LED, LOW);
      digitalWrite(GREEN_LED, LOW);
      digitalWrite(RED_LED, HIGH);
    } else {
      weightedSum += normalizedValue * leftBiasedWeights[i] / divideNum;
      digitalWrite(RED_LED, LOW);
      digitalWrite(BLUE_LED, HIGH);
      digitalWrite(GREEN_LED, LOW);
    }
    turnAroundSum += normalizedValue / divideNum;
  }
  int PIDNumber = - (weightedSum) * 0.02 - (weightedSum - prevWeight) * 0.2;  

  leftSpeed = initialSpeed + (PIDNumber);
  rightSpeed = initialSpeed - (PIDNumber);

  //Bar Counter
  if (abs(turnAroundSum) >= 600) {
    if (barMissTimer > 750) {
      barCounter++;
    }
  } else {
    barCounter = 0;
  }  
  barMissTimer++;
  timer++;

  if (leftSpeed < 0) {
    leftSpeed = -1 * leftSpeed;
    digitalWrite(left_dir_pin,HIGH);
  } else {
    digitalWrite(left_dir_pin,LOW);
  }
  
  if (rightSpeed < 0) {
    rightSpeed = -1 * rightSpeed;
    digitalWrite(right_dir_pin,HIGH);
  } else {
    digitalWrite(right_dir_pin,LOW);
  }
}


