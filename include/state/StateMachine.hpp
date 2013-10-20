/*
 * Copyright (c) 2012 Christopher M. Baker
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#ifndef STATEMACHINE_HPP_
#define STATEMACHINE_HPP_

#include "../StackTrace.hpp"
#include "../Event.hpp"

#include <map>
#include <set>

namespace nitrus {

template <typename StateType, typename TriggerType> class StateMachine {
public:

	/**
	 * A class that encapsulates an attempt to transition to a new state from an undefined trigger.
	 */
	class UndefinedTriggerException : public std::runtime_error {
	public:

		/**
		 * Creates a new undefined trigger exception.
		 */
		UndefinedTriggerException() : std::runtime_error(__METHOD__) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Deletes the undefined trigger exception.
		 */
		virtual ~UndefinedTriggerException() throw() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
	};

	/**
	 * A class that encapsulates an attempt to transition to multiple states from a single trigger.
	 */
	class MultipleTriggerException : public std::runtime_error {
	public:

		/**
		 * Creates a new multiple trigger exception.
		 */
		MultipleTriggerException() : std::runtime_error(__METHOD__) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Deletes the multiple trigger exception.
		 */
		virtual ~MultipleTriggerException() throw() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
	};

	/**
	 * A class that provides functionality for configuring transitions and events for a single state.
	 */
	class StateConfiguration {
	private:

		/**
		 * A class that provides functionality for configuring conditional transitions for a trigger in a single state.
		 */
		class TriggerConfiguration {
		private:
			StateType _destination;
			Delegate<bool()> _predicate;

		public:

			/**
			 * Creates a new trigger configuration.
			 *
			 * @param destination The state to transition to.
			 * @param predicate The conditional function determining whether the transition is valid.
			 */
			TriggerConfiguration(StateType destination, const Delegate<bool()>& predicate) : _destination(destination), _predicate(predicate) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

			}

			/**
			 * Determines whether this trigger allows a transition.
			 *
			 * @return True if a transition is allowed, false otherwise.
			 */
			bool CanTransition() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
				return _predicate();
			}

			/**
			 * Returns the state to transition to should a transition be allowed.
			 *
			 * @return The state to transition to.
			 */
			StateType Destination() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
				return _destination;
			}

			/**
			 * Deletes the trigger configuration.
			 */
			virtual ~TriggerConfiguration() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

			}
		};

	public:
		typedef Delegate<StateConfiguration& (StateType)> LookupFunction;
		typedef std::set<StateType> ParentStates;
		typedef std::multimap<TriggerType, TriggerConfiguration> TriggerConfigurations;

		/**
		 * This is the default predicate function that is used if a predicate is not specified.
		 * This function will always allow the transition to be made.
		 *
		 * @return True
		 */
		static bool DefaultPredicate() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return true;
		}

	private:
		LookupFunction _lookup;
		ParentStates _superStates;
		TriggerConfigurations _triggerConfigurations;
		Delegate<void()> _stateEntered;
		Delegate<void()> _stateExited;

	public:

		/**
		 * Creates a new empty state configuration.
		 * This constructor should not be invoked explicitly, rather is only provided to allow use of the StateConfigurations type without dynamic allocation.
		 * Because the the lookup function is unassigned, use of this default instance will always throw an exception when the lookup is invoked.
		 */
		StateConfiguration() : _lookup(), _superStates(), _triggerConfigurations(), _stateEntered(), _stateExited() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Creates a new state configuration.
		 *
		 * @param lookup The lookup function used to resolve transitions for sub-states.
		 */
		StateConfiguration(const LookupFunction& lookup) : _lookup(lookup), _superStates(), _triggerConfigurations(), _stateEntered(), _stateExited() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Allows a transition to occur from this state to another state.
		 * This transition only occurs on the specified trigger and if and only if the predicate returns true.
		 *
		 * @param trigger The trigger that, if fired, will signal this transition.
		 * @param destination The state to transition to.
		 * @param predicate The optional conditional function used to allow or prevent a transition.
		 * @return A reference to this state configuration.
		 */
		StateConfiguration& Permit(TriggerType trigger, StateType destination, const Delegate<bool()>& predicate = Delegate<bool()>(DefaultPredicate)) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			_triggerConfigurations.insert(std::make_pair(trigger, TriggerConfiguration(destination, predicate)));
			return *this;
		}

		/**
		 * Assigns this state to be a sub-state of the specified super-state.
		 * If a trigger is fired and no transitions are found for this state, the super-states are queried for transitions.
		 * Transitions in sub-states have higher priority than transitions from their super-states.
		 *
		 * @param state The super-state of this state.
		 * @return A reference to this state configuration.
		 */
		StateConfiguration& SubstateOf(StateType state) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			_superStates.insert(state);
			return *this;
		}

		/**
		 * Sets the entry function handler.
		 * This function is called after a transition has been made to this state.
		 *
		 * @param handler The entry function handler.
		 * @return A reference to this state configuration.
		 */
		StateConfiguration& OnEntry(const Delegate<void()>& handler) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			_stateEntered = handler;
			return *this;
		}

		/**
		 * Sets the exit function handler.
		 * This function is called after a transition has been made from this state.
		 *
		 * @param handler The exit function handler.
		 * @return A reference to this state configuration.
		 */
		StateConfiguration& OnExit(const Delegate<void()>& handler) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			_stateExited = handler;
			return *this;
		}

		/**
		 * Signals that this state has been entered and invokes the entry function handler.
		 *
		 * @return A reference to this state configuration.
		 */
		StateConfiguration& OnStateEntered() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			_stateEntered();
			return *this;
		}

		/**
		 * Signals that this state has been exited and invokes the exit function handler.
		 *
		 * @return A reference to this state configuration.
		 */
		StateConfiguration& OnStateExited() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			_stateExited();
			return *this;
		}

		/**
		 * Determines the state that would be transitioned to should a particular trigger be fired.
		 * If the trigger is not defined for this state, the super-states are queried for the transition.
		 *
		 * @param trigger The trigger.
		 * @param destination Assigned to be the state that is transitioned to from the trigger. If this method returns false, then this parameter is not modified.
		 * @return True if the transition was allowed, false otherwise.
		 */
		bool Transition(TriggerType trigger, StateType& destination) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			std::pair<typename TriggerConfigurations::const_iterator, typename TriggerConfigurations::const_iterator> range = _triggerConfigurations.equal_range(trigger);
			typename TriggerConfigurations::const_iterator result = range.second;

			for (typename TriggerConfigurations::const_iterator i = range.first; i != range.second; i++) {
				if (i->second.CanTransition()) {
					if (result != range.second) {
						throw MultipleTriggerException();
					}

					result = i;
				}
			}

			// if we could not transition using this sub-state, try using the super-states
			if (result == range.second) {
				typename ParentStates::const_iterator result = _superStates.end();

				for (typename ParentStates::const_iterator i = _superStates.begin(); i != _superStates.end(); i++) {
					if (_lookup(*i).Transition(trigger, destination)) {
						if (result != _superStates.end()) {
							throw MultipleTriggerException();
						}

						result = i;
					}
				}

				return result != _superStates.end();
			}

			destination = result->second.Destination();
			return true;
		}

		/**
		 * Deletes the state configuration.
		 */
		virtual ~StateConfiguration() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
	};

public:
	typedef Delegate<StateType()> StateAccessor;
	typedef Delegate<void(StateType)> StateMutator;
	typedef Delegate<void(StateType, TriggerType)> UnhandledTriggerAction;
	typedef std::map<StateType, StateConfiguration> StateConfigurations;

private:
	StateAccessor _stateAccessor;
	StateMutator _stateMutator;
	UnhandledTriggerAction _unhandledTriggerAction;
	StateConfigurations _stateConfigurations;
	StateType _state;

	template <typename Signature> friend class Delegate;

	StateType DefaultStateAccessor() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return _state;
	}

	void DefaultStateMutator(StateType state) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		_state = state;
	}

public:

	/**
	 * Creates a new state machine whose state is controlled internally.
	 *
	 * @param state The initial state.
	 */
	StateMachine(StateType state) : _stateAccessor(), _stateMutator(), _unhandledTriggerAction(), _stateConfigurations(),  _state(state) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		_stateAccessor = StateAccessor(&StateMachine<StateType, TriggerType>::DefaultStateAccessor, this);
		_stateMutator = StateMutator(&StateMachine<StateType, TriggerType>::DefaultStateMutator, this);
	}

	/**
	 * Creates a new state machine whose state is controlled externally.
	 *
	 * @param stateAccessor The delegate used to get the current state.
	 * @param stateMutator The delegate used to set the current state.
	 */
	StateMachine(const StateAccessor& stateAccessor, const StateMutator& stateMutator) : _stateAccessor(stateAccessor), _stateMutator(stateMutator), _unhandledTriggerAction(), _stateConfigurations(), _state(_stateAccessor()) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

	/**
	 * Determines the current state of the state machine.
	 *
	 * @return The current state.
	 */
	StateType State() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return _stateAccessor();
	}

	/**
	 * Starts configuring transitions for the specified state.
	 *
	 * @param state The state to configure.
	 * @return A reference to the state configuration.
	 */
	StateConfiguration& Configure(StateType state) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		typename StateConfigurations::iterator i = _stateConfigurations.find(state);

		if (i == _stateConfigurations.end()) {
			_stateConfigurations[state] = StateConfiguration(typename StateConfiguration::LookupFunction(&StateMachine::Configure, this));
			return _stateConfigurations[state];
		}

		return i->second;
	}

	/**
	 * Determines whether or not a trigger can be fired and successfully signal a transition in state.
	 *
	 * @param trigger The trigger.
	 * @param source Assigned to be the source state. This is the state that would be transitioned from.
	 * @param destination Assigned to be the destination state. This is the state that would be transitioned to.
	 * @return True if the trigger can be fired, false otherwise. If false is returned, destination is not modified.
	 */
	bool CanFire(TriggerType trigger, StateType& source, StateType& destination) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		source = State();
		typename StateConfigurations::const_iterator i = _stateConfigurations.find(source);

		if (i == _stateConfigurations.end()) {
			return false;
		}

		return i->second.Transition(trigger, destination);
	}

	/**
	 * Determines whether or not a trigger can be fired and successfully signal a transition in state.
	 *
	 * @param trigger The trigger.
	 * @return True if the trigger can be fired, false otherwise.
	 */
	bool CanFire(TriggerType trigger) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		StateType source;
		StateType destination;
		return CanFire(trigger, source, destination);
	}

	/**
	 * Fires a trigger in order to signal a transition of state.
	 * If the current state of the state machine has not been configured or if the current state does not permit the trigger, an undefined trigger exception is thrown.
	 * If a state transition is found, the current state is changed to the destination state of the transition.
	 *
	 * @param trigger The trigger to fire.
	 * @return A reference to this state machine.
	 */
	StateMachine<StateType, TriggerType>& Fire(TriggerType trigger) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		StateType source;
		StateType destination;

		if (CanFire(trigger, source, destination) == false) {
			throw UndefinedTriggerException();
		}

		Configure(source).OnStateExited();
		_stateMutator(destination);
		Configure(destination).OnStateEntered();

		return *this;
	}

	/**
	 * Deletes the state machine.
	 */
	virtual ~StateMachine() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}
};

}

#endif /* STATEMACHINE_HPP_ */
