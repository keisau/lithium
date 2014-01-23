#ifndef __FSM_H__
#define __FSM_H__

#include "hashmap.h"
#include <vector>
#include <string.h>

template <typename _In, typename _State>
class fsm
{
protected:
	typedef pair <_State, _In> _input_pair_t;
	
	/*
	template <typename _T>
	struct _mem_less : public less <_T> {
		bool operator() (const _T &x, const _T &y) const;
	};

	typedef _mem_less<_input_pair_t>		_input_less_t;
	typedef _mem_less<_State>				_state_less_t;
	*/

	typedef li::hashmap<_input_pair_t, _State>	_transmap_t;
	typedef li::hashmap<_State, bool>			_finalmap_t;
public:
	fsm ();
	fsm (	_State				start, 
			_transmap_t			&transmap, 
			_finalmap_t			&finalmap
		);

	// write alphabet to state machine
	void operator<< (const _In input);
	
	// assignment
	fsm<_In, _State> & operator= (const fsm<_In, _State> &_fsm);

protected:
	_State				_initial;
	_State				_current;
	_transmap_t			_transmap;
	_finalmap_t			_finalmap;
};

template <typename _In, typename _State>
fsm<_In, _State>::fsm () 
{
}

template <typename _In, typename _State>
fsm<_In, _State>::fsm (_State start, _transmap_t &transmap, _finalmap_t &finalmap)
{
	_initial = start;
	_current = start;
	_transmap = _transmap_t (transmap);
	_finalmap = _finalmap_t (finalmap);
}

/**
 * operator<<()
 *
 * \brief		write input to state machine which will then transit
 *				to next state
 *
 * \param		input		: the input
 * \return					true if state machine is now in final state or
 *							false otherwise
 */
template <typename _In, typename _State>
void fsm<_In, _State>::operator<< (const _In input) 
{
	_input_pair_t input_pair = make_pair (_current, input);
	if (_transmap.find (input_pair) != _transmap.end())
		_current = _transmap[input_pair];
}

/**
 * \brief		assignment operator
 * \param		_fsm		the rvalue state machine
 */
template <typename _In, typename _State>
fsm<_In, _State> & fsm<_In, _State>::operator= (const fsm<_In, _State> &_fsm)
{
	_initial = _fsm._initial;
	_current = _fsm._current;
	_transmap = _fsm._transmap;
	_finalmap = _fsm._finalmap;
	return *this;
}

#endif // __FSM_H__
