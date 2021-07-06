#include <Arduino.h>
#include <ArduinoLog.h>
// #include <Fsm.h>
#include <assert.h>

#include "FlashStringTable.h"
#include "Machine.h"

IMPL_FLASH_STRING_TABLE_CLASS(Machine)

bool Machine::begin() {
  Log.traceln("Machine::begin() - _stateStrings = %d, States::Error = %d",
              getNumStrings(), (int)States::Error);

  for (uint16_t i = 0; i < getNumStrings(); i++) {
    Log.traceln("Machine [%d] = %S", i, getString(i));
  }

  return true;
}

void Machine::setCurrentState(const __FlashStringHelper* stateName) {
  // Log.traceln("Machine::setCurrentState()");
  _state = Error;
}

Machine::States Machine::getState() {
  // Log.traceln("Machine::getState() - %d", _state);
  return _state;
}

// const __FlashStringHelper *Machine::getStateString() {
//   Log.traceln("Machine::getStateString()");
//   return Machine::_pstrings[_state];
// };

size_t Beaker::printTo(Print& p) const { return p.print(getString(_state)); };

IMPL_FLASH_STRING_TABLE_CLASS(Beaker)

Beaker::Beaker() : _state((States)Nine), _trigger(0) {
  INIT_FLASH_STRING_TABLE_CLASS(Beaker, States::Ten + 1);
  Log.traceln("Beaker::Beaker()- State: %S", getStateString());

  for (int i = 0; i <= States::Ten; i++) {
    Log.traceln("Beaker State id %d = %S", i, getString(i));
    Log.traceln("Beaker State id %d (getString) = %S", i, Beaker::getString(i));
  }
}

bool Beaker::begin() {
  Log.traceln("Beaker::begin() - _beakerStateStrings = %d, States::Ten = %d",
              getNumStrings(), (int)States::Ten);
  return true;
}

const __FlashStringHelper* Beaker::getStateString() {
  Log.traceln("Machine::getStateString()");
  return getString(_state);
};

BEGIN_FLASH_STRING_TABLE_CLASS(Piggy)
ADD_FLASH_STRING("Mrs")
ADD_FLASH_STRING("Piggy")
ADD_FLASH_STRING("Loves")
ADD_FLASH_STRING("Kermit")
ADD_FLASH_STRING("The")
ADD_FLASH_STRING("Frog")
ADD_FLASH_STRING("!")
END_FLASH_STRING_TABLE()

Fsm* g_pfsm = nullptr;

State state1 = {[](void* ctx) {
                 Piggy* pig = (Piggy*)ctx;
                 pig->currentState = 1;
                 Log.traceln("%d on_enter", pig->currentState);
               },
               [](void* ctx) {
                 Piggy* pig = (Piggy*)ctx;
                 pig->currentState = 1;
                 Log.traceln("%d on_state", pig->currentState);
                 pig->pfsm->trigger(1, ctx);
               },
               [](void* ctx) {
                 Piggy* pig = (Piggy*)ctx;
                 pig->currentState = 1;
                 Log.traceln("%d on_exit", pig->currentState);
               }};

State rgStates[] = {
    // State 1
    state1,
    // State 1
    {[](void* ctx) {
       Piggy* pig = (Piggy*)ctx;
       pig->currentState = 2;
       Log.traceln("%d on_enter", pig->currentState);
     },
     [](void* ctx) {
       Piggy* pig = (Piggy*)ctx;
       pig->currentState = 2;
       Log.traceln("%d on_state", pig->currentState);
       g_pfsm->trigger(1, ctx);
     },
     [](void* ctx) {
       Piggy* pig = (Piggy*)ctx;
       pig->currentState = 2;
       Log.traceln("%d on_exit", pig->currentState);
     }}};

// State state1([](void * ctx) { Log.traceln("1 on_enter"); },
//              [](void * ctx) {
//                Log.traceln("1 on_state");
//                g_pfsm->trigger(1, ctx);
//              },
//              []() { Log.traceln("1 on_exit"); });

// State state2([]() { Log.traceln("2 on_enter"); },
//              []() { Log.traceln("2 on_state"); },
//              []() { Log.traceln("2 on_exit"); });

Piggy::Piggy() : strings(_progmem_Piggy) {
  pstate = &rgStates[0];

  currentState = 0;
  g_pfsm = pfsm = new Fsm(&rgStates[currentState]);

  pfsm->add_transition(&rgStates[0], &rgStates[1], 1, nullptr);
  pfsm->add_transition(&rgStates[1], &rgStates[0], 1, nullptr);

  Log.traceln("run_machine");
  pfsm->run_machine(this);
  Log.traceln("run_machine");
  pfsm->run_machine(this);
  Log.traceln("run_machine");
  pfsm->run_machine(this);
}

size_t Piggy::printTo(Print& p) const { return p.print(strings.getString(3)); };
