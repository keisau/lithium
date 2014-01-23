#ifndef __MFSM_H__
#define __MFSM_H__

/**
 * \brief		Member Function State Machine
 * \version		1.0
 * \author		CHING Kei Sau
 * \copyright	ASM
 *
 * \details		A tailor-made state machine container in hope of eliminating
 *				switch-case bordom and frustration. That will possibly
 *				increase coding efficiency and productivity as long as
 *				readability and maintainability.
 *
 * 15-Oct-2013, CHING Kei Sau :		first documented with doxygen;
 *									first tagged version.
 */

/**
 * Member function state machine - A useful state machine container
 *
 * Using this container may need knowledge in state machine.
 *
 * INTRODUCTION
 * ----------------------------------------------------------------------------
 * Generally speaking, using this container requires
 *
 * 1. A class name whose member functions are to be hooked
 *		with the state machine.
 *
 * 2. Type of states. Default type is (long (<class>::*) (long))
 *
 * 3. Pointer of the class instance - required to invoke member
 *		functions by pointer.
 *
 * Since the type of states is (long (<class>::*) (long)), calling
 * member functions from this container require one parameter of type (long).
 *
 * Feel free to inherit this class to provide custom behavior and use state
 * of other types.
 * ----------------------------------------------------------------------------
 *
 * RESTRICTION
 * ----------------------------------------------------------------------------
 * All simple, powerful thing comes with certain restrictions:
 *
 * 1. All member function must be of the same type and in the same class
 * 2. Member function must take exactly one argument as non-void type and 
 *		return non-void values.
 * 3. Whenever the member function is needed to be protected/private, be
 *		friend to mfsm.
 * 4. Use it on a single-threaded object only. (mfsm is not thread-safe yet)
 * ----------------------------------------------------------------------------
 */

// standard libraries, no default namespace is assumed
#include <vector>

#include "fsm.h"

/**
 * Transition type for defining a transition.
 *
 * \tparam	S		: type of actual paramter
 * \tparam	T		: type of input alphabet
 */
template <typename S, typename T>
struct transition {
	S current; T input; S next;
	transition () {}
	transition (S _current, T _input, S _next) :
		current (_current), input (_input), next (_next) {}
};

/**
 * \brief		Shorthand functions for easy creation of transition struct and 
 *				final state vector.
 *
 * \tparam	S		: type of passed function pointers (actual parameter)
 * \tparam	T		: type of input alphabet
 *
 * \details		S can be deduced automatically by compiler; The formal state type
 * can be deduced from state machine template arguments to which a transition is 
 * added.
 *				<tt>make_transition (current, input, next)</tt> originates from
 *				transition table in state machine definition. 
 *
 *				This makes state machine programming just like writing a 
 *				transition table.
 */	
template <typename S, typename T>
transition<S, T> make_transition (const S current, T input, const S next) {
	return transition<S, T> (current, input, next);
}

/**
 * make_vector()
 * 
 * \brief		used to create a vector of final states
 *
 * \tparam		T		: type of actual parameter
 * \param		argc	: number of arguments
 */
template <typename T>
vector <T> make_vector (int argc, T ...)
{
	va_list vals;
	va_start (vals, argc);
	vector <T> retval = vector <T> ();

	for (int i = 0; i < argc; ++i)
		retval.push_back (va_arg (vals, T));

	return retval;
}

/**
 * \brief member function state machine using member function pointers
 * 
 * \details		Use <tt>constructor (<instance>, <_initial>, [final state]) </tt>
 *				to create a state machine. Use `+` operator with <tt>make_transition()</tt>
 *				to add a transition to the state machine object.
 *
 *				The `()` operator provides a shorthand to invoke the current state
 *				as member function using the provided object pointer and member
 *				function pointer.
 *
 *				The `<<` operator acts as stream operator. The state machine will
 *				transit based on the input stream. Supported types are the 
 *				alphabet type T and vector<T>.
 *
 * \tparam		C		: Class 
 * \tparam		S		: State type
 * \tparam		T		: Input alphabet, (long) by default
 * \tparam		R		: Output type, (long) by default
 */
template <class C, typename S, typename T = long, typename R = long>
class mfsm : public fsm <T, S> {
	/* protected comparator defs and typedefs */
protected:
	typedef transition<S, T> _transition;

	/* class functions */
public:
	/* constructors */
	// default
	mfsm ();

	/* operations */
	void set_start (S start);
	void set_object (C *object);
	void start ();

	/* end condition */
	mfsm<C, S, T, R> & add_final_state (S final);
	const bool end_at (S state);
	const bool is_ended ();

	/* operator overloads */
	// add transition
	template <typename _T>
	mfsm<C, S, T, R> & operator+ (const transition<S, _T> &new_transition);

	// assignment
	mfsm<C, S, T, R> & operator= (const mfsm<C, S, T, R> &_fsm);

	// run state machine function
	const R operator() (T param) const;

	/* protected variables */
protected:
	C			*_object;
};

/**
 * mfsm()
 *
 */
template <class C, typename S, typename T, typename R>
mfsm<C, S, T, R>::mfsm ()
{
}

/* operations */
/**
 * \brief		set start state
 * \param		start		the start state
 */
template <class C, typename S, typename T, typename R>
void mfsm<C, S, T, R>::set_start (S start) { _initial = start; }

/**
 * \brief		set class instance
 * \param		object		the class instance
 */
template <class C, typename S, typename T, typename R>
void mfsm<C, S, T, R>::set_object (C *object) { _object = object; }

/**
 * \brief		reset state machine to initial state
 */
template <class C, typename S, typename T, typename R>
void mfsm<C, S, T, R>::start () { _current = _initial; }

/* end condition */
/**
 * \brief		add a final state
 * \details		insert a record to the final state map to note that
 *				the passed parameter is a final state.
 * \param		S		the final state
 */
template <class C, typename S, typename T, typename R>
mfsm<C, S, T, R> & mfsm<C, S, T, R>::add_final_state (S final)
{
	_finalmap[final] = true;
	return *this;
}

/**
 * end_at ()
 *
 * \brief		determine if a given state is a final state
 * \param		S		: state, also a member function pointer
 * \return				true if it is a final state, or
 *						false otherwise
 */
template <class C, typename S, typename T, typename R>
const bool mfsm<C, S, T, R>::end_at (S state) 
{
	return _finalmap[state];
}

/**
 * is_ended ()
 *
 * \brief		determine if the state machine is in final state
 * \return				true if it is in final state, or
 *						false otherwise
 */
template <class C, typename S, typename T, typename R>
const bool mfsm<C, S, T, R>::is_ended () { return end_at (_current); }

/**
 * \brief		add a transition
 *
 * \details		By using the transition struct, one may insert additional
 *				transitions to the state machine with + operator.
 * \param		new_transition			the new transition
 */
template <class C, typename S, typename T, typename R>
	template <typename _T>
mfsm<C, S, T, R> & mfsm<C, S, T, R>::operator+ (const transition<S, _T> &new_transition)
{
	pair<S, T> input_pair = 
		make_pair (new_transition.current, 
				(T)new_transition.input);

	S next_state = new_transition.next;
	_transmap[input_pair] = next_state;

	return *this;
}

/**
 * \brief		assignment operator
 * \param		_fsm		the rvalue state machine
 */
template <class C, typename S, typename T, typename R>
mfsm<C, S, T, R> & mfsm<C, S, T, R>::operator= (const mfsm<C, S, T, R> &_fsm)
{
	_object = _fsm._object;
	fsm<T, S>::operator= ((fsm<T, S>)_fsm);
	return *this;
}

/**
 * operator()()
 *
 * \brief	run state machine function
 * \param	param		: the param of type T
 * \return				return value of the member function in type R
 */
template <class C, typename S, typename T, typename R>
const R mfsm<C, S, T, R>::operator() (T param) const 
{
	return (_object->*_current) (param);
}

#endif	//	__MFSM_H__
