#include <Arduino.h>
#include <FlashStringTable.h>

BEGIN_FLASH_STRING_TABLE(myFlashStringTable)
    ADD_FLASH_STRING("1")
    ADD_FLASH_STRING("second")
    ADD_FLASH_STRING("third")
END_FLASH_STRING_TABLE()

void setup() {
   // NOTE: Call this only once. It scans the string table in flash and
   // assigns pointers to myFlashStringTable's elements. It uses malloc()
   // to allocated myFlashStringTable[].
   INIT_FLASH_STRING_TABLE(myFlashStringTable);
   // prints 3
   Serial.print(myFlashStringTable_size, DEC);
   // prints "second"
   Serial.println(myFlashStringTable[1]);
}

void loop() {
    
}
