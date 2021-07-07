#include <Arduino.h>
#include <ArduinoLog.h>
#include <assert.h>

#include <FlashStringTable.h>
#include "Machine.h"
#include <Arduino.h>
#include <ArduinoLog.h>
#include <assert.h>

Machine::Machine()
    : stateStrings(_progmem_SystemStates),
      triggerStrings(_progmem_SystemTriggers),
      _state(States::error),
      _trigger(Triggers::None)
{
  Log.traceln("Machine::Machine()");
  _rgStates = (State*) malloc(sizeof(State) * (Machine::States::error+1));

  _rgStates[Machine::States::error] = State(
      []()
      {
    Log.traceln("on_enter"); },
      []()
      {
    Log.traceln("on_state"); },
      []()
      {
    Log.traceln("on_exit"); });

  _pfsm = new Fsm(&_rgStates[_state]);
  _pfsm->add_transition(&_rgStates[Machine::States::error],
                        &_rgStates[Machine::States::error], Machine::Triggers::None, nullptr);
}

bool Machine::begin()
{
  Log.traceln("Machine::begin()");

  return true;
}

void Machine::runStateMachines()
{
  // trigger() ends up callilng old->on_exit, new->on_enter
  //logStatus(F("Trigger: %S"), triggerStrings.getString(_trigger));
  _pfsm->trigger(_trigger);

  // Reset the trigger
  setTrigger(None);

  // runs the current state's on_state then checks for
  // timed transitions
  _pfsm->run_machine();

  // the current state's on_state has returned
  // returning from here puts us back in loop()...
}

void Machine::setTrigger(Triggers trigger)
{
  // Trigger a state transition (asynchronously)
  // TODO: Redraw dispay?

  _trigger = trigger;
  _pfsm->trigger(trigger);
}

void Machine::setCurrentState(States state)
{
  // For diagnostics

  // TODO: Redraw dispay
}

Machine::Triggers Machine::process(States currentState)
{
  // Handle work (from on_state). Return new state transition trigger (or None)
  // DO NOT set _trigger or call setTrigger(); the calling on_state will do so
  Triggers trigger = Triggers::None;

  return trigger;
}

Machine::States Machine::getState()
{
  return _state;
}

size_t Machine::printTo(Print &p) const
{
  return p.print(stateStrings.getString((int)_state));
};






size_t Beaker::printTo(Print &p) const { return p.print(getString(_state)); };

IMPL_FLASH_STRING_TABLE_CLASS(Beaker)

Beaker::Beaker() : _state((States)Nine), _trigger(0)
{
  INIT_FLASH_STRING_TABLE_CLASS(Beaker, States::Ten + 1);
  Log.traceln("Beaker::Beaker()- State: %S", getStateString());

  for (int i = 0; i <= States::Ten; i++)
  {
    Log.traceln("Beaker State id %d = %S", i, getString(i));
    Log.traceln("Beaker State id %d (getString) = %S", i, Beaker::getString(i));
  }
}

bool Beaker::begin()
{
  Log.traceln("Beaker::begin() - _beakerStateStrings = %d, States::Ten = %d",
              getNumStrings(), (int)States::Ten);
  return true;
}

const __FlashStringHelper *Beaker::getStateString()
{
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

Piggy::Piggy() : strings(_progmem_Piggy)
{
  // pstate = &rgStates[0];

  // currentState = 0;
  // g_pfsm = pfsm = new Fsm(&rgStates[currentState]);

  // pfsm->add_transition(&rgStates[0], &rgStates[1], 1, nullptr);
  // pfsm->add_transition(&rgStates[1], &rgStates[0], 1, nullptr);

  // Log.traceln("run_machine");
  // pfsm->run_machine();
  // Log.traceln("run_machine");
  // pfsm->run_machine();
  // Log.traceln("run_machine");
  // pfsm->run_machine();
}

size_t Piggy::printTo(Print &p) const { return p.print(strings.getString(3)); };
