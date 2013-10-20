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

#ifndef DELEGATE_HPP_
#define DELEGATE_HPP_

#include "StackTrace.hpp"

namespace nitrus {

/**
 * A class that encapsulates an attempt to invoke an empty delegate that requires a return value.
 */
class EmptyDelegateException : public std::runtime_error {
public:

	/**
	 * Creates a new empty delegate exception.
	 */
	EmptyDelegateException() : std::runtime_error(__METHOD__) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

	/**
	 * Deletes the empty delegate exception.
	 */
	virtual ~EmptyDelegateException() throw() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}
};

/**
 * A template class representing a function type.
 * This class has no implicit implementation, rather is specialized for certain function definitions.
 */
template <typename Signature> class Delegate;

/**
 * A specialized class that consolidates static functions and instance functions that take no parameters and return a value.
 */
template <typename Return> class Delegate<Return ()> {
private:

	/**
	 * An interface from which both static functions and instance functions derive.
	 */
	class Invokable {
	public:

		/**
		 * Invokes the function pointer contained by this class.
		 *
		 * @return The return value of the function pointer.
		 */
		virtual Return Invoke() const = 0;

		/**
		 * Makes a copy of this interface that references the same function pointer.
		 * The caller is responsible for deleting the clone.
		 *
		 * @return The clone.
		 */
		virtual Invokable* Clone() const = 0;

		/**
		 * Compares this invokable object to another invokable object.
		 *
		 * @param that The object to compare to.
		 * @return True if the objects are considered equal, false otherwise.
		 */
		virtual bool Equals(const Invokable& that) const = 0;

		/**
		 * Deletes the invokable object.
		 */
		virtual ~Invokable() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
	};

	/**
	 * A class that represents an object that invokes a static function.
	 */
	class StaticInvokable : public Invokable {
	private:
		Return (*_function)();

	public:

		/**
		 * Creates a new static function object.
		 *
		 * @param function The function pointer to invoke.
		 */
		StaticInvokable(Return (*function)()) : _function(function) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Deletes the static function object.
		 */
		virtual ~StaticInvokable() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Invokes the function pointer contained by this class.
		 *
		 * @return The return value of the function pointer.
		 */
		virtual Return Invoke() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return _function();
		}

		/**
		 * Makes a copy of this interface that references the same function pointer.
		 * The caller is responsible for deleting the clone.
		 *
		 * @return The clone.
		 */
		virtual Invokable* Clone() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return new StaticInvokable(_function);
		}

		/**
		 * Compares this invokable object to another invokable object.
		 *
		 * @param that The object to compare to.
		 * @return True if the objects reference the same function pointer, false otherwise.
		 */
		virtual bool Equals(const Invokable& that) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			const StaticInvokable* other = dynamic_cast<const StaticInvokable*>(&that);
			return other && other->_function == _function;
		}
	};

	/**
	 * A class that represents an object that invokes a member function.
	 */
	template <typename Instance> class InstanceInvokable : public Invokable {
	private:
		Return (Instance::*_function)();
		Instance* _instance;

	public:

		/**
		 * Creates a new member function object.
		 *
		 * @param function The function pointer to invoke.
		 * @param instance The instance to invoke the function for.
		 */
		InstanceInvokable(Return (Instance::*function)(), Instance* instance) : _function(function), _instance(instance) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
		
		/**
		 * Creates a new member function object from another.
		 *
		 * @param that The member function object to clone.
		 */
		InstanceInvokable(const InstanceInvokable& that) : _function(that._function), _instance(that._instance) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			
		}
		
		/**
		 * Copies a member function object into this object.
		 *
		 * @param that The member function object to clone.
		 */
		InstanceInvokable& operator = (const InstanceInvokable& that) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			_function = that._function;
			_instance = that._instance;
			
			return *this;
		}

		/**
		 * Deletes the member function object.
		 */
		virtual ~InstanceInvokable() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Invokes the function pointer contained by this class.
		 *
		 * @return The return value of the function pointer.
		 */
		virtual Return Invoke() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return (_instance->*_function)();
		}

		/**
		 * Makes a copy of this interface that references the same function pointer.
		 * The caller is responsible for deleting the clone.
		 *
		 * @return The clone.
		 */
		virtual Invokable* Clone() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return new InstanceInvokable<Instance>(_function, _instance);
		}

		/**
		 * Compares this invokable object to another invokable object.
		 *
		 * @param that The object to compare to.
		 * @return True if the objects reference the same function pointer and the same instance, false otherwise.
		 */
		virtual bool Equals(const Invokable& that) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			const InstanceInvokable<Instance>* other = dynamic_cast<const InstanceInvokable<Instance>*>(&that);
			return other && other->_function == _function && other->_instance == _instance;
		}
	};

	/**
	 * A class that represents an object that invokes a constant member function.
	 */
	template <typename Instance> class ConstantInstanceInvokable : public Invokable {
	private:
		Return (Instance::*_function)() const;
		const Instance* _instance;

	public:

		/**
		 * Creates a new constant member function object.
		 *
		 * @param function The function pointer to invoke.
		 * @param instance The instance to invoke the function for.
		 */
		ConstantInstanceInvokable(Return (Instance::*function)() const, const Instance* instance) : _function(function), _instance(instance) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
		
		/**
		 * Creates a new constant member function object from another.
		 *
		 * @param that The constant member function object to clone.
		 */
		ConstantInstanceInvokable(const ConstantInstanceInvokable& that) : _function(that._function), _instance(that._instance) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			
		}
		
		/**
		 * Copies a constant member function object into this object.
		 *
		 * @param that The constant member function object to clone.
		 */
		ConstantInstanceInvokable& operator = (const ConstantInstanceInvokable& that) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			_function = that._function;
			_instance = that._instance;
			
			return *this;
		}

		/**
		 * Deletes the constant member function object.
		 */
		virtual ~ConstantInstanceInvokable() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Invokes the function pointer contained by this class.
		 *
		 * @return The return value of the function pointer.
		 */
		virtual Return Invoke() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return (_instance->*_function)();
		}

		/**
		 * Makes a copy of this interface that references the same function pointer.
		 * The caller is responsible for deleting the clone.
		 *
		 * @return The clone.
		 */
		virtual Invokable* Clone() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return new ConstantInstanceInvokable<Instance>(_function, _instance);
		}

		/**
		 * Compares this invokable object to another invokable object.
		 *
		 * @param that The object to compare to.
		 * @return True if the objects reference the same function pointer and the same instance, false otherwise.
		 */
		virtual bool Equals(const Invokable& that) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			const ConstantInstanceInvokable<Instance>* other = dynamic_cast<const ConstantInstanceInvokable<Instance>*>(&that);
			return other && other->_function == _function && other->_instance == _instance;
		}
	};

public:

	/**
	 * Creates a new delegate that returns the default value when invoked.
	 */
	Delegate() : _invokable(0) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

	/**
	 * Creates a new delegate that is a clone of another delegate.
	 *
	 * @param that The delegate to clone.
	 */
	Delegate(const Delegate<Return ()>& that) : _invokable(that._invokable ? that._invokable->Clone() : 0) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

	/**
	 * Creates a new delegate that invokes a static function.
	 *
	 * @param function The function to invoke.
	 */
	Delegate(Return (*function)()) : _invokable(new StaticInvokable(function)) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

	/**
	 * Creates a new delegate that invokes a member function.
	 *
	 * @param function The function to invoke.
	 * @param instance The instance to invoke on.
	 */
	template <typename Instance> Delegate(Return (Instance::*function)(), Instance* instance) : _invokable(new InstanceInvokable<Instance>(function, instance)) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

	/**
	 * Creates a new delegate that invokes a constant member function.
	 *
	 * @param function The function to invoke.
	 * @param instance The instance to invoke on.
	 */
	template <typename Instance> Delegate(Return (Instance::*function)() const, const Instance* instance) : _invokable(new ConstantInstanceInvokable<Instance>(function, instance)) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

	/**
	 * Clones the specified delegate and assigns it to this delegate.
	 *
	 * @param that The delegate to clone.
	 * @return This delegate.
	 */
	Delegate<Return ()>& operator = (const Delegate<Return ()>& that) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		if (_invokable) {
			delete _invokable;
			_invokable = 0;
		}

		_invokable = that._invokable ? that._invokable->Clone() : 0;
		return *this;
	}

	/**
	 * Invokes the delegate.
	 *
	 * @return The value returned by the function pointer.
	 */
	Return operator ()() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		if (_invokable) {
			return _invokable->Invoke();
		}

		throw EmptyDelegateException();
	}

	/**
	 * Compares this delegate to another delegate.
	 *
	 * @param that The delegate to compare to.
	 * @return True if the delegates are considered equal, false otherwise.
	 */
	bool operator == (const Delegate<Return ()>& that) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		if (_invokable) {
			return that._invokable && _invokable->Equals(*that._invokable);
		}
		else {
			return that._invokable == 0;
		}
	}

	/**
	 * Deletes the delegate.
	 */
	virtual ~Delegate() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		if (_invokable) {
			delete _invokable;
			_invokable = 0;
		}
	}

	/**
	 * Returns an empty delegate.
	 *
	 * @return The empty delegate.
	 */
	static Delegate<Return ()> Empty() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		static Delegate<Return ()> empty;
		return empty;
	}

private:
	Invokable* _invokable;
};

template <typename Return> Delegate<Return ()> delegate(Return (*function)()) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
	return Delegate<Return ()>(function);
}

template <typename Return, typename Instance> Delegate<Return ()> delegate(Return (Instance::*function)(), Instance* instance) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
	return Delegate<Return ()>(function, instance);
}

template <typename Return, typename Instance> Delegate<Return ()> delegate(Return (Instance::*function)() const, const Instance* instance) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
	return Delegate<Return ()>(function, instance);
}

/**
 * A specialized class that consolidates static functions and instance functions that take no parameters and return no value.
 */
template <> class Delegate<void ()> {
private:

	/**
	 * An interface from which both static functions and instance functions derive.
	 */
	class Invokable {
	public:

		/**
		 * Invokes the function pointer contained by this class.
		 */
		virtual void Invoke() const = 0;

		/**
		 * Makes a copy of this interface that references the same function pointer.
		 * The caller is responsible for deleting the clone.
		 *
		 * @return The clone.
		 */
		virtual Invokable* Clone() const = 0;

		/**
		 * Compares this invokable object to another invokable object.
		 *
		 * @param that The object to compare to.
		 * @return True if the objects are considered equal, false otherwise.
		 */
		virtual bool Equals(const Invokable& that) const = 0;

		/**
		 * Deletes the invokable object.
		 */
		virtual ~Invokable() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
	};

	/**
	 * A class that represents an object that invokes a static function.
	 */
	class StaticInvokable : public Invokable {
	private:
		void (*_function)();

	public:

		/**
		 * Creates a new static function object.
		 *
		 * @param function The function pointer to invoke.
		 */
		StaticInvokable(void (*function)()) : _function(function) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Deletes the static function object.
		 */
		virtual ~StaticInvokable() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Invokes the function pointer contained by this class.
		 */
		virtual void Invoke() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			_function();
		}


		/**
		 * Makes a copy of this interface that references the same function pointer.
		 * The caller is responsible for deleting the clone.
		 *
		 * @return The clone.
		 */
		virtual Invokable* Clone() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return new StaticInvokable(_function);
		}

		/**
		 * Compares this invokable object to another invokable object.
		 *
		 * @param that The object to compare to.
		 * @return True if the objects reference the same function pointer, false otherwise.
		 */
		virtual bool Equals(const Invokable& that) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			const StaticInvokable* other = dynamic_cast<const StaticInvokable*>(&that);
			return other && other->_function == _function;
		}
	};

	/**
	 * A class that represents an object that invokes a member function.
	 */
	template <typename Instance> class InstanceInvokable : public Invokable {
	private:
		void (Instance::*_function)();
		Instance* _instance;

	public:

		/**
		 * Creates a new member function object.
		 *
		 * @param function The function pointer to invoke.
		 * @param instance The instance to invoke the function for.
		 */
		InstanceInvokable(void (Instance::*function)(), Instance* instance) : _function(function), _instance(instance) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
		
		/**
		 * Creates a new member function object from another.
		 *
		 * @param that The member function object to clone.
		 */
		InstanceInvokable(const InstanceInvokable& that) : _function(that._function), _instance(that._instance) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			
		}
		
		/**
		 * Copies a member function object into this object.
		 *
		 * @param that The member function object to clone.
		 */
		InstanceInvokable& operator = (const InstanceInvokable& that) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			_function = that._function;
			_instance = that._instance;
			
			return *this;
		}

		/**
		 * Deletes the member function object.
		 */
		virtual ~InstanceInvokable() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Invokes the function pointer contained by this class.
		 */
		virtual void Invoke() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			(_instance->*_function)();
		}

		/**
		 * Makes a copy of this interface that references the same function pointer.
		 * The caller is responsible for deleting the clone.
		 *
		 * @return The clone.
		 */
		virtual Invokable* Clone() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return new InstanceInvokable<Instance>(_function, _instance);
		}

		/**
		 * Compares this invokable object to another invokable object.
		 *
		 * @param that The object to compare to.
		 * @return True if the objects reference the same function pointer and the same instance, false otherwise.
		 */
		virtual bool Equals(const Invokable& that) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			const InstanceInvokable<Instance>* other = dynamic_cast<const InstanceInvokable<Instance>*>(&that);
			return other && other->_function == _function && other->_instance == _instance;
		}
	};

	/**
	 * A class that represents an object that invokes a constant member function.
	 */
	template <typename Instance> class ConstantInstanceInvokable : public Invokable {
	private:
		void (Instance::*_function)() const;
		const Instance* _instance;

	public:

		/**
		 * Creates a new constant member function object.
		 *
		 * @param function The function pointer to invoke.
		 * @param instance The instance to invoke the function for.
		 */
		ConstantInstanceInvokable(void (Instance::*function)() const, const Instance* instance) : _function(function), _instance(instance) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
		
		/**
		 * Creates a new constant member function object from another.
		 *
		 * @param that The constant member function object to clone.
		 */
		ConstantInstanceInvokable(const ConstantInstanceInvokable& that) : _function(that._function), _instance(that._instance) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			
		}
		
		/**
		 * Copies a constant member function object into this object.
		 *
		 * @param that The constant member function object to clone.
		 */
		ConstantInstanceInvokable& operator = (const ConstantInstanceInvokable& that) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			_function = that._function;
			_instance = that._instance;
			
			return *this;
		}

		/**
		 * Deletes the constant member function object.
		 */
		virtual ~ConstantInstanceInvokable() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Invokes the function pointer contained by this class.
		 */
		virtual void Invoke() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			(_instance->*_function)();
		}

		/**
		 * Makes a copy of this interface that references the same function pointer.
		 * The caller is responsible for deleting the clone.
		 *
		 * @return The clone.
		 */
		virtual Invokable* Clone() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return new ConstantInstanceInvokable<Instance>(_function, _instance);
		}

		/**
		 * Compares this invokable object to another invokable object.
		 *
		 * @param that The object to compare to.
		 * @return True if the objects reference the same function pointer and the same instance, false otherwise.
		 */
		virtual bool Equals(const Invokable& that) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			const ConstantInstanceInvokable<Instance>* other = dynamic_cast<const ConstantInstanceInvokable<Instance>*>(&that);
			return other && other->_function == _function && other->_instance == _instance;
		}
	};

public:

	/**
	 * Creates a new delegate that does nothing when invoked.
	 */
	Delegate() : _invokable(0) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

	/**
	 * Creates a new delegate that is a clone of another delegate.
	 *
	 * @param that The delegate to clone.
	 */
	Delegate(const Delegate<void ()>& that) : _invokable(that._invokable ? that._invokable->Clone() : 0) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

	/**
	 * Creates a new delegate that invokes a static function.
	 *
	 * @param function The function to invoke.
	 */
	Delegate(void (*function)()) : _invokable(new StaticInvokable(function)) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

	/**
	 * Creates a new delegate that invokes a member function.
	 *
	 * @param function The function to invoke.
	 * @param instance The instance to invoke on.
	 */
	template <typename Instance> Delegate(void (Instance::*function)(), Instance* instance) : _invokable(new InstanceInvokable<Instance>(function, instance)) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

	/**
	 * Creates a new delegate that invokes a constant member function.
	 *
	 * @param function The function to invoke.
	 * @param instance The instance to invoke on.
	 */
	template <typename Instance> Delegate(void (Instance::*function)() const, const Instance* instance) : _invokable(new ConstantInstanceInvokable<Instance>(function, instance)) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

	/**
	 * Clones the specified delegate and assigns it to this delegate.
	 *
	 * @param that The delegate to clone.
	 * @return This delegate.
	 */
	Delegate<void ()>& operator = (const Delegate<void ()>& that) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		if (_invokable) {
			delete _invokable;
			_invokable = 0;
		}

		_invokable = that._invokable ? that._invokable->Clone() : 0;
		return *this;
	}

	/**
	 * Invokes the delegate.
	 */
	void operator ()() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		if (_invokable) {
			_invokable->Invoke();
		}
	}

	/**
	 * Compares this delegate to another delegate.
	 *
	 * @param that The delegate to compare to.
	 * @return True if the delegates are considered equal, false otherwise.
	 */
	bool operator == (const Delegate<void ()>& that) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		if (_invokable) {
			return that._invokable && _invokable->Equals(*that._invokable);
		}
		else {
			return that._invokable == 0;
		}
	}

	/**
	 * Deletes the delegate.
	 */
	virtual ~Delegate() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		if (_invokable) {
			delete _invokable;
			_invokable = 0;
		}
	}

	/**
	 * Returns an empty delegate.
	 *
	 * @return The empty delegate.
	 */
	static Delegate<void ()> Empty() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		static Delegate<void ()> empty;
		return empty;
	}

private:
	Invokable* _invokable;
};

Delegate<void ()> delegate(void (*function)()) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
	return Delegate<void ()>(function);
}

template <typename Instance> Delegate<void ()> delegate(void (Instance::*function)(), Instance* instance) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
	return Delegate<void ()>(function, instance);
}

template <typename Instance> Delegate<void ()> delegate(void (Instance::*function)() const, const Instance* instance) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
	return Delegate<void ()>(function, instance);
}

/**
 * A specialized class that consolidates static functions and instance functions that take one parameter and return a value.
 */
template <typename Return, typename A> class Delegate<Return (A)> {
private:

	/**
	 * An interface from which both static functions and instance functions derive.
	 */
	class Invokable {
	public:

		/**
		 * Invokes the function pointer contained by this class.
		 *
		 * @return The return value of the function pointer.
		 */
		virtual Return Invoke(A a) const = 0;

		/**
		 * Makes a copy of this interface that references the same function pointer.
		 * The caller is responsible for deleting the clone.
		 *
		 * @return The clone.
		 */
		virtual Invokable* Clone() const = 0;

		/**
		 * Compares this invokable object to another invokable object.
		 *
		 * @param that The object to compare to.
		 * @return True if the objects are considered equal, false otherwise.
		 */
		virtual bool Equals(const Invokable& that) const = 0;

		/**
		 * Deletes the invokable object.
		 */
		virtual ~Invokable() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
	};

	/**
	 * A class that represents an object that invokes a static function.
	 */
	class StaticInvokable : public Invokable {
	private:
		Return (*_function)(A);

	public:

		/**
		 * Creates a new static function object.
		 *
		 * @param function The function pointer to invoke.
		 */
		StaticInvokable(Return (*function)(A)) : _function(function) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Deletes the static function object.
		 */
		virtual ~StaticInvokable() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Invokes the function pointer contained by this class.
		 *
		 * @param a The parameter to the function pointer.
		 * @return The return value of the function pointer.
		 */
		virtual Return Invoke(A a) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return _function(a);
		}

		/**
		 * Makes a copy of this interface that references the same function pointer.
		 * The caller is responsible for deleting the clone.
		 *
		 * @return The clone.
		 */
		virtual Invokable* Clone() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return new StaticInvokable(_function);
		}

		/**
		 * Compares this invokable object to another invokable object.
		 *
		 * @param that The object to compare to.
		 * @return True if the objects reference the same function pointer, false otherwise.
		 */
		virtual bool Equals(const Invokable& that) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			const StaticInvokable* other = dynamic_cast<const StaticInvokable*>(&that);
			return other && other->_function == _function;
		}
	};

	/**
	 * A class that represents an object that invokes a member function.
	 */
	template <typename Instance> class InstanceInvokable : public Invokable {
	private:
		Return (Instance::*_function)(A);
		Instance* _instance;

	public:

		/**
		 * Creates a new member function object.
		 *
		 * @param function The function pointer to invoke.
		 * @param instance The instance to invoke the function for.
		 */
		InstanceInvokable(Return (Instance::*function)(A), Instance* instance) : _function(function), _instance(instance) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
		
		/**
		 * Creates a new member function object from another.
		 *
		 * @param that The member function object to clone.
		 */
		InstanceInvokable(const InstanceInvokable& that) : _function(that._function), _instance(that._instance) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			
		}
		
		/**
		 * Copies a member function object into this object.
		 *
		 * @param that The member function object to clone.
		 */
		InstanceInvokable& operator = (const InstanceInvokable& that) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			_function = that._function;
			_instance = that._instance;
			
			return *this;
		}

		/**
		 * Deletes the member function object.
		 */
		virtual ~InstanceInvokable() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Invokes the function pointer contained by this class.
		 *
		 * @return The return value of the function pointer.
		 */
		virtual Return Invoke(A a) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return (_instance->*_function)(a);
		}

		/**
		 * Makes a copy of this interface that references the same function pointer.
		 * The caller is responsible for deleting the clone.
		 *
		 * @return The clone.
		 */
		virtual Invokable* Clone() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return new InstanceInvokable<Instance>(_function, _instance);
		}

		/**
		 * Compares this invokable object to another invokable object.
		 *
		 * @param that The object to compare to.
		 * @return True if the objects reference the same function pointer and the same instance, false otherwise.
		 */
		virtual bool Equals(const Invokable& that) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			const InstanceInvokable<Instance>* other = dynamic_cast<const InstanceInvokable<Instance>*>(&that);
			return other && other->_function == _function && other->_instance == _instance;
		}
	};

	/**
	 * A class that represents an object that invokes a constant member function.
	 */
	template <typename Instance> class ConstantInstanceInvokable : public Invokable {
	private:
		Return (Instance::*_function)(A) const;
		const Instance* _instance;

	public:

		/**
		 * Creates a new constant member function object.
		 *
		 * @param function The function pointer to invoke.
		 * @param instance The instance to invoke the function for.
		 */
		ConstantInstanceInvokable(Return (Instance::*function)(A) const, const Instance* instance) : _function(function), _instance(instance) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
		
		/**
		 * Creates a constant new member function object from another.
		 *
		 * @param that The constant member function object to clone.
		 */
		ConstantInstanceInvokable(const ConstantInstanceInvokable& that) : _function(that._function), _instance(that._instance) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			
		}
		
		/**
		 * Copies a constant member function object into this object.
		 *
		 * @param that The constant member function object to clone.
		 */
		ConstantInstanceInvokable& operator = (const ConstantInstanceInvokable& that) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			_function = that._function;
			_instance = that._instance;
			
			return *this;
		}

		/**
		 * Deletes the constant member function object.
		 */
		virtual ~ConstantInstanceInvokable() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Invokes the function pointer contained by this class.
		 *
		 * @return The return value of the function pointer.
		 */
		virtual Return Invoke(A a) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return (_instance->*_function)(a);
		}

		/**
		 * Makes a copy of this interface that references the same function pointer.
		 * The caller is responsible for deleting the clone.
		 *
		 * @return The clone.
		 */
		virtual Invokable* Clone() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return new ConstantInstanceInvokable<Instance>(_function, _instance);
		}

		/**
		 * Compares this invokable object to another invokable object.
		 *
		 * @param that The object to compare to.
		 * @return True if the objects reference the same function pointer and the same instance, false otherwise.
		 */
		virtual bool Equals(const Invokable& that) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			const ConstantInstanceInvokable<Instance>* other = dynamic_cast<const ConstantInstanceInvokable<Instance>*>(&that);
			return other && other->_function == _function && other->_instance == _instance;
		}
	};

public:

	/**
	 * Creates a new delegate that returns the default value when invoked.
	 */
	Delegate() : _invokable(0) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

	/**
	 * Creates a new delegate that is a clone of another delegate.
	 *
	 * @param that The delegate to clone.
	 */
	Delegate(const Delegate<Return (A)>& that) : _invokable(that._invokable ? that._invokable->Clone() : 0) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

	/**
	 * Creates a new delegate that invokes a static function.
	 *
	 * @param function The function to invoke.
	 */
	Delegate(Return (*function)(A)) : _invokable(new StaticInvokable(function)) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

	/**
	 * Creates a new delegate that invokes a member function.
	 *
	 * @param function The function to invoke.
	 * @param instance The instance to invoke on.
	 */
	template <typename Instance> Delegate(Return (Instance::*function)(A), Instance* instance) : _invokable(new InstanceInvokable<Instance>(function, instance)) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

	/**
	 * Creates a new delegate that invokes a constant member function.
	 *
	 * @param function The function to invoke.
	 * @param instance The instance to invoke on.
	 */
	template <typename Instance> Delegate(Return (Instance::*function)(A) const, const Instance* instance) : _invokable(new ConstantInstanceInvokable<Instance>(function, instance)) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

	/**
	 * Clones the specified delegate and assigns it to this delegate.
	 *
	 * @param that The delegate to clone.
	 * @return This delegate.
	 */
	Delegate<Return (A)>& operator = (const Delegate<Return (A)>& that) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		if (_invokable) {
			delete _invokable;
			_invokable = 0;
		}

		_invokable = that._invokable ? that._invokable->Clone() : 0;
		return *this;
	}

	/**
	 * Invokes the delegate.
	 *
	 * @return The value returned by the function pointer.
	 */
	Return operator ()(A a) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		if (_invokable) {
			return _invokable->Invoke(a);
		}

		throw EmptyDelegateException();
	}

	/**
	 * Compares this delegate to another delegate.
	 *
	 * @param that The delegate to compare to.
	 * @return True if the delegates are considered equal, false otherwise.
	 */
	bool operator == (const Delegate<Return (A)>& that) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		if (_invokable) {
			return that._invokable && _invokable->Equals(*that._invokable);
		}
		else {
			return that._invokable == 0;
		}
	}

	/**
	 * Deletes the delegate.
	 */
	virtual ~Delegate() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		if (_invokable) {
			delete _invokable;
			_invokable = 0;
		}
	}

	/**
	 * Returns an empty delegate.
	 *
	 * @return The empty delegate.
	 */
	static Delegate<Return (A)> Empty() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		static Delegate<Return (A)> empty;
		return empty;
	}

private:
	Invokable* _invokable;
};

template <typename Return, typename A> Delegate<Return (A)> delegate(Return (*function)(A)) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
	return Delegate<Return (A)>(function);
}

template <typename Return, typename A, typename Instance> Delegate<Return (A)> delegate(Return (Instance::*function)(A), Instance* instance) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
	return Delegate<Return (A)>(function, instance);
}

template <typename Return, typename A, typename Instance> Delegate<Return (A)> delegate(Return (Instance::*function)(A) const, const Instance* instance) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
	return Delegate<Return (A)>(function, instance);
}

/**
 * A specialized class that consolidates static functions and instance functions that take one parameter and return no value.
 */
template <typename A> class Delegate<void (A)> {
private:

	/**
	 * An interface from which both static functions and instance functions derive.
	 */
	class Invokable {
	public:

		/**
		 * Invokes the function pointer contained by this class.
		 *
		 * @param a The parameter to the function pointer.
		 */
		virtual void Invoke(A a) const = 0;

		/**
		 * Makes a copy of this interface that references the same function pointer.
		 * The caller is responsible for deleting the clone.
		 *
		 * @return The clone.
		 */
		virtual Invokable* Clone() const = 0;

		/**
		 * Compares this invokable object to another invokable object.
		 *
		 * @param that The object to compare to.
		 * @return True if the objects are considered equal, false otherwise.
		 */
		virtual bool Equals(const Invokable& that) const = 0;

		/**
		 * Deletes the invokable object.
		 */
		virtual ~Invokable() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
	};

	/**
	 * A class that represents an object that invokes a static function.
	 */
	class StaticInvokable : public Invokable {
	private:
		void (*_function)(A);

	public:

		/**
		 * Creates a new static function object.
		 *
		 * @param function The function pointer to invoke.
		 */
		StaticInvokable(void (*function)(A)) : _function(function) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Deletes the static function object.
		 */
		virtual ~StaticInvokable() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Invokes the function pointer contained by this class.
		 *
		 * @param a The parameter to the function pointer.
		 */
		virtual void Invoke(A a) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			_function(a);
		}

		/**
		 * Makes a copy of this interface that references the same function pointer.
		 * The caller is responsible for deleting the clone.
		 *
		 * @return The clone.
		 */
		virtual Invokable* Clone() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return new StaticInvokable(_function);
		}

		/**
		 * Compares this invokable object to another invokable object.
		 *
		 * @param that The object to compare to.
		 * @return True if the objects reference the same function pointer, false otherwise.
		 */
		virtual bool Equals(const Invokable& that) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			const StaticInvokable* other = dynamic_cast<const StaticInvokable*>(&that);
			return other && other->_function == _function;
		}
	};

	/**
	 * A class that represents an object that invokes a member function.
	 */
	template <typename Instance> class InstanceInvokable : public Invokable {
	private:
		void (Instance::*_function)(A);
		Instance* _instance;

	public:

		/**
		 * Creates a new member function object.
		 *
		 * @param function The function pointer to invoke.
		 * @param instance The instance to invoke the function for.
		 */
		InstanceInvokable(void (Instance::*function)(A), Instance* instance) : _function(function), _instance(instance) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
		
		/**
		 * Creates a new member function object from another.
		 *
		 * @param that The member function object to clone.
		 */
		InstanceInvokable(const InstanceInvokable& that) : _function(that._function), _instance(that._instance) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			
		}
		
		/**
		 * Copies a member function object into this object.
		 *
		 * @param that The member function object to clone.
		 */
		InstanceInvokable& operator = (const InstanceInvokable& that) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			_function = that._function;
			_instance = that._instance;
			
			return *this;
		}

		/**
		 * Deletes the member function object.
		 */
		virtual ~InstanceInvokable() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Invokes the function pointer contained by this class.
		 *
		 * @param a The parameter to the function pointer.
		 */
		virtual void Invoke(A a) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			(_instance->*_function)(a);
		}

		/**
		 * Makes a copy of this interface that references the same function pointer.
		 * The caller is responsible for deleting the clone.
		 *
		 * @return The clone.
		 */
		virtual Invokable* Clone() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return new InstanceInvokable<Instance>(_function, _instance);
		}

		/**
		 * Compares this invokable object to another invokable object.
		 *
		 * @param that The object to compare to.
		 * @return True if the objects reference the same function pointer and the same instance, false otherwise.
		 */
		virtual bool Equals(const Invokable& that) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			const InstanceInvokable<Instance>* other = dynamic_cast<const InstanceInvokable<Instance>*>(&that);
			return other && other->_function == _function && other->_instance == _instance;
		}
	};

	/**
	 * A class that represents an object that invokes a constant member function.
	 */
	template <typename Instance> class ConstantInstanceInvokable : public Invokable {
	private:
		void (Instance::*_function)(A) const;
		const Instance* _instance;

	public:

		/**
		 * Creates a new constant member function object.
		 *
		 * @param function The function pointer to invoke.
		 * @param instance The instance to invoke the function for.
		 */
		ConstantInstanceInvokable(void (Instance::*function)(A) const, const Instance* instance) : _function(function), _instance(instance) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
		
		/**
		 * Creates a constant new member function object from another.
		 *
		 * @param that The constant member function object to clone.
		 */
		ConstantInstanceInvokable(const ConstantInstanceInvokable& that) : _function(that._function), _instance(that._instance) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			
		}
		
		/**
		 * Copies a constant member function object into this object.
		 *
		 * @param that The constant member function object to clone.
		 */
		ConstantInstanceInvokable& operator = (const ConstantInstanceInvokable& that) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			_function = that._function;
			_instance = that._instance;
			
			return *this;
		}

		/**
		 * Deletes the constant member function object.
		 */
		virtual ~ConstantInstanceInvokable() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Invokes the function pointer contained by this class.
		 *
		 * @param a The parameter to the function pointer.
		 */
		virtual void Invoke(A a) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			(_instance->*_function)(a);
		}

		/**
		 * Makes a copy of this interface that references the same function pointer.
		 * The caller is responsible for deleting the clone.
		 *
		 * @return The clone.
		 */
		virtual Invokable* Clone() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return new ConstantInstanceInvokable<Instance>(_function, _instance);
		}

		/**
		 * Compares this invokable object to another invokable object.
		 *
		 * @param that The object to compare to.
		 * @return True if the objects reference the same function pointer and the same instance, false otherwise.
		 */
		virtual bool Equals(const Invokable& that) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			const ConstantInstanceInvokable<Instance>* other = dynamic_cast<const ConstantInstanceInvokable<Instance>*>(&that);
			return other && other->_function == _function && other->_instance == _instance;
		}
	};

public:

	/**
	 * Creates a new delegate that returns the default value when invoked.
	 */
	Delegate() : _invokable(0) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

	/**
	 * Creates a new delegate that is a clone of another delegate.
	 *
	 * @param that The delegate to clone.
	 */
	Delegate(const Delegate<void (A)>& that) : _invokable(that._invokable ? that._invokable->Clone() : 0) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

	/**
	 * Creates a new delegate that invokes a static function.
	 *
	 * @param function The function to invoke.
	 */
	Delegate(void (*function)(A)) : _invokable(new StaticInvokable(function)) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

	/**
	 * Creates a new delegate that invokes a member function.
	 *
	 * @param function The function to invoke.
	 * @param instance The instance to invoke on.
	 */
	template <typename Instance> Delegate(void (Instance::*function)(A), Instance* instance) : _invokable(new InstanceInvokable<Instance>(function, instance)) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

	/**
	 * Creates a new delegate that invokes a constant member function.
	 *
	 * @param function The function to invoke.
	 * @param instance The instance to invoke on.
	 */
	template <typename Instance> Delegate(void (Instance::*function)(A) const, const Instance* instance) : _invokable(new ConstantInstanceInvokable<Instance>(function, instance)) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

	/**
	 * Clones the specified delegate and assigns it to this delegate.
	 *
	 * @param that The delegate to clone.
	 * @return This delegate.
	 */
	Delegate<void (A)>& operator = (const Delegate<void (A)>& that) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		if (_invokable) {
			delete _invokable;
			_invokable = 0;
		}

		_invokable = that._invokable ? that._invokable->Clone() : 0;
		return *this;
	}

	/**
	 * Invokes the delegate.
	 *
	 * @param a The parameter to the function pointer.
	 */
	void operator ()(A a) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		if (_invokable) {
			_invokable->Invoke(a);
		}
	}

	/**
	 * Compares this delegate to another delegate.
	 *
	 * @param that The delegate to compare to.
	 * @return True if the delegates are considered equal, false otherwise.
	 */
	bool operator == (const Delegate<void (A)>& that) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		if (_invokable) {
			return that._invokable && _invokable->Equals(*that._invokable);
		}
		else {
			return that._invokable == 0;
		}
	}

	/**
	 * Deletes the delegate.
	 */
	virtual ~Delegate() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		if (_invokable) {
			delete _invokable;
			_invokable = 0;
		}
	}

	/**
	 * Returns an empty delegate.
	 *
	 * @return The empty delegate.
	 */
	static Delegate<void (A)> Empty() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		static Delegate<void (A)> empty;
		return empty;
	}

private:
	Invokable* _invokable;
};

template <typename A> Delegate<void (A)> delegate(void (*function)(A)) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
	return Delegate<void (A)>(function);
}

template <typename A, typename Instance> Delegate<void (A)> delegate(void (Instance::*function)(A), Instance* instance) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
	return Delegate<void (A)>(function, instance);
}

template <typename A, typename Instance> Delegate<void (A)> delegate(void (Instance::*function)(A) const, const Instance* instance) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
	return Delegate<void (A)>(function, instance);
}

/**
 * A specialized class that consolidates static functions and instance functions that take two parameters and return a value.
 */
template <typename Return, typename A, typename B> class Delegate<Return (A, B)> {
private:

	/**
	 * An interface from which both static functions and instance functions derive.
	 */
	class Invokable {
	public:

		/**
		 * Invokes the function pointer contained by this class.
		 *
		 * @param a The first parameter to the function pointer.
		 * @param b The second parameter to the function pointer.
		 * @return The return value of the function pointer.
		 */
		virtual Return Invoke(A a, B b) const = 0;

		/**
		 * Makes a copy of this interface that references the same function pointer.
		 * The caller is responsible for deleting the clone.
		 *
		 * @return The clone.
		 */
		virtual Invokable* Clone() const = 0;

		/**
		 * Compares this invokable object to another invokable object.
		 *
		 * @param that The object to compare to.
		 * @return True if the objects are considered equal, false otherwise.
		 */
		virtual bool Equals(const Invokable& that) const = 0;

		/**
		 * Deletes the invokable object.
		 */
		virtual ~Invokable() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
	};

	/**
	 * A class that represents an object that invokes a static function.
	 */
	class StaticInvokable : public Invokable {
	private:
		Return (*_function)(A, B);

	public:

		/**
		 * Creates a new static function object.
		 *
		 * @param function The function pointer to invoke.
		 */
		StaticInvokable(Return (*function)(A, B)) : _function(function) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Deletes the static function object.
		 */
		virtual ~StaticInvokable() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Invokes the function pointer contained by this class.
		 *
		 * @param a The first parameter to the function pointer.
		 * @param b The second parameter to the function pointer.
		 * @return The return value of the function pointer.
		 */
		virtual Return Invoke(A a, B b) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return _function(a, b);
		}

		/**
		 * Makes a copy of this interface that references the same function pointer.
		 * The caller is responsible for deleting the clone.
		 *
		 * @return The clone.
		 */
		virtual Invokable* Clone() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return new StaticInvokable(_function);
		}

		/**
		 * Compares this invokable object to another invokable object.
		 *
		 * @param that The object to compare to.
		 * @return True if the objects reference the same function pointer, false otherwise.
		 */
		virtual bool Equals(const Invokable& that) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			const StaticInvokable* other = dynamic_cast<const StaticInvokable*>(&that);
			return other && other->_function == _function;
		}
	};

	/**
	 * A class that represents an object that invokes a member function.
	 */
	template <typename Instance> class InstanceInvokable : public Invokable {
	private:
		Return (Instance::*_function)(A, B);
		Instance* _instance;

	public:

		/**
		 * Creates a new member function object.
		 *
		 * @param function The function pointer to invoke.
		 * @param instance The instance to invoke the function for.
		 */
		InstanceInvokable(Return (Instance::*function)(A, B), Instance* instance) : _function(function), _instance(instance) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
		
		/**
		 * Creates a new member function object from another.
		 *
		 * @param that The member function object to clone.
		 */
		InstanceInvokable(const InstanceInvokable& that) : _function(that._function), _instance(that._instance) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			
		}
		
		/**
		 * Copies a member function object into this object.
		 *
		 * @param that The member function object to clone.
		 */
		InstanceInvokable& operator = (const InstanceInvokable& that) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			_function = that._function;
			_instance = that._instance;
			
			return *this;
		}

		/**
		 * Deletes the member function object.
		 */
		virtual ~InstanceInvokable() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Invokes the function pointer contained by this class.
		 *
		 * @param a The first parameter to the function pointer.
		 * @param b The second parameter to the function pointer.
		 * @return The return value of the function pointer.
		 */
		virtual Return Invoke(A a, B b) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return (_instance->*_function)(a, b);
		}

		/**
		 * Makes a copy of this interface that references the same function pointer.
		 * The caller is responsible for deleting the clone.
		 *
		 * @return The clone.
		 */
		virtual Invokable* Clone() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return new InstanceInvokable<Instance>(_function, _instance);
		}

		/**
		 * Compares this invokable object to another invokable object.
		 *
		 * @param that The object to compare to.
		 * @return True if the objects reference the same function pointer and the same instance, false otherwise.
		 */
		virtual bool Equals(const Invokable& that) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			const InstanceInvokable<Instance>* other = dynamic_cast<const InstanceInvokable<Instance>*>(&that);
			return other && other->_function == _function && other->_instance == _instance;
		}
	};

	/**
	 * A class that represents an object that invokes a constant member function.
	 */
	template <typename Instance> class ConstantInstanceInvokable : public Invokable {
	private:
		Return (Instance::*_function)(A, B) const;
		const Instance* _instance;

	public:

		/**
		 * Creates a new constant member function object.
		 *
		 * @param function The function pointer to invoke.
		 * @param instance The instance to invoke the function for.
		 */
		ConstantInstanceInvokable(Return (Instance::*function)(A, B) const, const Instance* instance) : _function(function), _instance(instance) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
		
		/**
		 * Creates a new constant member function object from another.
		 *
		 * @param that The constant member function object to clone.
		 */
		ConstantInstanceInvokable(const ConstantInstanceInvokable& that) : _function(that._function), _instance(that._instance) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			
		}
		
		/**
		 * Copies a constant member function object into this object.
		 *
		 * @param that The constant member function object to clone.
		 */
		ConstantInstanceInvokable& operator = (const ConstantInstanceInvokable& that) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			_function = that._function;
			_instance = that._instance;
			
			return *this;
		}

		/**
		 * Deletes the constant member function object.
		 */
		virtual ~ConstantInstanceInvokable() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Invokes the function pointer contained by this class.
		 *
		 * @param a The first parameter to the function pointer.
		 * @param b The second parameter to the function pointer.
		 * @return The return value of the function pointer.
		 */
		virtual Return Invoke(A a, B b) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return (_instance->*_function)(a, b);
		}

		/**
		 * Makes a copy of this interface that references the same function pointer.
		 * The caller is responsible for deleting the clone.
		 *
		 * @return The clone.
		 */
		virtual Invokable* Clone() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return new ConstantInstanceInvokable<Instance>(_function, _instance);
		}

		/**
		 * Compares this invokable object to another invokable object.
		 *
		 * @param that The object to compare to.
		 * @return True if the objects reference the same function pointer and the same instance, false otherwise.
		 */
		virtual bool Equals(const Invokable& that) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			const ConstantInstanceInvokable<Instance>* other = dynamic_cast<const ConstantInstanceInvokable<Instance>*>(&that);
			return other && other->_function == _function && other->_instance == _instance;
		}
	};

public:

	/**
	 * Creates a new delegate that returns the default value when invoked.
	 */
	Delegate() : _invokable(0) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

	/**
	 * Creates a new delegate that is a clone of another delegate.
	 *
	 * @param that The delegate to clone.
	 */
	Delegate(const Delegate<Return (A, B)>& that) : _invokable(that._invokable ? that._invokable->Clone() : 0) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

	/**
	 * Creates a new delegate that invokes a static function.
	 *
	 * @param function The function to invoke.
	 */
	Delegate(Return (*function)(A, B)) : _invokable(new StaticInvokable(function)) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

	/**
	 * Creates a new delegate that invokes a member function.
	 *
	 * @param function The function to invoke.
	 * @param instance The instance to invoke on.
	 */
	template <typename Instance> Delegate(Return (Instance::*function)(A, B), Instance* instance) : _invokable(new InstanceInvokable<Instance>(function, instance)) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

	/**
	 * Creates a new delegate that invokes a constant member function.
	 *
	 * @param function The function to invoke.
	 * @param instance The instance to invoke on.
	 */
	template <typename Instance> Delegate(Return (Instance::*function)(A, B) const, const Instance* instance) : _invokable(new ConstantInstanceInvokable<Instance>(function, instance)) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

	/**
	 * Clones the specified delegate and assigns it to this delegate.
	 *
	 * @param that The delegate to clone.
	 * @return This delegate.
	 */
	Delegate<Return (A, B)>& operator = (const Delegate<Return (A, B)>& that) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		if (_invokable) {
			delete _invokable;
			_invokable = 0;
		}

		_invokable = that._invokable ? that._invokable->Clone() : 0;
		return *this;
	}

	/**
	 * Invokes the delegate.
	 *
	 * @param a The first parameter to the function pointer.
	 * @param b The second parameter to the function pointer.
	 * @return The return value of the function pointer.
	 */
	Return operator ()(A a, B b) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		if (_invokable) {
			return _invokable->Invoke(a, b);
		}

		throw EmptyDelegateException();
	}

	/**
	 * Compares this delegate to another delegate.
	 *
	 * @param that The delegate to compare to.
	 * @return True if the delegates are considered equal, false otherwise.
	 */
	bool operator == (const Delegate<Return (A, B)>& that) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		if (_invokable) {
			return that._invokable && _invokable->Equals(*that._invokable);
		}
		else {
			return that._invokable == 0;
		}
	}

	/**
	 * Deletes the delegate.
	 */
	virtual ~Delegate() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		if (_invokable) {
			delete _invokable;
			_invokable = 0;
		}
	}

	/**
	 * Returns an empty delegate.
	 *
	 * @return The empty delegate.
	 */
	static Delegate<Return (A, B)> Empty() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		static Delegate<Return (A, B)> empty;
		return empty;
	}

private:
	Invokable* _invokable;
};

template <typename Return, typename A, typename B> Delegate<Return (A, B)> delegate(Return (*function)(A, B)) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
	return Delegate<Return (A, B)>(function);
}

template <typename Return, typename A, typename B, typename Instance> Delegate<Return (A, B)> delegate(Return (Instance::*function)(A, B), Instance* instance) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
	return Delegate<Return (A, B)>(function, instance);
}

template <typename Return, typename A, typename B, typename Instance> Delegate<Return (A, B)> delegate(Return (Instance::*function)(A, B) const, const Instance* instance) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
	return Delegate<Return (A, B)>(function, instance);
}

/**
 * A specialized class that consolidates static functions and instance functions that take two parameters and return no value.
 */
template <typename A, typename B> class Delegate<void (A, B)> {
private:

	/**
	 * An interface from which both static functions and instance functions derive.
	 */
	class Invokable {
	public:

		/**
		 * Invokes the function pointer contained by this class.
		 *
		 * @param a The first parameter to the function pointer.
		 * @param b The second parameter to the function pointer.
		 */
		virtual void Invoke(A a, B b) const = 0;

		/**
		 * Makes a copy of this interface that references the same function pointer.
		 * The caller is responsible for deleting the clone.
		 *
		 * @return The clone.
		 */
		virtual Invokable* Clone() const = 0;

		/**
		 * Compares this invokable object to another invokable object.
		 *
		 * @param that The object to compare to.
		 * @return True if the objects are considered equal, false otherwise.
		 */
		virtual bool Equals(const Invokable& that) const = 0;

		/**
		 * Deletes the invokable object.
		 */
		virtual ~Invokable() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
	};

	/**
	 * A class that represents an object that invokes a static function.
	 */
	class StaticInvokable : public Invokable {
	private:
		void (*_function)(A, B);

	public:

		/**
		 * Creates a new static function object.
		 *
		 * @param function The function pointer to invoke.
		 */
		StaticInvokable(void (*function)(A, B)) : _function(function) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Deletes the static function object.
		 */
		virtual ~StaticInvokable() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Invokes the function pointer contained by this class.
		 *
		 * @param a The first parameter to the function pointer.
		 * @param b The second parameter to the function pointer.
		 */
		virtual void Invoke(A a, B b) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			_function(a, b);
		}

		/**
		 * Makes a copy of this interface that references the same function pointer.
		 * The caller is responsible for deleting the clone.
		 *
		 * @return The clone.
		 */
		virtual Invokable* Clone() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return new StaticInvokable(_function);
		}

		/**
		 * Compares this invokable object to another invokable object.
		 *
		 * @param that The object to compare to.
		 * @return True if the objects reference the same function pointer, false otherwise.
		 */
		virtual bool Equals(const Invokable& that) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			const StaticInvokable* other = dynamic_cast<const StaticInvokable*>(&that);
			return other && other->_function == _function;
		}
	};

	/**
	 * A class that represents an object that invokes a member function.
	 */
	template <typename Instance> class InstanceInvokable : public Invokable {
	private:
		void (Instance::*_function)(A, B);
		Instance* _instance;

	public:

		/**
		 * Creates a new member function object.
		 *
		 * @param function The function pointer to invoke.
		 * @param instance The instance to invoke the function for.
		 */
		InstanceInvokable(void (Instance::*function)(A, B), Instance* instance) : _function(function), _instance(instance) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
		
		/**
		 * Creates a new member function object from another.
		 *
		 * @param that The member function object to clone.
		 */
		InstanceInvokable(const InstanceInvokable& that) : _function(that._function), _instance(that._instance) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			
		}
		
		/**
		 * Copies a member function object into this object.
		 *
		 * @param that The member function object to clone.
		 */
		InstanceInvokable& operator = (const InstanceInvokable& that) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			_function = that._function;
			_instance = that._instance;
			
			return *this;
		}

		/**
		 * Deletes the member function object.
		 */
		virtual ~InstanceInvokable() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Invokes the function pointer contained by this class.
		 *
		 * @param a The first parameter to the function pointer.
		 * @param b The second parameter to the function pointer.
		 */
		virtual void Invoke(A a, B b) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			(_instance->*_function)(a, b);
		}

		/**
		 * Makes a copy of this interface that references the same function pointer.
		 * The caller is responsible for deleting the clone.
		 *
		 * @return The clone.
		 */
		virtual Invokable* Clone() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return new InstanceInvokable<Instance>(_function, _instance);
		}


		/**
		 * Compares this invokable object to another invokable object.
		 *
		 * @param that The object to compare to.
		 * @return True if the objects reference the same function pointer and the same instance, false otherwise.
		 */
		virtual bool Equals(const Invokable& that) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			const InstanceInvokable<Instance>* other = dynamic_cast<const InstanceInvokable<Instance>*>(&that);
			return other && other->_function == _function && other->_instance == _instance;
		}
	};

	/**
	 * A class that represents an object that invokes a constant member function.
	 */
	template <typename Instance> class ConstantInstanceInvokable : public Invokable {
	private:
		void (Instance::*_function)(A, B) const;
		const Instance* _instance;

	public:

		/**
		 * Creates a new constant member function object.
		 *
		 * @param function The function pointer to invoke.
		 * @param instance The instance to invoke the function for.
		 */
		ConstantInstanceInvokable(void (Instance::*function)(A, B) const, const Instance* instance) : _function(function), _instance(instance) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
		
		/**
		 * Creates a new constant member function object from another.
		 *
		 * @param that The constant member function object to clone.
		 */
		ConstantInstanceInvokable(const ConstantInstanceInvokable& that) : _function(that._function), _instance(that._instance) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			
		}
		
		/**
		 * Copies a constant member function object into this object.
		 *
		 * @param that The constant member function object to clone.
		 */
		ConstantInstanceInvokable& operator = (const ConstantInstanceInvokable& that) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			_function = that._function;
			_instance = that._instance;
			
			return *this;
		}

		/**
		 * Deletes the constant member function object.
		 */
		virtual ~ConstantInstanceInvokable() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Invokes the function pointer contained by this class.
		 *
		 * @param a The first parameter to the function pointer.
		 * @param b The second parameter to the function pointer.
		 */
		virtual void Invoke(A a, B b) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			(_instance->*_function)(a, b);
		}

		/**
		 * Makes a copy of this interface that references the same function pointer.
		 * The caller is responsible for deleting the clone.
		 *
		 * @return The clone.
		 */
		virtual Invokable* Clone() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return new ConstantInstanceInvokable<Instance>(_function, _instance);
		}

		/**
		 * Compares this invokable object to another invokable object.
		 *
		 * @param that The object to compare to.
		 * @return True if the objects reference the same function pointer and the same instance, false otherwise.
		 */
		virtual bool Equals(const Invokable& that) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			const ConstantInstanceInvokable<Instance>* other = dynamic_cast<const ConstantInstanceInvokable<Instance>*>(&that);
			return other && other->_function == _function && other->_instance == _instance;
		}
	};

public:

	/**
	 * Creates a new delegate that returns the default value when invoked.
	 */
	Delegate() : _invokable(0) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

	/**
	 * Creates a new delegate that is a clone of another delegate.
	 *
	 * @param that The delegate to clone.
	 */
	Delegate(const Delegate<void (A, B)>& that) : _invokable(that._invokable ? that._invokable->Clone() : 0) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

	/**
	 * Creates a new delegate that invokes a static function.
	 *
	 * @param function The function to invoke.
	 */
	Delegate(void (*function)(A, B)) : _invokable(new StaticInvokable(function)) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

	/**
	 * Creates a new delegate that invokes a member function.
	 *
	 * @param function The function to invoke.
	 * @param instance The instance to invoke on.
	 */
	template <typename Instance> Delegate(void (Instance::*function)(A, B), Instance* instance) : _invokable(new InstanceInvokable<Instance>(function, instance)) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

	/**
	 * Creates a new delegate that invokes a constant member function.
	 *
	 * @param function The function to invoke.
	 * @param instance The instance to invoke on.
	 */
	template <typename Instance> Delegate(void (Instance::*function)(A, B) const, const Instance* instance) : _invokable(new ConstantInstanceInvokable<Instance>(function, instance)) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

	/**
	 * Clones the specified delegate and assigns it to this delegate.
	 *
	 * @param that The delegate to clone.
	 * @return This delegate.
	 */
	Delegate<void (A, B)>& operator = (const Delegate<void (A, B)>& that) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		if (_invokable) {
			delete _invokable;
			_invokable = 0;
		}

		_invokable = that._invokable ? that._invokable->Clone() : 0;
		return *this;
	}

	/**
	 * Invokes the delegate.
	 *
	 * @param a The first parameter to the function pointer.
	 * @param b The second parameter to the function pointer.
	 */
	void operator ()(A a, B b) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		if (_invokable) {
			_invokable->Invoke(a, b);
		}
	}

	/**
	 * Compares this delegate to another delegate.
	 *
	 * @param that The delegate to compare to.
	 * @return True if the delegates are considered equal, false otherwise.
	 */
	bool operator == (const Delegate<void (A, B)>& that) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		if (_invokable) {
			return that._invokable && _invokable->Equals(*that._invokable);
		}
		else {
			return that._invokable == 0;
		}
	}

	/**
	 * Deletes the delegate.
	 */
	virtual ~Delegate() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		if (_invokable) {
			delete _invokable;
			_invokable = 0;
		}
	}

	/**
	 * Returns an empty delegate.
	 *
	 * @return The empty delegate.
	 */
	static Delegate<void (A, B)> Empty() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		static Delegate<void (A, B)> empty;
		return empty;
	}

private:
	Invokable* _invokable;
};

template <typename A, typename B> Delegate<void (A, B)> delegate(void (*function)(A, B)) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
	return Delegate<void (A, B)>(function);
}

template <typename A, typename B, typename Instance> Delegate<void (A, B)> delegate(void (Instance::*function)(A, B), Instance* instance) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
	return Delegate<void (A, B)>(function, instance);
}

template <typename A, typename B, typename Instance> Delegate<void (A, B)> delegate(void (Instance::*function)(A, B) const, const Instance* instance) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
	return Delegate<void (A, B)>(function, instance);
}

}

#endif /* DELEGATE_HPP_ */
