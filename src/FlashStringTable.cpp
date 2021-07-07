/**
 * @author github.com/tig/FlashStringHelper
 * @brief Library to implify definition and use of Arduino flash-based
 * (PROGMEM) global string constants.
 * @date 2021-07-05
 */

#include "FlashStringTable.h"

/**
 * @brief Helper function that determins the size of the string table.
 *
 * @param p progmem pointer
 * @return uint16_t number of entries in the string table (number of
 * null-terminated strings in the doublly-null-terminated buffer)
 */
uint16_t getTableSize(PGM_P p) {
  uint16_t size = 0;
  char c = pgm_read_byte(p);
  for (size = 0; c != 0; size++) {
    for (; c != 0; p++) {
      c = pgm_read_byte(p);
    }
    c = pgm_read_byte(p);
  };
  assert(size > 0);
  return size;
}

/**
 * @brief Creates a table of `__FlashStringHelper*` pointers in RAM. Each
 * pointer in the returned array is a progmem reference into flash that can be
 * used with the `pgm_read_byte()` and related functions to retrive the string
 * data stored in flash.
 *
 * @param p progmem pointer to the string table buffer in flash
 * @param size number of strings in the string table (use `getTableSize()`)
 * @return const __FlashStringHelper** ()
 */
const __FlashStringHelper** createStringTable(PGM_P p, uint16_t size) {
  // Allocated
  const __FlashStringHelper** table;
  table =
      (const __FlashStringHelper**)malloc(sizeof(__FlashStringHelper*) * size);
  char c = pgm_read_byte(p);
  for (uint16_t index = 0; c != 0; index++) {
    table[index] = (__FlashStringHelper*)p++;
    for (; c != 0; p++) {
      c = pgm_read_byte(p);
    }
    c = pgm_read_byte(p);
  };
  return table;
}

