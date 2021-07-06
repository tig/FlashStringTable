# FlashStringTable - A simple way of storing and accessing strings in flash memory for Arduino

This Arduino library simplifies the definition and use of Arduino flash-based
(PROGMMEM) global string constants. It supports both `C` and `C++` style coding.

C Example:

```c
#include <Arduino.h>
#include <FlashStringTable.h>
BEGIN_FLASH_STRING_TABLE(myFlashStringTable)
    ADD_FLASH_STRING("1")
    ADD_FLASH_STRING("second")
    ADD_FLASH_STRING("third")
END_FLASH_STRING_TABLE()
void Setup() {
   // NOTE: Call this only once. It scans the string table in flash and
   // assigns pointers to myFlashStringTable's elements. It uses malloc()
   // to allocated myFlashStringTable[].
   INIT_FLASH_STRING_TABLE(myFlashStringTable);
   // prints 3
   Serial.print(myFlashStringTable_size, DEC);
   // prints "second"
   Serial.println(myFlashStringTable[1]);
}

void loop() {}
```
C++ (class-based) example:

```c++
#include <Arduino.h>
#include <FlashStringTable.h>

BEGIN_FLASH_STRING_TABLE_CLASS(Piggy)
    ADD_FLASH_STRING("Mrs")
    ADD_FLASH_STRING("Piggy")
    ADD_FLASH_STRING("Loves")
    ADD_FLASH_STRING("Kermit")
    ADD_FLASH_STRING("The")
    ADD_FLASH_STRING("Frog")
    ADD_FLASH_STRING("!")
END_FLASH_STRING_TABLE()

class Piggy : public Printable {
  public:
    Piggy() : strings(_progmem_Piggy) {} ;
    size_t printTo(Print& p) const { return p.print(strings); };;

    StringTable strings;
};

void startup() {
    while (!Serial && !Serial.available()) {}

    Piggy piggy;
    
    char sz[256];
    sprintf_p(sz, F("Piggy has %d strings"), piggy.strings.getNumStrings());
    Serial.println(sz);


    Serial.print(F("Piggy's printTo() says: "));
    Serial.println(piggy);

    Serial.print(F("Piggy's strings: "));
    for (uint16_t i = 0; i < piggy.strings.getNumStrings(); i ++){
        sprintf_p(sz, F("%d = %s, "), i, piggy.strings.getString(i));
        Serial.print(sz);
    }
    Serial.println(F("Done!"));
}

void loop() {}

```


There is a small startup cost to using this library as each table of strings is parsed by the `INIT_FLASH_STRING_TABLE`
macro to fill in the table of __FlashStringHelper* pointers. However, 