// Bounce.pde
// -*- mode: C++ -*-
//
// Make a single stepper bounce from one limit to another
//
// Copyright (C) 2012 Mike McCauley
// $Id: Random.pde,v 1.1 2011/01/05 01:51:01 mikem Exp mikem $

TaskHandle_t handle_core_0;
TaskHandle_t handle_core_1;

bool debug = false;
long int count = 0; //temp work around to display slowing the stepper down
long int previousUpdate = 0;

#include <AccelStepper.h>
AccelStepper stepper(1, 2, 15);     //(mode, pulse, dir)

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS0 0x3D // display 1?
#define SCREEN_ADDRESS1 0x3c // display 2?
Adafruit_SSD1306 display0(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_SSD1306 display1(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


#include "HX711.h"
#define calibration_factor -7050.0 //This value is obtained using the SparkFun_HX711_Calibration sketch
#define LOADCELL_DOUT_PIN  0
#define LOADCELL_SCK_PIN  4
HX711 scale;





void setup()
{  
  if (debug) {
    Serial.begin(9600);

    if(!display0.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS0)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
    }

  } else {
    display0.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS0);
    display1.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS1);
  }

  //
  //
  //Stepper Control
  //
  //
    stepperSetup(2600, 3000);

    display_0_Setup();  //identify and then chose pick one for spices and the other for weight
    display_1_Setup();  //

  //
  //
  //Display control
  //
  //

  delay(2000);  //wait for everything to start up -> wifi?
  display0.clearDisplay();
  display1.clearDisplay();

  display0.setTextSize(1);
  display0.setTextColor(SSD1306_WHITE);
  display0.setCursor(0,0);
  display0.print(F("Green Button: Stepper Runs While Pressed\n Yellow Button: Stepper Moves A Programmed Number Of Steps With Acceleration"));
  display0.display();

  display1.setTextSize(1);
  display1.setTextColor(SSD1306_WHITE);
  display1.setCursor(0,0);
  display1.print(F("Current Load Cell Reading: "));
  display1.display();

  //
  //
  //Multi Threading
  //
  //
    //Core 0
      xTaskCreatePinnedToCore(
                    core_0,   /* Task function. */
                    "stepperDrive",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &handle_core_0,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */                  
  delay(500);
    //Core 1
        xTaskCreatePinnedToCore(
                      core_1,   /* Task function. */
                      "displayReadings",     /* name of task. */
                      10000,       /* Stack size of task */
                      NULL,        /* parameter of the task */
                      1,           /* priority of the task */
                      &handle_core_1,      /* Task handle to keep track of created task */
                      1);          /* pin task to core 0 */                  
    delay(500);


}

void core_0(void * nullParam) {
//  for(;;) {
//    stepperControl(2600, 3000);
//
//
//  }



}

void core_1(void * nullParam) {




}


void loop()
{
  
}







void stepperSetup(int maxSpeed_, int maxAcceleration_) {
  pinMode(32, OUTPUT);    //dissable & enable pin for the stepper controller
  digitalWrite(32, HIGH);   //turn off the stepper driver to keep heat down and save power
  pinMode(33, INPUT_PULLUP);    //cont. run button
  pinMode(25, INPUT_PULLUP);    //canned distance button
  stepper.setMaxSpeed(maxSpeed_);
  stepper.setAcceleration(maxAcceleration_);
}

void display_0_Setup() {    //on the left side
  display0.clearDisplay();

  display0.setTextSize(2);
  display0.setTextColor(SSD1306_WHITE);
  display0.print(F("Display 0: \nLoading..."));

  display0.display();

}
void display_1_Setup() {    //on the right side
  display1.clearDisplay();

  display1.setTextSize(2);
  display1.setTextColor(SSD1306_WHITE);
  display1.print(F("Display 1: \nLoading..."));

  display1.display();

}

void scaleSetup() {
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor); //This value is obtained by using the SparkFun_HX711_Calibration sketch
  scale.tare(); //Assuming there is no weight on the scale at start up, reset the scale to 0

}

void stepperControl(int maxSpeed, int maxAcceleration) {
  if(digitalRead(33) == 0) {  //button 1 pressed -> run while button is pressed
    digitalWrite(32, LOW);    //when pin is low the stepper driver should turn on
    stepper.setSpeed(2000);
    stepper.runSpeed();
    stepper.move(0);

    
  } else if(digitalRead(25) == 0) { //button 2 pressed runn a set distance only
    stepper.stop();
    digitalWrite(32, LOW);    //when pin is low the stepper driver should turn on
    
    if (stepper.distanceToGo() == 0) {
      stepper.setAcceleration(3000);
      stepper.setMaxSpeed(2000);
      stepper.move(2000);
      stepper.setSpeed(2000);
      stepper.runSpeedToPosition();
    }
    
  } else if (digitalRead(33) == 1 && stepper.distanceToGo() == 0) {   //wait to turn off untill the button is released and or the stepper has reached it's target dest.
    digitalWrite(32, HIGH);
  }

  stepper.run();    //run any steps qued for the stepper
}

