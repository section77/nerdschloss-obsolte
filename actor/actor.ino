#include <AccelStepper.h>

#define FULL_STEP_MODE 4
#define HALF_STEP_MODE 8

#define DEFAULT_MAX_SPEED 2000
#define DEFAULT_SPEED 1000
#define DEFAULT_ACCELERATION 500

#define SERIAL_STATE_PIN 7


AccelStepper initStepper(int);

typedef struct {
  String cmd;
  int    payload;
} Command;

AccelStepper stepper = initStepper(HALF_STEP_MODE);
boolean usagePrinted = true;

void setup() {
  Serial.begin(9600);
}


void loop() {

  if (Serial.available() > 0) {
    Command command = serialReadCommand();

    // config
    if (command.cmd == "full-step-mode") {
      serialPrintf("set full-step-mode", FULL_STEP_MODE);
      stepper = initStepper(FULL_STEP_MODE);
    } else if (command.cmd == "half-step-mode") {
      serialPrintf("set half-step-mode", HALF_STEP_MODE);
      stepper = initStepper(HALF_STEP_MODE);
    } else if (command.cmd == "max-speed") {
      serialPrintf("set max-speed: %d", command.payload);
      stepper.setMaxSpeed(command.payload);
    } else if (command.cmd == "speed") {
      serialPrintf("set speed: %d", command.payload);
      stepper.setSpeed(command.payload);
    } else if (command.cmd == "acceleration") {
      serialPrintf("set acceleration: %d", command.payload);
      stepper.setAcceleration(command.payload);
    }


    // move
    if (command.cmd == "move-to") {
      moveStepper(command.payload);
    }
  }

  if(clientConnected()) {
    if(! usagePrinted) {
      usagePrinted = true;
      Serial.println("Supported commands:");
      Serial.println(" - full-step-mode / half-step-mode");
      Serial.println(" - max-speed:x");
      Serial.println(" - speed:x");
      Serial.println(" - acceleration:x");
      Serial.println(" - move-to:x");

      stepper = initStepper(HALF_STEP_MODE);
    }
  } else {
    usagePrinted = false;
  }

  delay(10);
}


// FIXME: enable / disable motor
void moveStepper(int steps) {
  serialPrintf("move stepper: %d", steps);
  stepper.moveTo(steps);
  stepper.runToPosition();
}


Command serialReadCommand() {
  String raw = Serial.readStringUntil('\n');
  int sepIndex = raw.indexOf(":");

  // command without payload
  if (sepIndex == -1) {
    return Command { raw, 0 };
  }

  // command with payload - split each part
  String cmd = raw.substring(0, sepIndex);
  String payload_str = raw.substring(sepIndex + 1);

  // convert the given payload to int - this is really unsafe
  int payload = payload_str.toInt();

  return Command { cmd, payload };
}


AccelStepper initStepper(int mode) {

  serialPrintf("init stepper\n  - mode:%d\n  - max_speed:%d\n  - speed:%d\n  - acceleration:%d\n",
               mode, DEFAULT_MAX_SPEED, DEFAULT_SPEED, DEFAULT_ACCELERATION);
  stepper = AccelStepper(mode, 8, 10, 9, 11);
  stepper.setMaxSpeed(DEFAULT_MAX_SPEED);
  stepper.setSpeed(DEFAULT_SPEED);
  stepper.setAcceleration(DEFAULT_ACCELERATION);
  return stepper;
}


 boolean clientConnected() {
   return digitalRead(SERIAL_STATE_PIN);
 }
