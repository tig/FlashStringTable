/**
 * @author github.com/tig/FlashStringHelper
 * @brief Library to implify definition and use of Arduino flash-based
 * (PROGMEM) global string constants.
 * @date 2021-07-05
 */
#pragma once

#include <Arduino.h>
#include <assert.h>

/**
 * @brief Get the numeber of null terminated strings in a doubly-null terminated
 * buffer held in flash memory
 *
 * @param p - pointer to buffer in flash memory
 * @return uint16_t
 */
uint16_t getTableSize(PGM_P p);

/**
 * @brief Given a pointer to a buffer of doubly-null terminated strings
 * in flash memory, allocates an array of _FlashStringHelper* and assigns each
 * to the flash pointer to a string found in the flash memory.
 *
 * @param p - pointer to buffer in flash memory
 * @param size - number of strings expected. The reulsting array will be [size]
 * elements
 */
const __FlashStringHelper** createStringTable(PGM_P p, uint16_t size);

/**
 * @brief These macros simplify definition and use of Arduino flash-based
 * (PROGMEM) global string constants. There is a small startup cost to using
 * these as each table of strings is parsed by the `INIT_FLASH_STRING_TABLE`
 * macro to fill in the table of __FlashStringHelper* pointers.
 *
 * C Example:
 *
 * #include <Arduino.h>
 * #include "FlashStringTable.h"
 * BEGIN_FLASH_STRING_TABLE(myFlashStringTable)
 *   ADD_FLASH_STRING("1")
 *   ADD_FLASH_STRING("second")
 *   ADD_FLASH_STRING("third")
 * END_FLASH_STRING_TABLE()
 *
 * vois Setup(){
 *    // NOTE: Call this only once. It scans the string table in flash and
 *    // assigns pointers to myFlashStringTable's elements. It uses malloc()
 *    // to allocated myFlashStringTable[].
 *    INIT_FLASH_STRING_TABLE(myFlashStringTable);
 *
 *    // prints 3
 *    Serial.print(myFlashStringTable_size, DEC);
 *    // prints "second"
 *    Serial.println(myFlashStringTable[1]);
 * }
 *
 */

/**
 * @brief Starts the definition of a flash string table 'C' style.
 * The `table` and `_progmem_table` will be global variables.
 *
 * Finish with `END_FLASH_STRING_TABLE`
 *
 */
#define BEGIN_FLASH_STRING_TABLE(string_table_name) \
  __FlashStringHelper** string_table_name;          \
  const char _progmem_##string_table_name[] PROGMEM =

/**
 * @brief Starts the definition of a flash string table 'C++' style.
 * The `_progmem_table[]` wil be a global variable.
 *
 * Use `DECLARE_FLASH_STRING_TABLE_CLASS` within a class definition
 * to delcar the `table` variable as a class member,
 *
 * Finish with `END_FLASH_STRING_TABLE`
 *
 */
#define BEGIN_FLASH_STRING_TABLE_CLASS(classname) \
  const char _progmem_##classname[] PROGMEM =

/**
 * @brief Adds a flash string to the string table in flash.
 *
 */
#define ADD_FLASH_STRING(s) s "\0"

/**
 * @brief Ends the definiiton of a flash-based string table.
 * Brackets `BEGIN_FLASH_STRING_TABLE and `BEGIN_FLASH_STRING_TABLE_CLASS`
 *
 */
#define END_FLASH_STRING_TABLE() "\0";

/**
 * @brief Initalizes the string table 'C' style, using two varialbes declared
 * at global scope: `table` (thet table of pointers to flash) and
 * `_progmem_table` (the doubly-null terminated string built by the
 * `ADD_FLASH_STRING` macros).
 *
 * Call this near program start. Call it only once.
 *
 */
#define INIT_FLASH_STRING_TABLE(string_table_name)                      \
  static uint16_t string_table_name##_size = 0;                         \
  {                                                                     \
    PGM_P p = reinterpret_cast<PGM_P>(_progmem_##string_table_name);    \
    string_table_name##_size = getTableSize(p);                         \
    string_table_name =                                                 \
        (__FlashStringHelper**)createStringTable(p, string_table_name##_size); \
  }

/**
 * @brief Declares the string table 'C++' style. `table` (the table of pointers
 * to flash) will be a member of whatever class this macro is included in. And
 * `_progmem_table` (the doubly-null terminated string built by the
 * `ADD_FLASH_STRING` macros) will be a variable declared at global scope.
 *
 * Put this macro somewhere within the private: parts of a class definition.
 *
 */
#define DECLARE_FLASH_STRING_TABLE_CLASS(classname)                       \
 public:                                                                  \
  virtual const __FlashStringHelper* getString(uint16_t i) const {        \
    return _pstrings##classname[i];                                       \
  }                                                                       \
  virtual uint16_t getNumStrings() const { return _nstrings##classname; } \
                                                                          \
 private:                                                                 \
  static const __FlashStringHelper** _pstrings##classname;                \
  static uint16_t _nstrings##classname;

/**
 * @brief Put this in your .cpp file
 *
 */
#define IMPL_FLASH_STRING_TABLE_CLASS(classname)                \
  const __FlashStringHelper** classname ::_pstrings##classname; \
  uint16_t classname ::_nstrings##classname = 0;

/**
 * @brief Initalizes the string table 'C++' style. `table` (the table of
 * pointers to flash) will be a member of whatever class this macro is included
 * in. And `_progmem_table` (the doubly-null terminated string built by the
 * `ADD_FLASH_STRING` macros) will be a variable declared at global scope.
 *
 * Call this in the constructor of the class. It will get called each time
 * an instance of the class is created, but will only do it's thing the first
 * time.
 *
 */
#define INIT_FLASH_STRING_TABLE_CLASS(classname, verify_count) \
  if (classname ::_nstrings##classname == 0) {                 \
    PGM_P p = reinterpret_cast<PGM_P>(_progmem_##classname);   \
    classname ::_nstrings##classname = getTableSize(p);        \
    classname ::_pstrings##classname =                         \
        (const __FlashStringHelper**)createStringTable(               \
            p, classname ::_nstrings##classname);              \
    assert(classname ::_nstrings##classname == verify_count);  \
  }

/**
 * @brief Provides simple way of accessing a set of strings
 * in flash memory.
 *
 * To use, use the `BEGIN_FLASH_STRING_TABLE(name)`, `ADD_FLASH_STRING()`, and
 * `END_FLASH_STRING_TABLE()` macros to define the strings. Create a
 * `StringTable` object passing `_progmem_name` to the initalizier (e.g.
 * `StringTable myStrings(_progman_MyStrings)`.
 *
 */
class StringTable : public Printable {
 public:
  StringTable(const char* pprogmem) : StringTable() {
    PGM_P p = reinterpret_cast<PGM_P>(pprogmem);
    _nstrings = getTableSize(p);
    _pstrings = (const __FlashStringHelper**)createStringTable(p, _nstrings);
  }
  StringTable() {
    _nstrings = 0;
    _pstrings = nullptr;
  }

  /**
   * @brief Gets a pointer to string `i` in flash memory.
   * The resultant pointer can be used with any function that
   * takes a `__FlashStringHelper*`.
   *
   * e.g. `Serial.println(st.getString(ID_ERROR));`
   *
   * @param i zero-based index into string table
   * @return const __FlashStringHelper*
   */
  virtual const __FlashStringHelper* getString(uint16_t i) const {
    return _pstrings[i];
  }

  /**
   * @brief returns the number of items in the string table.
   *
   * @return uint16_t
   */
  virtual uint16_t getNumStrings() const { return _nstrings; }

  /**
   * @brief Supports outputing this object to any function that supports
   * `Printable`. Output is all strings, comma-delimited.
   *
   * @param p
   * @return size_t
   */
  size_t printTo(Print& p) const {
    uint16_t n = 0;
    for (uint16_t i = 0; i < _nstrings; i++) {
      n += p.print(_pstrings[i]);
      if (i + 1 != _nstrings) {
        n += p.print(", ");
      }
    }
    return n;
  };

 private:
  static const __FlashStringHelper** _pstrings;
  static uint16_t _nstrings;
};
