#include <assert.h>
#include "api.hpp"
#include "state.hpp"
#include "missions.hpp"

void S7::api_set_map (long long _w, long long _h, const char* s)
{
	int w = static_cast<int> (_w);
	int h = static_cast<int> (_h);
	auto&& state = State::get();
	state.set_map (w, h, s);
}

void S7::api_comm_move (long long _x, long long _y)
{
	int x = static_cast<int> (_x);
	int y = static_cast<int> (_y);
	auto&& state = State::get();
	auto&& agent = state.get_agent();
	auto&& mission = MissionPtr (new MissionMove (x, y));
	auto&& top_mission = agent.get_mission();
	top_mission.add_sub_mission (std::move (mission));
}

void S7::api_comm_get (long long _what)
{
	auto what = static_cast<What> (_what);
	auto&& state = State::get();
	auto&& agent = state.get_agent();
	auto&& mission = MissionPtr (new MissionGet (what));
	auto&& top_mission = agent.get_mission();
	top_mission.add_sub_mission (std::move (mission));
}

void S7::api_comm_find (long long _what)
{
	auto what = static_cast<What> (_what);
	auto&& state = State::get();
	auto&& agent = state.get_agent();
	auto&& mission = MissionPtr (new MissionFind (what));
	auto&& top_mission = agent.get_mission();
	top_mission.add_sub_mission (std::move (mission));
}

void S7::api_start (long long _cnt)
{
	int cnt = static_cast<int> (_cnt);
	auto&& state = State::get();
	auto&& agent = state.get_agent();
	auto&& top_mission = agent.get_mission();
	for (int i=0; i<cnt; i++) 
	{
		printf ("**TURN%d**\n", i);
		if (state.do_turn())
			return;
		state.out();
		if (top_mission.is_clear())
			break;
	}
}
