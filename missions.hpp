#ifndef MISSIONS_HPP 
#define MISSIONS_HPP

#include <memory>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include "mission.hpp"
#include "agent.hpp"
#include "acts.hpp"
#include "map.hpp"

class MissionMove : public Mission {
	int x, y;
public:
	MissionMove (MapXY xy)
	: x (std::get<0> (xy)), y (std::get<0> (xy))
	{}
	MissionMove (int x, int y)
	: x (x), y (y)
	{}
	int get_x() { return x; }
	int get_y() { return y; }
	virtual ActPtr do_turn (const Agent& agent) {
		int ax = agent.get_x();
		int ay = agent.get_y();
		if (agent.can_move (x, y)) { 
			clear_sub_missions();
			return ActPtr (new ActMove (Map::get_direction (ax, ay, x, y)));
		}
		if (!sub_missions.size()) {
			auto&& xys = agent.get_route (ax, ay, x, y);
			if (!xys.size())
				abort();
			for (auto&& xy : xys) {
				int sx = std::get<0> (xy);
				int sy = std::get<1> (xy);
				printf ("add route (%d, %d)\n", sx ,sy); 
				add_sub_mission (MissionPtr (new MissionMove (sx, sy)));
			}
		}
		auto&& last = sub_missions.back();
		auto&& ret = last->do_turn (agent);
		sub_missions.pop_back();
		return std::move (ret);
	}
	virtual bool achieved(const Agent& agent) { return agent.get_x() == x and agent.get_y() == y; }	
};

class MissionFind : public Mission {
	What what;
public:
	MissionFind (What what)
	: what (what)
	{}
	What get_what () { return what; }
	virtual bool achieved (const Agent& agent) { return agent.get_xy() != agent.find (what); }
	virtual ActPtr do_turn (const Agent& agent) {
		auto xy = agent.find (what);
		if (agent.get_xy() != xy) {
			sub_missions.clear();
			sub_missions.push_back (MissionPtr (new MissionMove (xy)));
		} else if (!sub_missions.size()) {
			MapXY xy = agent.get_nearest_unreached ();
			int x = std::get<0> (xy);
			int y = std::get<1> (xy);
			if (x == agent.get_x() and y == agent.get_y()) 
				std::abort();
			add_sub_mission (MissionPtr (new MissionMove (x, y)));
		}
		auto&& last = sub_missions.back();
		return last->do_turn (agent);
	}
};

class MissionGet : public Mission {
	What what;
public:
	MissionGet (What what)
	: what (what)
	{}
	What get_what () { return what; }
	virtual bool achieved(const Agent& agent) { return agent.have (what); }	
	virtual ActPtr do_turn (const Agent& agent) {
		if (agent.get_at() == what)
			return ActPtr (new ActGet (what));
		if (!sub_missions.size())
			add_sub_mission (MissionPtr (new MissionFind (what)));
		auto&& last = sub_missions.back();
		return last->do_turn (agent);
	}
};

#endif

