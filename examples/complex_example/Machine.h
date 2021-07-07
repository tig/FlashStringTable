#pragma once

#include <Arduino.h>
#include <Fsm.h>
#include <FlashStringTable.h>

BEGIN_FLASH_STRING_TABLE_CLASS(SystemStates)
ADD_FLASH_STRING("error")
END_FLASH_STRING_TABLE()

BEGIN_FLASH_STRING_TABLE_CLASS(SystemTriggers)
ADD_FLASH_STRING("None")
END_FLASH_STRING_TABLE()

/**
 * @brief Abstract (pure v) base class for subystems that are run by a state machine.
 *
 */
class Machine : public Printable {
 public:
  /**
 * @brief The states the Machine's FSM can be in. Must have the same
 * number of items as stateStrings[]
 * 
 */
  enum States
  { error };

  /**
 * @brief The events that can trigger the Machine FSM to transition to another state.
 *  Must have the same number of items as stateStrings[]
 * 
 */
  enum Triggers
  { None };

  Machine();
  Machine(States initialState)
      : Machine() { _state = initialState; };

  /**
   * @brief Flash-memory based strings for the States
   * 
   */
  StringTable stateStrings;

  /**
   * @brief Flash-memory based strings for the States
   * 
   */
  StringTable triggerStrings;

 public:
  /**
   *
   * @brief Initiazlie object. Called at system startup.
   *
   * @return true if setup worked
   * @return false if setup failed
   */
  virtual bool begin();

  /**
   * @brief Runs the sub-system's state machine. Called from the holding
   * object's `runStateMachines()`; the top level machine (`System`)'s
   * `runMachine` is called from `loop()`.
   *
   */
  virtual void runStateMachines();

  /**
   * @brief Set the Trigger objectSets the trigger of the next state to transition to. 
   * The trigger will happen async. Trigger::None means stay in current state.
   * 
   * @param trigger 
   */
  virtual void setTrigger(Triggers trigger);

  /**
   * @brief Set the Current State object - Used for diagnostics; called from
   * on_enter of each state in statemachine
   *
   * @param state
   */
  virtual void setCurrentState(States state);

  /**
   * @brief Do machine work while in a state (to be called from `on_state` handlers).
   * 
   * @param currentState - passed as a convenience so on_state doesn't have to call getState().
   * @return Trigger - a trigger for a state transition. The `on_state` will then use this to
   * initate a state transition (by calling the appropriate `setTrigger()`)
   */
  virtual Triggers process(States currentState);

  /**
   * @brief Returns current (cached) state of the machine
   *
   * @return States
   */
  virtual States getState();

  /**
   * @brief `Printable::printTo` - prints the current State & Trigger
   * 'State(Trigger)'
   *
   * @param p
   * @return size_t
   */
  virtual size_t printTo(Print& p) const;

 private:
  /**
   * @brief Current state of the machine
   *
   */
  Machine::States _state;

  /**
   * @brief Set by a state machine function to trigger next state
   *
   */
  Machine::Triggers _trigger;

  State* _rgStates;// = g_rgSystemStates;
  Fsm* _pfsm;
  // const char* _f_[];
};

//static State *g_rgSystemStates; //[Machine::States::error + 1];// = {

BEGIN_FLASH_STRING_TABLE_CLASS(BeakerStates)
ADD_FLASH_STRING("error1")
END_FLASH_STRING_TABLE()

BEGIN_FLASH_STRING_TABLE_CLASS(BeakerTriggers)
ADD_FLASH_STRING("None2")
END_FLASH_STRING_TABLE()


BEGIN_FLASH_STRING_TABLE_CLASS(Beaker)
ADD_FLASH_STRING("Z")
ADD_FLASH_STRING("O")
ADD_FLASH_STRING("T")
ADD_FLASH_STRING("Th")
ADD_FLASH_STRING("Err")
ADD_FLASH_STRING("Nine")
ADD_FLASH_STRING("Ten")
END_FLASH_STRING_TABLE()

class Beaker : public Machine {
 public:
  enum States { Z, O, T, Th, Err, Nine, Ten };
  Beaker();
  bool begin();
  const __FlashStringHelper *getStateString() ;
  virtual size_t printTo(Print& p) const;

 private:
  DECLARE_FLASH_STRING_TABLE_CLASS(Beaker);
  /**
   * @brief Current state of the machine
   *
   */
  States _state;

  /**
   * @brief Set by a state machine function to trigger next state
   *
   */
  int _trigger;

};




class Piggy : public Printable{
  public:
    Piggy() ;
    size_t printTo(Print& p) const;

    StringTable strings;

    Fsm* pfsm;
    State* pstate;
    int currentState;
};

