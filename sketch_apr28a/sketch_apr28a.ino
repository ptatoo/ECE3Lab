#include <ECE3.h>
String dummy;

uint16_t sensorValues[8];


int minValues[] = {805, 664, 665, 573, 664, 687, 735, 804};
int maxValues[] = {2500, 2500, 2500, 1514, 2107, 2500, 2500, 2500};
int weights[] = {-15, -14, -12, -8, 8, 12, 14, 15};
int divideNum = 8;

int number_samples = 5;
bool print_directions = false;

//hehe red amogus sus :)
void setup()
{
  ECE3_Init();
  Serial.begin(9600); // set the data rate in bits per second for serial data transmission
  delay(1000);
}


void loop()
{
  //wait for user input
  
  int summed_values[8] = {0};
  int normalizedValues[8] = {0};
  float weightedSum = 0;

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

  // Print current position at start of each column
  Serial.print(current_position);
  Serial.print("\t");

  // Print average values (average value = summed_values / number_samples
  for (unsigned char i = 0; i < 8; i++) {
    int avg = summed_values[i] / number_samples;
    float normalizedValue= (summed_values[i] / number_samples - minValues[i])*(1.0)/(maxValues[i] - minValues[i]) * 1000.0;
    weightedSum += normalizedValue * weights[i] / divideNum;

    Serial.print(normalizedValue);
    Serial.print('\t');
  }
  //Serial.print(weightedSum);
  Serial.println();

  // Increment current position by the increment value
  delay(100);

}