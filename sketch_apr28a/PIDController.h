#ifndef PIDCONTROLLER_H
#define PIDCONTROLLER_H

#include <Time.h>

class PIDController {
    public:
        PIDController(double P, double I, double D, double setPoint) {
            kP = P;
            kI = I;
            kD = D;
            target = setPoint;
            prevError = 0;
            integral = 0;
            lastTime = millis();
        }

        int calculate(int value) {
            double now = millis();
            double dt = now - lastTime;

            double error = target - value;

            double P = kP * error;
            
            integral += error * dt;
            double I = kI * integral;

            double derivative = (error - prevError) / dt;
            double D = kD * derivative;

            prevError = error;
            lastTime = now;

            return P + I + D;
        }
    private:
        double kP, kI, kD, target, prevError, lastTime, integral;
};

#endif