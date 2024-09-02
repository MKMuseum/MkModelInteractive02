/* MKModel Touch Reader PICO 
*  Core 1 - Touch pad polling and response to Master PICO I2C request 
*  Core 2 - Unused
* Parent sketches:
*  MkModelTouchReader01
* MRD 24/04/2023 ver 01
* MRD 28/08/2024 Implement 'synthetic' touch mode
*    notes:  Now has sequence and casual modes. Change via 'style' constant below
*    Put into GIT control repro MkModelInteractive02
*    Quick WIP test
*     Quick update 2
*/
#include <Wire.h>
#include <ElapsedTime.h>

#define MIN_RETOUCH 3000     // Will not recognise pad re-touch if it occurs within this time period (milliseconds)
#define AUTO_INTERVAL 10000  // Interval that automatic styles will refresh touch index

enum styles { interact,
              sequence,
              casual };
const enum styles style = casual;

unsigned char noPad = '@';          // represents no pad touched since last master request
unsigned char touchBuffer = noPad;  // holds the ID of the last pad that was touched (A - X)
unsigned char prevTouch = noPad;

// Polling sequence depends on custom pcb conector sequence which is not in strict PICO GPIO pin order
// Can remap physical touchpad connections here if required
const pin_size_t pinRef[] = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 15, 14, 18, 19, 20, 21, 28, 27, 26, 22, 17, 18, 12, 13 };
const int padCount = sizeof(pinRef) / sizeof(pin_size_t);

// Interval timer for retouch 'debounce' and automatic styles
ElapsedTime intervalTimer(MIN_RETOUCH);

void setup() {
  Serial.begin(9600);
  Serial.println("Initialising");
  pinMode(LED_BUILTIN, OUTPUT);
  randomSeed(analogRead(0));
  // Set touchpad GPIO pin modes
  initTouchPads();
  // initialaise I2C communications
  initWire();
  Serial.println("Ready");
}

void loop() {
  // only read touchpad states if style is 'interactive'. Must be minimum interval 'MIN_RETOUCH' between touches.
  // otherwise sequential or random selection on next 'touch' index. Must be interval of 'AUTO_INTERVAL' between selections.
  if (intervalTimer.timeHasElapsed()) {
    switch (style) {
      case interact:
        readTouchPads();
        intervalTimer.startTimer(MIN_RETOUCH);
        break;
      case sequence:
        fakeTouchSequence();
        intervalTimer.startTimer(AUTO_INTERVAL);
        break;
      case casual:
        fakeTouchCasual();
        intervalTimer.startTimer(AUTO_INTERVAL);
        break;
    }
  }
}

// initialise touch pad GPIO pins
void initTouchPads() {
  for (int index = 0; index < padCount; index++) {
    pinMode(pinRef[index], INPUT_PULLDOWN);
    //Serial.println(pinRef[index]);
  }
}

// initialise the I2C comms
void initWire() {
  Wire.setSDA(0);
  Wire.setSCL(1);
  Wire.begin(0x30);
  Wire.onReceive(recv);
  Wire.onRequest(req);
  delay(100);
}

// Read any touches and store in I2C call buffer
void readTouchPads() {
  digitalWrite(LED_BUILTIN, LOW);
  for (int index = 0; index < padCount; index++) {
    if (digitalRead(pinRef[index]) == HIGH) {
      digitalWrite(LED_BUILTIN, HIGH);
      while (digitalRead(pinRef[index]) == HIGH) {
        delay(10);
      }
      recordTouch(index);
      break;  // don't pole further down the array of touch pads if a touch is detected
    }
  }
}

// record the ID of a touchpad to provision buffer for subsequent request from I2C master
void recordTouch(int padIndex) {
  // record 'A' for index 0, 'X' for index 23 etc.
  // recorded value references PCB touchpad connector order not PICO GPIO pin number
  unsigned char touch = padIndex + 'A';
  if (touch != prevTouch) {
    touchBuffer = touch;
    prevTouch = touch;
    Serial.println(touchBuffer);
    Serial.println((char)touchBuffer);
  }
}

// Fake a touchpad touch in sequence. Sequence is as for pinRef list
void fakeTouchSequence() {
  static int padIndex = 0;

  recordTouch(padIndex);

  if (++padIndex > padCount - 1) {
    padIndex = 0;
  }
}

// Fake a touchpad touch randomly
void fakeTouchCasual() {
  recordTouch(random(0, padCount - 1));
}

// Called when the I2C slave gets written to
void recv(int len) {
  // Just consume anything recieved for now
  for (int i = 0; i < len; i++) {
    Wire.read();
  }
}

// Called when the I2C slave is read from
void req() {
  Wire.write((byte)touchBuffer);
  touchBuffer = noPad;
}
