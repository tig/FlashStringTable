#include <Arduino.h>
#include <ArduinoLog.h>

#include <FlashStringTable.h>
#include "Machine.h"

void waitForSerial(unsigned long timeout_millis) {
  // Start Serial
  Serial.begin(57600);
  unsigned long start = millis();
  while (!Serial && !Serial.available()) {
    if (millis() - start > timeout_millis) break;
  }
}

BEGIN_FLASH_STRING_TABLE(myFlashStringTable)
ADD_FLASH_STRING("1")
ADD_FLASH_STRING("second")
ADD_FLASH_STRING("third")
END_FLASH_STRING_TABLE()

void setup() {
  // put your setup code here, to run once:
  waitForSerial(8000);
  delay(250);

  // TODO: For release builds, change to LOG_LEVEL_WARNING
#ifndef DRV_DEBUG_H_
  Log.begin(LOG_LEVEL_VERBOSE, &Serial, false);
#else
  Log.begin(LOG_LEVEL_VERBOSE, &_Debug, false);
#endif

  // Log.setPrefix(printTimestamp); // Uncomment to get timestamps as prefix
  Log.noticeln("Arduino Test App");
  INIT_FLASH_STRING_TABLE(myFlashStringTable);

  for (uint16_t i = 0; i < myFlashStringTable_size; i++) {
    Log.traceln("entry %d = '%S'", i, myFlashStringTable[i]);
  }

  Log.traceln("Machine!");
  Machine machine;
  machine.begin();
  Log.traceln("Machine printTo: %p ", machine);
  Log.traceln("Machine State Strings: %d [0] = %S", machine.stateStrings.getNumStrings(), machine.stateStrings.getString(0));
  Log.traceln("Machine State printTo: %p ", machine.stateStrings);
  for (uint16_t i = 0; i < machine.stateStrings.getNumStrings(); i ++){
    Log.trace("%d = %S, ", i, machine.stateStrings.getString(i));
  }
  Log.traceln("");
  Log.traceln("Machine Trigger Strings: %d [0] = %S", machine.triggerStrings.getNumStrings(), machine.triggerStrings.getString(0));
  Log.traceln("Machine Trigger printTo: %p", machine.triggerStrings);
  for (uint16_t i = 0; i < machine.triggerStrings.getNumStrings(); i ++){
    Log.trace("%d = %S, ", i, machine.triggerStrings.getString(i));
  }
  Log.traceln("");

  Log.traceln("StringTable!");
  StringTable st(_progmem_myFlashStringTable);
    for (uint16_t i = 0; i < st.getNumStrings(); i ++){
    Log.trace("%d = %S, ", i, st.getString(i));
  }
  Log.traceln("");
  Log.traceln("st printTo: %p", st);

  Log.traceln("Beaker!");
  Beaker b;
  b.begin();
  Log.traceln("Beaker State: %d %S", b.getState(), b.getStateString());
  Log.traceln("Beaker printTo: %p", b);
  for (uint16_t i = 0; i < b.getNumStrings(); i ++){
    Log.trace("%d = %S, ", i, b.getString(i));
  }
  Log.traceln("");

  Log.traceln("Piggy!");
  Piggy piggy;
//  piggy.begin();
  Log.traceln("Piggy strings: %d %S", piggy.strings.getNumStrings(), piggy.strings.getString(0));
  Log.traceln("Piggy printTo: %p", piggy);
  for (uint16_t i = 0; i < piggy.strings.getNumStrings(); i ++){
    Log.trace("%d = %S, ", i, piggy.strings.getString(i));
  }
  Log.traceln("");



  Log.traceln("Done!");
}

void loop() {
}

// handle diagnostic informations given by assertion and abort program
// execution:
void __assert(const char* __func, const char* __file, int __lineno,
              const char* __sexp) {
  // transmit diagnostic informations through serial link.
  char out[255];
  sprintf_P(out, PSTR("FATAL: %s:%d: assert: '%s' in %s()"), __file, __lineno,
            __sexp, __func);
  Log.fatalln(out);

  Log.fatalln(F("program execution halted"));

  // abort program execution.
  Serial.flush();
  // abort();
}
