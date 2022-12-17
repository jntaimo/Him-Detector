#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL343.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Adafruit_DRV2605.h"
//VIBRATION START
Adafruit_DRV2605 drv;
#define VIBRATION_EFFECT 52
//VIBRATION STOP
//DISPLAY START
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
//DISPLAY STOP

//ACCELEROMETER START
/* Assign a unique ID to this sensor at the same time */
/* Uncomment following line for default Wire bus      */
Adafruit_ADXL343 accel = Adafruit_ADXL343(12345);
float accelMag = 9;
float filtAccelMag = 9;
float alpha = 0.05;
float accelMoveThreshold = 10;
long accelReadDelay = 10; //milliseconds
long accelPrintDelay = 100;
long lastAccelRead = 0;
//ACCELEROMETER START

enum mode {
  HIMOTHY,
  CHILL
};

//default the current status to chill
mode currentStatus = CHILL;

void accelerometerSetup() {

  /* Initialise the sensor */
  if(!accel.begin())
  {
    /* There was a problem detecting the ADXL343 ... check your connections */
    Serial.println("Ooops, no ADXL343 detected ... Check your wiring!");
    while(1);
  }
  accel.setRange(ADXL343_RANGE_8_G);

}

void vibrationSetup(){
  if (! drv.begin()) {
    Serial.println("Could not find DRV2605");
    while (1) delay(10);
  }
  drv.selectLibrary(1);
  // I2C trigger by sending 'go' command 
  // default, internal trigger when sending GO command
  drv.setMode(DRV2605_MODE_INTTRIG); 
}
void setup(void)
{
  Serial.begin(115200);
  while (!Serial);
  accelerometerSetup();
  vibrationSetup();
}


void loop(void)
{
  //read acceleration at a constant interval
  if (millis() - lastAccelRead> accelReadDelay) {
  /* Get a new sensor event */
  sensors_event_t event;
  accel.getEvent(&event);
  //get new acceleration reading
  accelMag = sqrt(sq(event.acceleration.x) + sq(event.acceleration.y) + sq(event.acceleration.z));
  //smooth the data
  filtAccelMag = alpha*accelMag + (1-alpha)*filtAccelMag;
  lastAccelRead = millis();
  }
  //if you shake the device, switch to himothy
  if (filtAccelMag > accelMoveThreshold) currentStatus = HIMOTHY;

  switch(currentStatus){
      case CHILL:
        Serial.println("Chillin Like a Villian");

        break;
      case HIMOTHY:
        
        Serial.println("You are he");
          // set the effect to play
        drv.setWaveform(0, VIBRATION_EFFECT);  // play effect 
        drv.setWaveform(1, 0);       // end waveform
        // play the effect!
        drv.go();
        delay(500);
        currentStatus = CHILL;
        break;
  }

}