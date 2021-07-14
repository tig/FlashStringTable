#include <Arduino.h>
#include <FlashStringTable.h>

BEGIN_FLASH_STRING_TABLE(myFlashStringTable)
ADD_FLASH_STRING("1")
ADD_FLASH_STRING("second")
ADD_FLASH_STRING("third")
END_FLASH_STRING_TABLE()

void setup()
{
    Serial.begin(57600);
    while (!Serial && !Serial.available())
    {
    }
    delay(500);

    // NOTE: Call this only once. It scans the string table in flash and
    // assigns pointers to myFlashStringTable's elements. It uses malloc()
    // to allocated myFlashStringTable[].
    INIT_FLASH_STRING_TABLE(myFlashStringTable);
    // prints 3
    Serial.print(myFlashStringTable_size, DEC);
    // prints "second"
    Serial.println(myFlashStringTable[1]);

    char sz[256];
    Serial.print(F("All strings: "));
    for (uint16_t i = 0; i < myFlashStringTable_size; i++)
    {
        char szTemp[50];
        strcpy_P(szTemp,  (PGM_P)myFlashStringTable[i]);
        sprintf_P(sz, PSTR("%d = %s"), i, szTemp);
        Serial.println(sz);
    }
    Serial.println(F("Done!"));
}

void loop()
{
}
