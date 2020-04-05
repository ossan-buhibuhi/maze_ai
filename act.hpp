// act.hpp

#ifndef ACT_HPP
#define ACT_HPP
#include <vector>
#include <memory>

class State;
class Agent;

class Act {
public:
	virtual void process (State& state, Agent& agent)=0;
};

using ActPtr = std::unique_ptr<Act>;
using Acts = std::vector<ActPtr>;

#endif
