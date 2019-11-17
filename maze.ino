#include <MeMCore.h>
#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>

MeUltrasonicSensor ultrasonic_3(3);
MeDCMotor motor_9(9);
MeDCMotor motor_10(10);
MeIR ir; 

int FORWARD = 0;
int RIGHT = 1;
int LEFT = 2;
int BACKWARD = 3;
int movementLeft[4] = {1, 1, -1, -1};
int movementRight[4] = {1, -1, 1, -1};

int FORWARD_KEY = 70;
int RIGHT_KEY = 67;
int LEFT_KEY = 68;
int BACKWARD_KEY = 21;
int START_KEY = 12;
int STOP_KEY = 22;

void move(int direction, int speed) {
  int leftSpeed = speed * movementLeft[direction];
  int rightSpeed = speed * movementRight[direction];
  
  motor_9.run((9) == M1 ? -(leftSpeed) : (leftSpeed));
  motor_10.run((10) == M1 ? -(rightSpeed) : (rightSpeed));
}

void stopShortly() {
  move(FORWARD, 0);
  delay(100);
}

void turn(int direction) {
  if(direction != 0 && direction != 3)
  {
      move(direction, 90);
      delay(825);
  }
  stopShortly();
}

void runByIRValue() {
  if(ir.keyPressed(FORWARD_KEY))
    move(FORWARD, 255);

  if(ir.keyPressed(BACKWARD_KEY))
    move(BACKWARD, 255);

  if(ir.keyPressed(RIGHT_KEY))
    move(RIGHT, 255);

  if(ir.keyPressed(LEFT_KEY))
    move(LEFT, 255);

  move(FORWARD, 0);
}

boolean stopped = true;
boolean isStopped(){
  if (ir.keyPressed(STOP_KEY))
    stopped = true;
  else if (ir.keyPressed(START_KEY))
    stopped = false;

  return stopped;
}

boolean solveByStep(int step) {
  if(isStopped())
    return true;
  
  long unsigned startTime;
  for (int dir = 0; dir < 3; dir++) 
  {
    turn(dir);
    
    startTime = millis();
    while (ultrasonic_3.distanceCm() >= 5.5)
    {
      move(FORWARD, 150);
      if(isStopped())
        break;
    }
    long unsigned timeTravelled = millis() - startTime;
    stopShortly();
    
    if (isStopped() || ((timeTravelled > 500) && (solveByStep(step+1))))
      return true;
    else
    {
      startTime = millis();
      while ((millis() - startTime) < timeTravelled)
      {
        move(BACKWARD, 150);
        if(isStopped())
          break;
      }
      stopShortly();
      turn(3-dir);
    }
  }
  return false;
}

void solveMaze(){
  Serial.println("Solving maze...");
  solveByStep(0);
}


void setup()
{
    ir.begin();
    Serial.begin(9600);
    Serial.println("mBot ready");
}

void loop()
{
  if(!isStopped())
  {
    solveMaze();
  }
  runByIRValue();
}
