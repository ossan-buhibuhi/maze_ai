#ifndef MISSION_HPP
#define MISSION_HPP

#include <memory>
#include <vector>
#include <cstdlib>
#include "act.hpp"

class Mission;
using MissionPtr = std::unique_ptr<Mission>;
using Missions = std::vector<MissionPtr>;

class Agent;

class Mission {
protected:
	Missions sub_missions;
public:
	virtual ActPtr do_turn (const Agent& agent) {
		auto&& top_mission = sub_missions.back();
		return top_mission->do_turn (agent);
	}                  
	virtual bool achieved(const Agent& agent) { return is_clear(); }
public:
	void erase_achieved (Agent& agent)
	{
		while (sub_missions.size()) {
			auto&& top_mission = sub_missions.back();
			top_mission->erase_achieved (agent);
			if (!top_mission->achieved (agent))
				break;
			sub_missions.pop_back();
		}
	}
	void clear_sub_missions ()
		{ sub_missions.clear(); }
	void add_sub_mission (MissionPtr&& mission)
	{
		sub_missions.push_back (std::move (mission));
	}
	bool is_clear() { return sub_missions.empty(); }
};

#endif

