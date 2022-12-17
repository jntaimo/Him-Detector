#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL343.h>
#include <Adafruit_GFX.h>

/* Assign a unique ID to this sensor at the same time */
/* Uncomment following line for default Wire bus      */
Adafruit_ADXL343 accel = Adafruit_ADXL343(12345);
enum mode {
  HIMOTHY,
  CHILL
};

float accelMag = 9;
float filtAccelMag = 9;
float alpha = 0.05;
float accelMoveThreshold = 10;
long accelReadDelay = 10; //milliseconds
long accelPrintDelay = 100;
long lastAccelRead = 0;

//default the current status to chill
  mode currentStatus = CHILL;
void setup(void)
{
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Accelerometer Test"); Serial.println("");

  /* Initialise the sensor */
  if(!accel.begin())
  {
    /* There was a problem detecting the ADXL343 ... check your connections */
    Serial.println("Ooops, no ADXL343 detected ... Check your wiring!");
    while(1);
  }
  
  accel.setRange(ADXL343_RANGE_8_G);
  
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
        currentStatus = CHILL;
        break;
  }

}