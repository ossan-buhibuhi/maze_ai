// act.hpp

#ifndef ACTS_HPP
#define ACTS_HPP
#include <vector>
#include <memory>
#include "act.hpp"
#include "agent.hpp"
#include "state.hpp"
#include "map.hpp"

class ActMove : public Act {
	Direct direct;
public:
	ActMove (Direct direct)
	: direct (direct) {}
	virtual void process (State& state, Agent& agent) {
		puts ("ActMove::process()");
		state.act_move (agent, direct);
	}
};

class ActGet : public Act {
	What what;
public:
	ActGet (What what)
	: what (what) {}
	virtual void process (State& state, Agent& agent) {
		puts ("ActGet::process()");
		state.act_get (agent, what);
	}
};
#endif

