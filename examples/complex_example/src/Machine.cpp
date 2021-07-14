#include <Arduino.h>
#include <ArduinoLog.h>
#include <assert.h>

#include "Machine.h"

void Machine::allocateFsm(StateType lastState, TriggerType lastTrigger) {
  Log.traceln(F("Machine::allocateFsm(%d, %d)"), lastState, lastTrigger);
  assert(_rgpStates == nullptr);

  _numStates = lastState + 1;
  _numTriggers = lastTrigger + 1;

  assert(_numStates == _stateStrings.getNumStrings());
  assert(_numTriggers == _triggerStrings.getNumStrings());

  _rgpStates = (State **)new State[_numStates];
};

void Machine::setStartState(StateType state) {
  Log.traceln(F("Machine::setStartState(%d)"), state);
  assert(_rgpStates != nullptr);
  assert(_pFsm == nullptr);
  assert(state < _numStates);

  for (StateType i = 0; i < _numStates; i++) {
    assert(_rgpStates[i] != nullptr);
  }
  _pFsm = new Fsm(_rgpStates[state]);
};

void Machine::trigger(TriggerType trigger, bool immediate) {
  //Log.traceln(F("Machine::trigger(%d, %d)"), trigger, immediate);
  assert(_rgpStates != nullptr);
  assert(_pFsm != nullptr);
  assert(trigger < _numTriggers);
  _pFsm->trigger(trigger, immediate);
}

void Machine::runMachine() {
  //Log.traceln(F("Machine::runMachine()"));
  assert(_rgpStates != nullptr);
  assert(_pFsm != nullptr);
  _pFsm->run_machine();
}

void Machine::addTransition(StateType stateFrom, StateType stateTo, TriggerType trigger, void (*on_transition)()) {
  //Log.traceln(F("Machine::addTransition(%d, %d, %d)"), stateFrom, stateTo, trigger);
  assert(_pFsm != nullptr);
  assert(_rgpStates != nullptr);
  assert(stateFrom < _numStates);
  assert(stateTo < _numStates);
  assert(trigger < _numTriggers);
  assert(_rgpStates[stateFrom] != nullptr);
  assert(_rgpStates[stateTo] != nullptr);
  _pFsm->add_transition(_rgpStates[stateFrom], _rgpStates[stateTo], trigger, on_transition);
}

void Machine::addTimedTransition(StateType stateFrom, StateType stateTo, unsigned long interval, void (*on_transition)()) {
  //Log.traceln(F("Machine::addTimedTransition(%d, %d, %d)"), stateFrom, stateTo, interval);
  assert(_pFsm != nullptr);
  assert(_rgpStates != nullptr);
  assert(stateFrom < _numStates);
  assert(stateTo < _numStates);
  assert(_rgpStates[stateFrom] != nullptr);
  assert(_rgpStates[stateTo] != nullptr);
  _pFsm->add_timed_transition(_rgpStates[stateFrom], _rgpStates[stateTo], interval, on_transition);
}

StateType Machine::getCurrentState() const {
  if (_pFsm == nullptr) {
    return States::error;
  }
  assert(_rgpStates != nullptr);

  StateType state;
  State *pCurState = _pFsm->get_current_state();
  if (pCurState == nullptr) {
    state = States::error;
  } else {
    for (state = 0; state < _numStates; state++) {
      if (_rgpStates[state] == pCurState) {
        break;
      }
    }
  }
  assert(state < _numStates);
  return state;
}

bool Machine::begin() {
  Log.traceln(F("Machine::begin()"));

  assert(States::error + 1 == _stateStrings.getNumStrings());

  _rgpStates = nullptr;
  _pFsm = nullptr;

  // this code block illustrates a typical `begin()` implementation
  allocateFsm(States::error, Triggers::None);

  _rgpStates[error] = new State(
      []() {
        TRACE_STATE_FN(Machine, on_enter, true);
      },
      []() {
        TRACE_STATE_STATE_FN(Machine, false);
      },
      []() {
        TRACE_STATE_FN(Machine, on_exit, true);
      });

  setStartState(error);

  return true;
}

void Machine::runSubMachines() {
  //Log.traceln(F("Machine::runSubMachines()"));
  assert(_rgpStates != nullptr);
  assert(_pFsm != nullptr);

  // Trigger an outstanding trigger if there is one
  if (_trigger != Machine::Triggers::None) {
    // trigger() ends up callilng old->on_exit, new->on_enter
    Log.traceln(F("Machine::runSubMachines - trigger(%S)"), _triggerStrings.getString(_trigger));
    trigger(_trigger, true);
    // Reset the trigger
    //Log.traceln(F("Machine::runSubMachines - trigger(%S) completed, resetting trigger and exiting..."), _triggerStrings.getString(_trigger));
    setTrigger(Machine::Triggers::None);
    return;
  }

  // runs the current state's on_state then checks for
  // timed transitions
  runMachine();

  // the current state's on_state has returned
  // returning from here puts us back in loop()...
}

void Machine::setTrigger(TriggerType trigger) {
  assert(_rgpStates != nullptr);
  assert(_pFsm != nullptr);
  //Log.traceln(F("Machine::setTrigger(%S)"), _triggerStrings.getString(trigger));
  // Trigger a state transition (asynchronously)
  // TODO: Redraw dispay?

  _trigger = trigger;
}

void Machine::stateChanged(StateType state) {
  assert(_rgpStates != nullptr);
  assert(_pFsm != nullptr);
  // For diagnostics

  // TODO: Redraw dispay
}

TriggerType Machine::process(StateType stateCalling) {
  assert(_rgpStates != nullptr);
  assert(_pFsm != nullptr);

  Log.traceln(F("Machine::process(%S)"), _stateStrings.getString(stateCalling));
  // Handle work (from on_state). Return new state transition trigger (or None)
  // DO NOT set trigger or call setTrigger(); the calling on_state will do so
  TriggerType trigger = Triggers::None;

  return trigger;
}

size_t Machine::printTo(Print &p) const {
  return p.print(_stateStrings.getString(getCurrentState()));
};
