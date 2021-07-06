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

void setup() {
    while (!Serial && !Serial.available()) {}

    Piggy piggy;
    
    char sz[256];
    sprintf_P(sz, PSTR("Piggy has %d strings"), piggy.strings.getNumStrings());
    Serial.println(sz);


    Serial.print(PSTR("Piggy's printTo() says: "));
    Serial.println(piggy);

    Serial.print(PSTR("Piggy's strings: "));
    for (uint16_t i = 0; i < piggy.strings.getNumStrings(); i ++){
        sprintf_P(sz, PSTR("%d = %s, "), i, piggy.strings.getString(i));
        Serial.print(sz);
    }
    Serial.println(PSTR("Done!"));
}

void loop() {}