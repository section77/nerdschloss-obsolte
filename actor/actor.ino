#include <Stepper.h>
Stepper configureStepper(int, int);

typedef struct {
  String cmd;
  int    payload;
} Command;

int stepsPerRevolution = 2048;
int rpm = 15;
Stepper stepper = configureStepper(stepsPerRevolution, rpm);

void setup() {
  Serial.begin(9600);
}


void loop() {

  if (Serial.available() > 0) {
    Command command = serialReadCommand();

    // config
    if (command.cmd == "spr") {
      stepper = configureStepper(command.payload, rpm);
    } else if (command.cmd == "rpm") {
      stepper = configureStepper(stepsPerRevolution, command.payload);
    }


    // steps
    if (command.cmd == "s") {
      moveStepper(command.payload);
    }


    // right
    if (command.cmd == "1r") {
      moveStepper(-stepsPerRevolution);
    } else if (command.cmd == "1/2r") {
      moveStepper(-stepsPerRevolution / 2);
    } else if (command.cmd == "1/4r") {
      moveStepper(-stepsPerRevolution / 4);
    }


    // left
    if (command.cmd == "1l") {
      moveStepper(stepsPerRevolution);
    } else if (command.cmd == "1/2l") {
      moveStepper(stepsPerRevolution / 2);
    } else if (command.cmd == "1/4l") {
      moveStepper(stepsPerRevolution / 4);
    }
  }
  delay(10);
}


// FIXME: enable / disable motor
void moveStepper(int steps) {
  serialPrintf("move stepper: %s", steps);
  stepper.step(steps);
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


Stepper configureStepper(int _stepsPerRevolution, int _rpm) {
  stepsPerRevolution = _stepsPerRevolution;
  rpm = _rpm;

  serialPrintf("configure stepper\n  - spr:%d\n  - rpm:%d", stepsPerRevolution, rpm);
  stepper = Stepper(stepsPerRevolution, 8, 10, 9, 11);
  stepper.setSpeed(rpm);
  return stepper;
}
