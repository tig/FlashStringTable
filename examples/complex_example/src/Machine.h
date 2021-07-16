#pragma once

#include <FlashStringTable.h>
#include <Fsm.h>

BEGIN_FLASH_STRING_TABLE_CLASS(MachineStates)
ADD_FLASH_STRING("error")
END_FLASH_STRING_TABLE()

BEGIN_FLASH_STRING_TABLE_CLASS(MachineTriggers)
ADD_FLASH_STRING("None")
END_FLASH_STRING_TABLE()

/**
 * @brief Shortcut for ensuring a on_enter/state/exit function can be setup/traced easily
 * e.g. TRACE_STATE_FN(MainMachine, on_enter, true);
 * 
 * @param machine classname of the `Machine` subclass.
 * @param fn on_enter, on_state, or on_exit
 * @param trace if `true` Log.traceln will be called 
 */
#define TRACE_STATE_FN(machine, fn, trace)                                                                            \
  machine& temp##machine = machine::getInstance();                                                                    \
  if (trace) {                                                                                                        \
    Log.traceln(F(#machine " %S state - " #fn), temp##machine._stateStrings.getString(temp##machine.getCurrentState())); \
  }

/**
 * @brief Shortcut for ensuring a on_enter function can be setup/traced easily. 
 * e.g. TRACE_STATE_FN(MainMachine, true);
 * 
 * @param machine classname of the `Machine` subclass.
 * @param trace if `true` Log.traceln will be called 
 */
#define TRACE_STATE_ENTER_FN(machine, trace)                                                                              \
  machine& temp##machine = machine::getInstance();                                                                        \
  if (trace) {                                                                                                            \
    Log.traceln(F(#machine " %S state - on_enter"), temp##machine._stateStrings.getString(temp##machine.getCurrentState())); \
  }                                                                                                                       

// Because c/c++ does not support polymorhpic enums we can't really pass our nice
// enums around strongly typed. So we just an int. And silly defines to make the code
// more readable
#define StateType   uint16_t
#define TriggerType uint16_t

/**
 * @brief Abstract (pure v) base class for subystems that are run by a state machine.
 *
 */
class Machine : public Printable {
 public:
  /**
   * @brief The states the Machine's FSM can be in. Must have the same
   * number of items as _stateStrings[]
   * 
   */
  enum States
  { error };

  /**
   * @brief The events that can trigger the Machine FSM to transition to another state.
   *  Must have the same number of items as _stateStrings[]
   * 
   */
  enum Triggers
  { None };

  /**
   * @brief Flash-memory based strings for the States
   * 
   */
  StringTable _stateStrings = _progmem_MachineStates;

  /**
   * @brief Flash-memory based strings for the States
   * 
   */
  StringTable _triggerStrings = _progmem_MachineTriggers;

  /**************** BEGIN Fsm Wrappers *****/
  /**
   * @brief Allocates the state machine implementation. Call from the end of 
   * implementaitons of `begin()` before using `_rgpStates`
   * 
   */
  void allocateFsm(StateType lastState, TriggerType lastTrigger);

  /**
   * @brief Allocates the state machine implementation. Call from the end of 
   * implementaitons of `begin()` before using `_rgpStates`
   * 
   */
  void setStartState(StateType state);

  /**
   * trigger transition with the event 
   * @param trigger enum that defines the trigger
   * @param bool true (default) causes state transition to happen during the call to trigger (synchronously)
   * @param bool false sets a flag such that the state transition will happen in the next call to run_machine()
   */
  void trigger(TriggerType trigger, bool immediate = true);

  /**
   * iterate one run-cycle of the state machine
   */
  void runMachine();

  /**
   * @brief Adds a transition
   * 
   * @param stateFrom
   * @param stateTo
   * @param trigger 
   * @param on_transition 
   */
  void addTransition(StateType stateFrom, StateType stateTo, TriggerType trigger, void (*on_transition)() = nullptr);

  /**
   * @brief Adds a timed transition
   * 
   * @param stateFrom
   * @param stateTo
   * @param interval 
   * @param on_transition 
   */
  void addTimedTransition(StateType stateFrom, StateType stateTo, unsigned long interval, void (*on_transition)() = nullptr);

  /**
   * returns current state (helpful if the same handler is used to drive many similar states)
   * @return index of current state
   */
  StateType getCurrentState() const;

  /**************** END Fsm Wrappers *****/

  /**
   *
   * @brief Initialize state machine. Should be called at system startup.
   * Subclasses should override this and call Machine::begin() before doing their
   * own begin() processing.
   *
   * @return true if setup worked
   * @return false if setup failed
   */
  virtual bool begin();

  /**
   * @brief Runs the sub-system's state machine. Called from the holding
   * object's `runSubMachines()`; the top level machine (`System`)'s
   * `runMachine` is called from `loop()`.
   *
   */
  virtual void runSubMachines();

  /**
   * @brief Set the Trigger objectSets the trigger of the next state to transition to. 
   * The trigger will happen async. Trigger::None means stay in current state.
   * 
   * @param trigger 
   */
  virtual void setTrigger(TriggerType trigger);

    /**
   * @brief Do machine work while in a state (to be called from `on_state` handlers).
   * 
   * @param stateCalling - passed as a convenience so on_state doesn't have to call getState().
   * @return Trigger - a trigger for a state transition. The `on_state` will then use this to
   * initate a state transition (by calling the appropriate `setTrigger()`)
   */
  virtual TriggerType on_state(StateType stateCalling = Machine::States::error);

  /**
   * @brief `Printable::printTo` - prints the current State & Trigger
   * 'State(Trigger)'
   *
   * @param p
   * @return size_t
   */
  virtual size_t printTo(Print& p) const;

  /**
   * @brief Set by a state machine function to trigger next state
   *
   */
  TriggerType _trigger = 0;

  /**
   * @brief array of pointers to State objects
   * 
   */
  State** _rgpStates = nullptr;

 private:
  // /**
  //  * @brief Current state of the machine
  //  *
  //  */
  // StateType _state = 0;

  /**
   * @brief pointer to arduino-fsm `Fsm` instance
   * 
   */
  Fsm* _pFsm = nullptr;

 private:
  uint16_t _numStates = States::error + 1;
  uint16_t _numTriggers = Triggers::None + 1;

  // =======================================================
  // Singleton support
  // https://stackoverflow.com/a/1008289/297526
 public:
  /**
   * @brief Returns the single instance object
   *
   */
  static Machine& getInstance() {
    static Machine instance;  // Guaranteed to be destroyed.
                              // Instantiated on first use.
    return instance;
  }

  // NOTE: Normally for a singleton the constructor would be private
  // Prohibiting External Constructions. But Machine is
  // actually abstract and we only ...
  // TODO: Fix this bs.
 public:
  Machine() {
    // Don't do anything in here because these objects are static
    // and thus constructors are called before Serial or other
    // stuff is available, making debugging hard. Use begin() instead.
  }

 private:
  // C++ 11
  // =======
  // We can use the better technique of deleting the methods
  // we don't want.
 public:
  // This breaks printTo
  //Machine(Machine const&) = delete;
  void operator=(Machine const&) = delete;
  // =======================================================
};
