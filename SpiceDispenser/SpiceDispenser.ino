// GEEN1400 Scuba Lemurs
//Taro, Rose, Dalal, Lucas, Carter

#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"
TaskHandle_t handle_core_0;
TaskHandle_t handle_core_1;

bool debug = true;
long int debounce = 200; // button debounce time (ms)
long int previousUpdate0 = 0;
long int previousUpdate1 = 0;
long int previousUpdate2 = 0;
long int previousUpdate3 = 0;
long int previousUpdate4 = 0;
long int previousUpdate5 = 0;


#include <AccelStepper.h>
AccelStepper stepper1(1, 2, 15);  //(mode, pulse, dir)
AccelStepper stepper2(1, 16, 17); //(mode, pulse, dir)   //needs different pins

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128     // OLED display width, in pixels
#define SCREEN_HEIGHT 64     // OLED display height, in pixels
#define OLED_RESET -1        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS0 0x3D // display 1?
#define SCREEN_ADDRESS1 0x3c // display 2?
Adafruit_SSD1306 display0(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_SSD1306 display1(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#include "HX711.h"
#define calibration_factor1 -8230.0 // This value is obtained using the SparkFun_HX711_Calibration sketch
#define LOADCELL_DOUT_PIN 0
#define LOADCELL_SCK_PIN 4
HX711 scale1;

#define calibration_factor2 -8230.0 // This value is obtained using the SparkFun_HX711_Calibration sketch
#define LOADCELL_DOUT_PIN2 27
#define LOADCELL_SCK_PIN2 26
HX711 scale2;


#define buttonPin0 34
#define buttonPin1 35
#define buttonPin2 12
#define buttonPin3 13
#define buttonPin4 14
#define buttonPin5 19

volatile bool buttonState[6] = {false, false, false, false, false, false};

void setup()
{
  if (debug == true)
  {
    Serial.begin(9600);

    display0.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS0);
    display1.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS1);
  }
  else
  {
    display0.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS0);
    display1.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS1);
  }

  //
  //
  // Stepper Control
  //
  //
  stepper1Setup(2600, 3000);  //dispense
  stepper2Setup(2600, 3000);

  if (debug == true)  Serial.println("stepper setup");

  //
  //
  // scale1 controll
  //
  //
  scale1Setup();
  scale2Setup();

  //
  //
  // Display control
  //
  //
  display_0_Setup(); // identify and then chose pick one for spices and the other for weight
  display_1_Setup(); //

  //
  //
  // Display control
  //
  //
  delay(2000); // wait for everything to start up -> wifi?
  display0.clearDisplay();
  display1.clearDisplay();

  display0.setTextSize(1);
  display0.setTextColor(SSD1306_WHITE);
  display0.setCursor(0, 0);
  display0.print(F("Green Button: Stepper Runs While Pressed\n Yellow Button: Stepper Moves A Programmed Number Of Steps With Acceleration"));
  display0.display();

  display1.setTextSize(1);
  display1.setTextColor(SSD1306_WHITE);
  display1.setCursor(0, 0);
  display1.print(F("Current Load Cell Reading: "));
  display1.display();

  //
  //
  // Button control
  //
  //
  pinMode(buttonPin0, INPUT_PULLUP);
  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);
  pinMode(buttonPin3, INPUT_PULLUP);
  pinMode(buttonPin4, INPUT_PULLUP);
  pinMode(buttonPin5, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(buttonPin0), changeButtonState0, FALLING);
  attachInterrupt(digitalPinToInterrupt(buttonPin1), changeButtonState1, FALLING);
  attachInterrupt(digitalPinToInterrupt(buttonPin2), changeButtonState2, FALLING);
  attachInterrupt(digitalPinToInterrupt(buttonPin3), changeButtonState3, FALLING);
  attachInterrupt(digitalPinToInterrupt(buttonPin4), changeButtonState4, FALLING);
  attachInterrupt(digitalPinToInterrupt(buttonPin5), changeButtonState5, FALLING);

  //
  //
  // Multi Threading
  //
  //
  // Core 0
  xTaskCreatePinnedToCore(
      core_0,         /* Task function. */
      "stepperDrive", /* name of task. */
      10000,          /* Stack size of task */
      NULL,           /* parameter of the task */
      1,              /* priority of the task */
      &handle_core_0, /* Task handle to keep track of created task */
      0);             /* pin task to core 0 */
  delay(500);
  // Core 1
  xTaskCreatePinnedToCore(
      core_1,            /* Task function. */
      "displayReadings", /* name of task. */
      10000,             /* Stack size of task */
      NULL,              /* parameter of the task */
      1,                 /* priority of the task */
      &handle_core_1,    /* Task handle to keep track of created task */
      1);                /* pin task to core 0 */
  delay(500);

  if (debug == true) Serial.println("multithreading setup complete");
}

void core_0(void *nullParam)
{
  for (;;)
  {
    //
    //
    // Stepper Control
    //
    //
    stepper1Control(-700, 2000);  //dispenser stepper
    stepper2Control(1200, 500, 50);    //rotation stepper

    //
    //
    // Work around for watchdog crashshing/restarts
    //
    //
    TIMERG0.wdt_wprotect = TIMG_WDT_WKEY_VALUE; // Work around for watchdog crashshing/restarts
    TIMERG0.wdt_feed = 1;                       // Work around for watchdog crashshing/restarts
    TIMERG0.wdt_wprotect = 0;                   // Work around for watchdog crashshing/restarts
    // Work around for watchdog crashshing/restarts
  }
}

void core_1(void *nullParam)
{

  for (;;)
  {
    if (debug == true) Serial.println("core_1");
    //
    //
    // Display control
    //
    //
    display1.clearDisplay();
    display1.setTextSize(1);
    display1.setTextColor(SSD1306_WHITE);
    display1.setCursor(0, 0);
    display1.print(F("Current Load Cell Total: "));
    char temp_char[15];
    int temp = scale1.get_units();
    itoa(temp, temp_char, 10);
    display1.print(F(temp_char));
    display1.print(F("\n"));
    temp = scale2.get_units();
    itoa(temp, temp_char, 10);
    display1.print(F(temp_char));
    display1.print(F("\n"));
    temp += scale1.get_units();
    // temp = 50;
    itoa(temp, temp_char, 10);

    display1.print(F(temp_char));
    display1.display();
    //previousUpdate = millis();


    display0.clearDisplay();
    display0.setTextSize(1);
    display0.setTextColor(SSD1306_WHITE);
    display0.setCursor(0, 0);
    for(int i = 0; i < 6; i++)
    {
      display0.print(F("Button "));
      display0.print(i);
      if(buttonState[i])
      {
        display0.print(F(" is pressed\n"));
      } else {
        display0.print(F(" not pressed\n"));
      }
    }
    display0.display();
  }
}

void loop()
{
  Serial.println("loop");
}

void stepper1Setup(int maxSpeed_, int maxAcceleration_)
{
  pinMode(32, OUTPUT);       // dissable & enable pin for the stepper controller
  digitalWrite(32, HIGH);    // turn off the stepper driver to keep heat down and save power
  pinMode(33, INPUT_PULLUP); // cont. run button
  // pinMode(25, INPUT_PULLUP);    //canned distance button
  stepper1.setMaxSpeed(maxSpeed_);
  stepper1.setAcceleration(maxAcceleration_);
}

void stepper2Setup(int maxSpeed_, int maxAcceleration_)
{
  // pinMode(32, OUTPUT);    //dissable & enable pin for the stepper controller
  // uses the same enable and dissable pin as stepper 1
  // digitalWrite(32, HIGH);   //turn off the stepper driver to keep heat down and save power
  //  pinMode(33, INPUT_PULLUP);    //cont. run button
  //  pinMode(25, INPUT_PULLUP);    //canned distance button
  pinMode(25, INPUT_PULLUP); // run for stepper 2
  stepper2.setMaxSpeed(maxSpeed_);
  stepper2.setAcceleration(maxAcceleration_);
}

void display_0_Setup()
{ // on the left side
  display0.clearDisplay();

  display0.setTextSize(2);
  display0.setTextColor(SSD1306_WHITE);
  display0.print(F("Display 0: \nLoading..."));

  display0.display();
}
void display_1_Setup()
{ // on the right side
  display1.clearDisplay();

  display1.setTextSize(2);
  display1.setTextColor(SSD1306_WHITE);
  display1.print(F("Display 1: \nLoading..."));

  display1.display();
}

void scale1Setup()
{
  scale1.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale1.set_scale(calibration_factor1); // This value is obtained by using the SparkFun_HX711_Calibration sketch
  scale1.tare();                         // Assuming there is no weight on the scale1 at start up, reset the scale1 to 0
}

void scale2Setup()
{
  scale2.begin(LOADCELL_DOUT_PIN2, LOADCELL_SCK_PIN2);
  scale2.set_scale(calibration_factor2); // This value is obtained by using the SparkFun_HX711_Calibration sketch
  scale2.tare();                         // Assuming there is no weight on the scale2 at start up, reset the scale2 to 0
}

void stepper1Control(int speed, int maxAcceleration)
{
  if (digitalRead(33) == 0)
  {                        // button 1 pressed -> run while button is pressed
    digitalWrite(32, LOW); // when pin is low the stepper driver should turn on
    stepper1.setSpeed(speed);
    stepper1.runSpeed();
    stepper1.move(0);
  }
  else if (digitalRead(33) == 1 && stepper1.distanceToGo() == 0 && stepper2.distanceToGo() == 0)
  { // wait to turn off untill the button is released and or the stepper has reached it's target dest.
    stepper1.move(0);
    stepper2.move(0);
    digitalWrite(32, HIGH);
  }
  else
  {
    stepper1.move(0);
  }

  stepper1.run(); // run any steps qued for the stepper
}

void stepper2Control(int maxSpeed, int maxAcceleration, int distanceToStep)
{
  // if(digitalRead(25) == 0) {  //button 1 pressed -> run while button is pressed
  //   digitalWrite(32, LOW);    //when pin is low the stepper driver should turn on
  //   stepper2.setSpeed(2000);
  //   stepper2.runSpeed();
  //   stepper2.move(0);

  // }
  if (digitalRead(25) == 0)
  { // button 2 pressed runn a set distance only
    stepper1.stop();
    stepper2.stop();
    digitalWrite(32, LOW); // when pin is low the stepper driver should turn on

    if (stepper2.distanceToGo() == 0)
    {
      stepper2.setSpeed(0);
      stepper2.setAcceleration(maxAcceleration);
      stepper2.setMaxSpeed(maxSpeed);
      stepper2.move(distanceToStep);
      //stepper2.setSpeed(maxSpeed);
      //stepper2.runSpeedToPosition();
    }
  }
  else if (digitalRead(25) == 1 && stepper2.distanceToGo() == 0 && stepper1.distanceToGo())
  { // wait to turn off untill the button is released and or the stepper has reached it's target dest.
    // stepper1.move(0);
    stepper2.move(0);
    digitalWrite(32, HIGH);
  }

  stepper2.run(); // run any steps qued for the stepper
}

//attach interup cannot use functions with parameters...
void changeButtonState0() {
  if (previousUpdate0 == 0){
    previousUpdate0 = millis();
    buttonState[0] = !buttonState[0];
  }
  else if (millis() - previousUpdate0 > debounce) {
    previousUpdate0 = millis();
    buttonState[0] = !buttonState[0];
  }
  //buttonState[0] = !buttonState[0];
}

void changeButtonState1() {
  // buttonState[1] = !buttonState[1];
  if (previousUpdate1 == 0){
    previousUpdate1 = millis();
    buttonState[1] = !buttonState[1];
  }
  else if (millis() - previousUpdate1 > debounce) {
    previousUpdate1 = millis();
    buttonState[1] = !buttonState[1];
  }
}

void changeButtonState2() {
  // buttonState[2] = !buttonState[2];
  if (previousUpdate2 == 0){
    previousUpdate2 = millis();
    buttonState[2] = !buttonState[2];
  }
  else if (millis() - previousUpdate2 > debounce) {
    previousUpdate2 = millis();
    buttonState[2] = !buttonState[2];
  }
}

void changeButtonState3() {
  // buttonState[3] = !buttonState[3];
  if (previousUpdate3 == 0){
    previousUpdate3 = millis();
    buttonState[3] = !buttonState[3];
  }
  else if (millis() - previousUpdate3 > debounce) {
    previousUpdate3 = millis();
    buttonState[3] = !buttonState[3];
  }
}

void changeButtonState4() {
  // buttonState[4] = !buttonState[4];
  if (previousUpdate4 == 0){
    previousUpdate4 = millis();
    buttonState[4] = !buttonState[4];
  }
  else if (millis() - previousUpdate4 > debounce) {
    previousUpdate4 = millis();
    buttonState[4] = !buttonState[4];
  }
}

void changeButtonState5() {
  // buttonState[5] = !buttonState[5];
  if (previousUpdate5 == 0){
    previousUpdate5 = millis();
    buttonState[5] = !buttonState[5];
  }
  else if (millis() - previousUpdate5 > debounce) {
    previousUpdate5 = millis();
    buttonState[5] = !buttonState[5];
  }
}

// void menu() {
  



// }
