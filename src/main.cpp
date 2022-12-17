#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL343.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Adafruit_DRV2605.h"

Adafruit_DRV2605 drv;
#define OLED_RESET     -1
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3D
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
/* Assign a unique ID to this sensor at the same time */
Adafruit_ADXL343 accel = Adafruit_ADXL343(12345);

/** The input pin to enable the interrupt on, connected to INT1 on the ADXL. */
#define INPUT_PIN_INT1   (A0) // SAMD21/SAMD51 = 5 for interrupt pin

uint32_t g_tap_count = 0;
int_config g_int_config_enabled = { 0 };
int_config g_int_config_map = { 0 };

void testscrolltext();
/** Interrupt service routine for INT1 events. This will be called when a single tap is detected. */
void int1_isr(void)
{
  Serial.println("Single tap detected!");
    g_tap_count++;
}

void setup(void)
{
  Serial.begin(115200);
  while (!Serial);
  Serial.println("ADXL343 Single Tap INT Tester"); Serial.println("");

  /* Initialise the sensor */
  if(!accel.begin())
  {
    /* There was a problem detecting the ADXL343 ... check your connections */
    Serial.println("Ooops, no ADXL343 detected ... Check your wiring!");
    while(1);
  }

  /* Set the range to whatever is appropriate for your project */
  accel.setRange(ADXL343_RANGE_16_G);

  /* Configure the HW interrupts. */
  pinMode(INPUT_PIN_INT1, INPUT);
  attachInterrupt(digitalPinToInterrupt(INPUT_PIN_INT1), int1_isr, RISING);

  /* Enable single tap interrupts on the accelerometer. */
  g_int_config_enabled.bits.single_tap = true;
  accel.enableInterrupts(g_int_config_enabled);

  /* Map single tap interrupts to INT1 pin. */
  g_int_config_map.bits.single_tap = ADXL343_INT1;
  accel.mapInterrupts(g_int_config_map);

  /* Reset tap counter. */
  g_tap_count = 0;

  Serial.println("ADXL343 init complete. Waiting for single tap INT activity.");
  //DISPLAY SETUP
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  if (! drv.begin()) {
    Serial.println("Could not find DRV2605");
    while (1) delay(10);
  }
  drv.selectLibrary(1);
  drv.setMode(DRV2605_MODE_INTTRIG);
  display.display();
}

long lastDisplayTime = 0;
long displayDelay = 300;
void loop(void)
{
  /* Get a new sensor event */
  /* Reading data clears the interrupts. */
  sensors_event_t event;
  accel.getEvent(&event);
  delay(10);


  if (g_tap_count) {
      Serial.println("Single tap detected!");
      /* Clear the interrupt as a side-effect of reading the interrupt source register.. */
      accel.checkInterrupts();
      /* Decrement the local interrupt counter. */
      g_tap_count = 0;
      //Activate the buzzer
      drv.setWaveform(0, 52);  // strong buzz
      drv.setWaveform(1, 0);       // end waveform
      drv.go();

      display.clearDisplay();
      display.setTextSize(6);
      display.setCursor(10,0);
      display.println(F("HIM"));      
     
  } else {
    if (millis() - lastDisplayTime > displayDelay){
    display.clearDisplay();
    display.setTextSize(4);
    display.setCursor(10,0);
    display.println(F("NOT HIM"));
    display.display();
    lastDisplayTime = millis();
    }

  }
  
}

void testscrolltext(void) {
  display.clearDisplay();

  display.setTextSize(4); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println(F("HIM"));
  display.display();      // Show initial text
  delay(100);

  // Scroll in various directions, pausing in-between:
  display.startscrollright(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrollleft(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrolldiagright(0x00, 0x07);
  delay(2000);
  display.startscrolldiagleft(0x00, 0x07);
  delay(2000);
  display.stopscroll();
  delay(1000);
}