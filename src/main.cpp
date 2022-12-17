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
float alpha = 0.01;
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
void testdrawrect(void);
void testdrawcircle(void);
void scroll(void);
void testdrawline(void);

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

void displaySetup(){
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    while(1) delay(10); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);
  display.setCursor(0,0);
  display.println("~Shake me~");
  display.display();
  
}
void setup(void)
{
  Serial.begin(115200);
  //while (!Serial);
  accelerometerSetup();
  vibrationSetup();
  displaySetup();
}

long lastPrint = 0;
long printDelay = 250;

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
        if (millis() - lastPrint > printDelay){
          Serial.println("Chillin Like a Villian");
          lastPrint = millis();
        }   
        break;
      case HIMOTHY:
        Serial.println("You are he");

        display.clearDisplay();
        display.setCursor(0,0);
        display.setTextSize(2);
        display.println("Analysis..");
        display.println("Analysis..");
        display.println("Analysis..");
        display.display();
        scroll();
        delay(1000);

        display.clearDisplay();
        display.display();

        drv.setWaveform(0, 77);  // play effect 
        drv.setWaveform(1, 0);       // end waveform   // end waveform
        drv.go();

        testdrawrect();

        drv.setWaveform(0, 79);  // play effect 
        drv.setWaveform(1, 0);       // end waveform   // end waveform
        drv.go();
        
        testdrawcircle();
        drv.setWaveform(0, 47);  // play effect 
        drv.setWaveform(1, 0);       // end waveform   // end waveform
        drv.go();
        testdrawline();
        display.clearDisplay();
        display.setCursor(10,0);
        display.setTextSize(6);

        display.println(F("HIM"));
        display.setTextSize(2);
        display.println(F(" DETECTED!"));
        display.display();

        for (int i = 0; i < 12;i++ ){
          drv.setWaveform(0, VIBRATION_EFFECT);  // play effect 
          drv.setWaveform(1, 0);       // end waveform   // end waveform
          drv.go();

          
          display.invertDisplay(i%2);
          delay(300);
        }
        
        //revert back to a normal display
        display.invertDisplay(false);
        display.clearDisplay();
        display.display();
        delay(500);
        display.setCursor(0,0);
        display.setTextSize(2);
        display.println(F("Waiting \nfor \nHIM"));
        display.display();
        delay(100);
        lastPrint = millis();
        //reset acceleratio mag so it can't be triggered for a bit
        filtAccelMag = 0;
        currentStatus = CHILL;
        break;
  }
}

void testdrawrect(void) {
  //display.clearDisplay();

  for(int16_t i=0; i<display.height()/2; i+=2) {
    display.drawRect(i, i, display.width()-2*i, display.height()-2*i, SSD1306_WHITE);
    display.display(); // Update screen with each newly-drawn rectangle
    delay(5);
  }

  delay(500);
}
void testdrawcircle(void) {
  //display.clearDisplay();

  for(int16_t i=0; i<max(display.width(),display.height())/2; i+=2) {
    display.drawCircle(display.width()/2, display.height()/2, i, SSD1306_WHITE);
    display.display();
    delay(5);
  }

  delay(500);
}

void scroll(void){
  display.startscrollright(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(100);
  display.startscrollleft(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
}

void testdrawline(void) {
  int16_t i;

  display.clearDisplay(); // Clear display buffer

  for(i=0; i<display.width(); i+=4) {
    display.drawLine(0, 0, i, display.height()-1, SSD1306_WHITE);
    display.display(); // Update screen with each newly-drawn line
    delay(1);
  }
  for(i=0; i<display.height(); i+=4) {
    display.drawLine(0, 0, display.width()-1, i, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  delay(150);
  drv.setWaveform(0, 47);  // play effect 
  drv.setWaveform(1, 0);       // end waveform   // end waveform
  drv.go();
  display.clearDisplay();

  for(i=0; i<display.width(); i+=4) {
    display.drawLine(0, display.height()-1, i, 0, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  for(i=display.height()-1; i>=0; i-=4) {
    display.drawLine(0, display.height()-1, display.width()-1, i, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  delay(150);
  drv.setWaveform(0, 47);  // play effect 
  drv.setWaveform(1, 0);       // end waveform   // end waveform
  drv.go();
  display.clearDisplay();

  for(i=display.width()-1; i>=0; i-=4) {
    display.drawLine(display.width()-1, display.height()-1, i, 0, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  for(i=display.height()-1; i>=0; i-=4) {
    display.drawLine(display.width()-1, display.height()-1, 0, i, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  delay(150);
  drv.setWaveform(0, 47);  // play effect 
  drv.setWaveform(1, 0);       // end waveform   // end waveform
  drv.go();
  display.clearDisplay();

  for(i=0; i<display.height(); i+=4) {
    display.drawLine(display.width()-1, 0, 0, i, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  for(i=0; i<display.width(); i+=4) {
    display.drawLine(display.width()-1, 0, i, display.height()-1, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  drv.setWaveform(0, 47);  // play effect 
  drv.setWaveform(1, 0);       // end waveform   // end waveform
  drv.go();
  delay(2000); // Pause for 2 seconds
}
