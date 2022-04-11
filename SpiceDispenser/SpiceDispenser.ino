// Bounce.pde
// -*- mode: C++ -*-
//
// Make a single stepper bounce from one limit to another
//
// Copyright (C) 2012 Mike McCauley
// $Id: Random.pde,v 1.1 2011/01/05 01:51:01 mikem Exp mikem $

<<<<<<< Updated upstream
bool debug = false;
=======
bool debug = true;
>>>>>>> Stashed changes
long int count = 0; //temp work around to display slowing the stepper down
long int previousUpdate = 0;

#include <AccelStepper.h>
AccelStepper stepper1(1, 2, 15);     //(mode, pulse, dir)
AccelStepper stepper2(1, 16, 17);     //(mode, pulse, dir)   //needs different pins


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
#define calibration_factor1 -7050.0 //This value is obtained using the SparkFun_HX711_Calibration sketch
#define LOADCELL_DOUT_PIN  0
#define LOADCELL_SCK_PIN  4
HX711 scale1;

//#define calibration_factor2 -7050.0 //This value is obtained using the SparkFun_HX711_Calibration sketch
//#define LOADCELL_DOUT_PIN  27
//#define LOADCELL_SCK_PIN  26
//HX711 scale2;





void setup()
{  
  if (debug) {
    Serial.begin(9600);

    display0.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS0);
    display1.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS1);

    // if(!display0.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS0)) {
    // Serial.println(F("SSD1306 allocation failed"));
    // for(;;); // Don't proceed, loop forever
    // }

  } else {
    display0.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS0);
    display1.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS1);
  }

  //
  //
  //Stepper Control
  //
  //
    stepper1Setup(2600, 3000);
    stepper2Setup(2600, 3000);

  //
  //
  //scale1 controll
  //
  //
    scale1Setup();

    //scale2Setup();


  //
  //
  //Display control
  //
  //
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
}

void loop()
{
  //
  //
  //Stepper Control
  //
  //
<<<<<<< Updated upstream
    stepperControl(2600, 3000);

  //
  //
  //Display 0 control
  //
  //
//    display0.setTextSize(1);
//    display0.setTextColor(SSD1306_WHITE);
//    display0.setCursor(0,0);
//    display0.print(F("Green Button: Stepper Runs While Pressed\n Yellow Button: Stepper Moves A Programmed Number Of Steps With Acceleration"));
//    display0.display();

  
  //
  //
  //Display 1 control
  //
  //
//    display1.setTextSize(1);
//    display1.setTextColor(SSD1306_WHITE);
//    display1.setCursor(0,0);
//    display1.print(F("Current Load Cell Reading: "));
//    display1.display();
//        if(millis() - previousUpdate >= 500) {
//          int temp = scale.get_units();
//          display1.print(F("test"));
//          display1.display();
//          previousUpdate = millis();
//        } else {
//          count++;
//        }




}


void stepperSetup(int maxSpeed_, int maxAcceleration_) {
  pinMode(32, OUTPUT);    //dissable & enable pin for the stepper controller
  digitalWrite(32, HIGH);   //turn off the stepper driver to keep heat down and save power
  pinMode(33, INPUT_PULLUP);    //cont. run button
  pinMode(25, INPUT_PULLUP);    //canned distance button
  stepper.setMaxSpeed(maxSpeed_);
  stepper.setAcceleration(maxAcceleration_);
}

=======
    stepper1Control(2600, 3000);
    stepper2Control(2600, 3000);

  //
  //
  //Display 0 control
  //
  //
//    display0.setTextSize(1);
//    display0.setTextColor(SSD1306_WHITE);
//    display0.setCursor(0,0);
//    display0.print(F("Green Button: Stepper Runs While Pressed\n Yellow Button: Stepper Moves A Programmed Number Of Steps With Acceleration"));
//    display0.display();

  
  //
  //
  //Display 1 control
  //
  //
   
      if(millis() - previousUpdate >= 1000) {
        //clear the display
        display1.clearDisplay();
        display1.setTextSize(1);
        display1.setTextColor(SSD1306_WHITE);
        display1.setCursor(0,0);
        display1.print(F("Current Load Cell Reading: "));
        //display1.display();
  
        int temp = scale1.get_units();
        //temp = 50;
        char temp_char[15];
        itoa(temp, temp_char, 10);

        display1.print(F(temp_char));
        display1.display();
        previousUpdate = millis();
      } else {
        count++;
      }




}


void stepper1Setup(int maxSpeed_, int maxAcceleration_) {
  pinMode(32, OUTPUT);    //dissable & enable pin for the stepper controller
  digitalWrite(32, HIGH);   //turn off the stepper driver to keep heat down and save power
  pinMode(33, INPUT_PULLUP);    //cont. run button
  //pinMode(25, INPUT_PULLUP);    //canned distance button
  stepper1.setMaxSpeed(maxSpeed_);
  stepper1.setAcceleration(maxAcceleration_);

}

void stepper2Setup(int maxSpeed_, int maxAcceleration_) {
  //pinMode(32, OUTPUT);    //dissable & enable pin for the stepper controller
  //uses the same enable and dissable pin as stepper 1
  //digitalWrite(32, HIGH);   //turn off the stepper driver to keep heat down and save power
  // pinMode(33, INPUT_PULLUP);    //cont. run button
  // pinMode(25, INPUT_PULLUP);    //canned distance button
  pinMode(25, INPUT_PULLUP);    //run for stepper 2
  stepper2.setMaxSpeed(maxSpeed_);
  stepper2.setAcceleration(maxAcceleration_);

}


>>>>>>> Stashed changes
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

void scale1Setup() {
  scale1.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale1.set_scale(calibration_factor1); //This value is obtained by using the SparkFun_HX711_Calibration sketch
  scale1.tare(); //Assuming there is no weight on the scale1 at start up, reset the scale1 to 0

}

// void scale2Setup() {
//   scale2.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
//   scale2.set_scale(calibration_factor2); //This value is obtained by using the SparkFun_HX711_Calibration sketch
//   scale2.tare(); //Assuming there is no weight on the scale2 at start up, reset the scale2 to 0

// }

void stepper1Control(int maxSpeed, int maxAcceleration) {
  if(digitalRead(33) == 0) {  //button 1 pressed -> run while button is pressed
    digitalWrite(32, LOW);    //when pin is low the stepper driver should turn on
    stepper1.setSpeed(1300);
    stepper1.runSpeed();
    stepper1.move(0);

  }
  // } else if(digitalRead(25) == 0) { //button 2 pressed runn a set distance only
  //   stepper.stop();
  //   digitalWrite(32, LOW);    //when pin is low the stepper driver should turn on
    
  //   if (stepper.distanceToGo() == 0) {
  //     stepper.setAcceleration(3000);
  //     stepper.setMaxSpeed(2000);
  //     stepper.move(2000);
  //     stepper.setSpeed(2000);
  //     stepper.runSpeedToPosition();
  //   }
    
  // }
   else if (digitalRead(33) == 1 && stepper1.distanceToGo() == 0 && stepper2.distanceToGo() == 0) {   //wait to turn off untill the button is released and or the stepper has reached it's target dest.
    stepper1.move(0);
    stepper2.move(0);
    digitalWrite(32, HIGH);
  }

  stepper1.run();    //run any steps qued for the stepper
}

void stepper2Control(int maxSpeed, int maxAcceleration) {
  // if(digitalRead(25) == 0) {  //button 1 pressed -> run while button is pressed
  //   digitalWrite(32, LOW);    //when pin is low the stepper driver should turn on
  //   stepper2.setSpeed(2000);
  //   stepper2.runSpeed();
  //   stepper2.move(0);

  // }
  if(digitalRead(25) == 0) { //button 2 pressed runn a set distance only
    stepper1.stop();
    stepper2.stop();
    digitalWrite(32, LOW);    //when pin is low the stepper driver should turn on
    
    if (stepper2.distanceToGo() == 0) {
      stepper2.setAcceleration(3000);
      stepper2.setMaxSpeed(2000);
      stepper2.move(2000);
      stepper2.setSpeed(2000);
      stepper2.runSpeedToPosition();
    }
    
  }
   else if (digitalRead(25) == 1 && stepper2.distanceToGo() == 0 && stepper1.distanceToGo()) {   //wait to turn off untill the button is released and or the stepper has reached it's target dest.
    stepper1.move(0);
    stepper2.move(0);
    digitalWrite(32, HIGH);
  }

<<<<<<< Updated upstream
  stepper.run();    //run any steps qued for the stepper
}
=======
  stepper2.run();    //run any steps qued for the stepper
}
>>>>>>> Stashed changes
