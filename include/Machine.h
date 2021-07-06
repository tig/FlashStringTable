#pragma once

#include <Arduino.h>
#include <Fsm.h>

BEGIN_FLASH_STRING_TABLE_CLASS(Machine)
ADD_FLASH_STRING("Started")
ADD_FLASH_STRING("Stopped")
ADD_FLASH_STRING("Error")
END_FLASH_STRING_TABLE()

/**
 * @brief Abstract (pure v) base class for subystems that are run by a state machine.
 *
 */
class Machine : public StringTable {
 public:
  enum States { Started, Stopped, Error };

 public:
  Machine() : _state(Error), _trigger(0) {
    INIT_FLASH_STRING_TABLE_CLASS(Machine, States::Error + 1);
  };

  Machine(States initialState) : Machine() { _state = initialState; };

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
  // virtual void runStateMachines();

  // Sets the trigger of the next state to transition to
  // Trigger::None (-1) means stay in current state.
  // virtual void setTrigger(int trigger);

  /**
   * @brief Set the Current State object - Used for diagnostics; called from
   * on_enter of each state in statemachine
   *
   * @param stateName
   */
  virtual void setCurrentState(const __FlashStringHelper* stateName);

  // Do processing; called via on_state for states that depend
  // on system. Generally updates status.
  // virtual int process();

  /**
   * @brief Returns current (cached status) of the machine
   *
   * @return int
   */
  virtual States getState();

  /**
   * @brief Returns current (cached status) of the machine as a string. For
   * diagnostics. Same string that `printTo()` will generate.
   *
   * @return const __FlashStringHelper*
   */
  virtual const __FlashStringHelper* getStateString() = 0;

  /**
   * @brief `Printable::printTo` - prints the current State
   *
   * @param p
   * @return size_t
   */
  virtual size_t printTo(Print& p) const { return 0; };

 private:
  DECLARE_FLASH_STRING_TABLE_CLASS(Machine);

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

  // Fsm* _pfsm;
  // State* _states;
  // const char* _f_[];
};

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