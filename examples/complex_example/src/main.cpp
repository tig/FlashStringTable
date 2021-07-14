#include <Arduino.h>
#include <ArduinoLog.h>

#include <FlashStringTable.h>
#include "Machine.h"

// make static so it's not dynamically allocated
static char _logStatus[255];

class testLog
{
public:
  /**
   * @brief Logs the status of the system
   * 
   * @param msg 
   * @param args 
   */
  template <class T, typename... Args>
  void logStatus(T msg, Args... args)
  {
    _logStatus[0] = 0;
    Log.notice(F("--- Status : "));
    Log.noticeln(msg, args...);
    sprintf_P(_logStatus, PSTR("| %-18S| %-10S| %-10S| %-8S %-6S %-6S %-6S %-4S |"),
              F("System"), F("Door"), F("Actuator"), F("Slider"), F("Fwd"), F("Rwd"), F("Speed"), F("%%"));
    Log.noticeln(_logStatus);
    // sprintf_P(_logStatus, PSTR("| %-18S| %-10S| %-10S| %-8S %-4dmm %-4dmm %-6d %-3d |"),
    // _stateStrings.getString(getCurrentState()),
    // door._stateStrings.getString(door.getCurrentState()),
    // door.actuator._stateStrings.getString(door.actuator.getCurrentState()),
    // slider._stateStrings.getString(slider.getCurrentState()),
    // slider.getForwardDistance(),
    // slider.getRearwardDistance(),
    // slider.getCurrentSpeed(),
    // slider.getSpeedFraction());
    // Log.noticeln(_logStatus);
  }
};

void waitForSerial(unsigned long timeout_millis)
{
  // Start Serial
  Serial.begin(57600);
  unsigned long start = millis();
  while (!Serial && !Serial.available())
  {
    if (millis() - start > timeout_millis)
      break;
  }
}

BEGIN_FLASH_STRING_TABLE(myFlashStringTable)
ADD_FLASH_STRING("1")
ADD_FLASH_STRING("second")
ADD_FLASH_STRING("third")
END_FLASH_STRING_TABLE()

void setup()
{
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
  Log.noticeln(F("Arduino Test App"));
  INIT_FLASH_STRING_TABLE(myFlashStringTable);

  for (uint16_t i = 0; i < myFlashStringTable_size; i++)
  {
    Log.traceln("entry %d = '%S'", i, myFlashStringTable[i]);
  }

  Log.traceln(F("Machine!"));
  Machine machine;
  machine.begin();
  Log.traceln("Machine printTo: %p ", machine);
  Log.traceln("Machine State Strings: %d [0] = %S", machine._stateStrings.getNumStrings(), machine._stateStrings.getString(0));
  Log.traceln("Machine State printTo: %p ", machine._stateStrings);
  for (uint16_t i = 0; i < machine._stateStrings.getNumStrings(); i++)
  {
    Log.trace("%d = %S, ", i, machine._stateStrings.getString(i));
  }
  Log.traceln("");
  Log.traceln("Machine Trigger Strings: %d [0] = %S", machine._triggerStrings.getNumStrings(), machine._triggerStrings.getString(0));
  Log.traceln("Machine Trigger printTo: %p", machine._triggerStrings);
  for (uint16_t i = 0; i < machine._triggerStrings.getNumStrings(); i++)
  {
    Log.trace("%d = %S, ", i, machine._triggerStrings.getString(i));
  }
  Log.traceln("");

  Log.traceln(F("StringTable!"));
  StringTable st(_progmem_myFlashStringTable);
  for (uint16_t i = 0; i < st.getNumStrings(); i++)
  {
    Log.trace("%d = %S, ", i, st.getString(i));
  }
  Log.traceln("");
  Log.traceln("st printTo: %p", st);

#ifdef ARDUINO_ARCH_SAMD
#define PSTR(str) ((const char*) str)
#define fmt "| %-18s| %-10s| %-10s| %-8s %-6s %-6s %-6s %-4s |"
#else
#define fmt "| %-18S| %-10S| %-10S| %-8S %-6S %-6S %-6S %-4S |"
#endif

  testLog tl;

  Log.traceln(F("Testing Log.traceln(F())"));
  Log.traceln(PSTR("Testing Log.traceln(PSTR())"));

  tl.logStatus(PSTR("Hello Using PSTR")); // PSTR to msg fails on AVR (works on SAMD)
  sprintf_P(_logStatus, PSTR(fmt),
            PSTR("System"), PSTR("Door"), PSTR("Actuator"), PSTR("Slider"), PSTR("Fwd"), PSTR("Rwd"), PSTR("Speed"), PSTR("%%"));
  Log.noticeln(_logStatus);

  tl.logStatus(F("Hello Using F"));
  sprintf_P(_logStatus, PSTR(fmt),
            F("System"), F("Door"), F("Actuator"), F("Slider"), F("Fwd"), F("Rwd"), F("Speed"), F("%%")); // This gives a compiler warnign on SAMD
  Log.noticeln(_logStatus);

  //   Log.traceln(F("Beaker!"));
  //   Beaker b;
  //   b.begin();
  //   Log.traceln("Beaker State: %d %S", b.getState(), b.getStateString());
  //   Log.traceln("Beaker printTo: %p", b);
  //   for (uint16_t i = 0; i < b.getNumStrings(); i ++){
  //     Log.trace("%d = %S, ", i, b.getString(i));
  //   }
  //   Log.traceln("");

  //   Log.traceln(F("Piggy!"));
  //   Piggy piggy;
  // //  piggy.begin();
  //   Log.traceln("Piggy strings: %d %S", piggy.strings.getNumStrings(), piggy.strings.getString(0));
  //   Log.traceln("Piggy printTo: %p", piggy);
  //   for (uint16_t i = 0; i < piggy.strings.getNumStrings(); i ++){
  //     Log.trace("%d = %S, ", i, piggy.strings.getString(i));
  //   }
  //   Log.traceln("");

  Log.traceln(F("Done!"));
}

void loop()
{
}

// handle diagnostic informations given by assertion and abort program
// execution:
void __assert(const char *__func, const char *__file, int __lineno,
              const char *__sexp)
{
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
