#include "Header.h"

void setup() {
  debug.begin();
  debug.useUserDefinedLogLevels();
  LOG_INFO = debug.createLogLevel("LOG_INFO");
  LOG_SENSOR = debug.createLogLevel("LOG_SENSOR");
  LOG_COMS = debug.createLogLevel("LOG_COMS");
  usbSerial.begin(&Serial);
  task.initialize(wifiTask);
  sensor.addModule("sensor1", new AnalogSens(34, 3.3, 4096));
  sensor.addModule("sensor2", new AnalogSens(35, 3.3, 4096));
  sensor.init();
  buzzer.toggleInit(100, 5);
}

void loop() {
  sensor.update([]() {
    // sensor.debug();
    value1 = sensor["sensor1"]["raw"];
    value2 = sensor["sensor2"]["raw"];

    debug.startPrint(LOG_SENSOR);
    debug.continuePrint("value1", value1, LOG_SENSOR);
    debug.continuePrint("value2", value2, LOG_SENSOR);
    debug.endPrint(LOG_SENSOR, true);
  });
  usbSerial.receiveString(usbCommunicationCallback);

  DigitalIn::updateAll(&buttonDown, &buttonOk, DigitalIn::stop());
  DigitalOut::updateAll(&buzzer, DigitalOut::stop());
}
