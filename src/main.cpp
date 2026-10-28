#include <Arduino.h>
#include <Wire.h>
#include <MPU6050.h>
#include "I2Cdev.h"
#include "BleMouse.h"

// 

#define LEFT_BTN 5
#define RIGHT_BTN 4

MPU6050 mpu;
BleMouse bleMouse;
int16_t gx, gy, gz;
float vx, vy;

float sensitivity = 0.003;
float dx {}, dy {};
float smoothed_dx {}, smoothed_dy {}; 


void setup() {
  //Set ledPin as digital output pin
  Serial.begin(115200);
  Wire.begin(21, 22);
  Wire.setClock(100000);

  pinMode(LEFT_BTN, INPUT_PULLUP);
  pinMode(RIGHT_BTN, INPUT_PULLUP);
  
  bleMouse.begin();
  
  while (!Serial)
    delay(10);
  
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


  // Get sensor data
  mpu.getRotation(&gx, &gy, &gz);
  
  Serial.print("g:\t");
  Serial.print(gx); Serial.print("\t");
  Serial.print(gy); Serial.print("\t");
  Serial.println(gz);

  dx = -gz * sensitivity;
  dy = gx * sensitivity;

  //Simple Highpass filter
  if (abs(dx) < 0.5) dx = 0;
  if (abs(dy) < 0.5) dy = 0;

  smoothed_dx = smoothed_dx * 0.7 + dx *0.3;
  smoothed_dy = smoothed_dy * 0.7 + dy *0.3;

  int move_x = constrain((int)smoothed_dx, -20, 20);
  int move_y = constrain((int)smoothed_dy, -20, 20);

  bleMouse.move(move_x, move_y);


  // Old version
  // vx = (gz+300-120)/200;
  // vy = -(gx-100+120)/200;

  // bleMouse.move(-vx, -vy);

  delay(10);
}
 

