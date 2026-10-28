#include <Arduino.h>
#include <Wire.h>
#include <MPU6050.h>
#include "I2Cdev.h"
#include "BleMouse.h"

// Defining pins for mouse buttons, the bluetooth mouse, gyroscopic vectors, mouse sensitivy, mouse movements and their smoothed counterparts.
#define LEFT_BTN 5
#define RIGHT_BTN 4

MPU6050 mpu;
BleMouse bleMouse;
int16_t gx, gy, gz;

float sensitivity = 0.003;
float dx {}, dy {};
float smoothed_dx {}, smoothed_dy {}; 


void setup() {
  //Start serial and I2C communication
  Serial.begin(115200);
  Wire.begin(21, 22);
  Wire.setClock(100000);

  //Define mouse input pins using internal pullup resistor. Buttons must be connected to ground.
  pinMode(LEFT_BTN, INPUT_PULLUP);
  pinMode(RIGHT_BTN, INPUT_PULLUP);
  
  bleMouse.begin();
  
  while (!Serial)
    delay(10);
  
  //Test to see if MPU6050 connects
  Serial.println("MPU6050 test!");
  mpu.initialize();
  Serial.println(F("Testing MPU6050 connection..."));
  if(mpu.testConnection() == false){
    Serial.println("MPU6050 connection failed");
    while(true);
  }
  else {
    Serial.println("MPU6050 connection successful");
  }

}

void loop() {

  //Test if mouse buttons are pressed, otherwise release them.
  bool leftPressed = (digitalRead(LEFT_BTN) == LOW);
  bool rightPressed = (digitalRead(RIGHT_BTN) == LOW);
  if (leftPressed){
    bleMouse.press(MOUSE_LEFT);
  }
  else{
    bleMouse.release(MOUSE_LEFT);
  }

  if (rightPressed){
    bleMouse.press(MOUSE_RIGHT);
  }
  else{
    bleMouse.release(MOUSE_RIGHT);
  }


  // Get gyroscopic data
  mpu.getRotation(&gx, &gy, &gz);

  //Choose which gyroscopic vectors to use according to MPU-6050 orientation on board.
  dx = -gz * sensitivity;
  dy = gx * sensitivity;

  //Simple Highpass filter and smoothening of output to minimize jitter.
  if (abs(dx) < 0.5) dx = 0;
  if (abs(dy) < 0.5) dy = 0;

  smoothed_dx = smoothed_dx * 0.7 + dx *0.3;
  smoothed_dy = smoothed_dy * 0.7 + dy *0.3;

  //Constrains movement of mouse to remove big cursor jumps.
  int move_x = constrain((int)smoothed_dx, -20, 20);
  int move_y = constrain((int)smoothed_dy, -20, 20);

  bleMouse.move(move_x, move_y);

  // Print raw gyroscopic data for debug purposes
  Serial.print("g:\t");
  Serial.print(gx); Serial.print("\t");
  Serial.print(gy); Serial.print("\t");
  Serial.println(gz);
  delay(10);
}
 

