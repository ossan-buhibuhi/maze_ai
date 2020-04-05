// agent.hpp

#ifndef AGENT_HPP
#define AGENT_HPP
#include <map>
#include <tuple>
#include <set>
#include <memory>
#include <algorithm>
#include <cmath>

#include "mission.hpp"
#include "map.hpp"

using MapMap = std::map<MapXY, char>;
using XySet = std::set<MapXY>;

class Agent {
	MapMap map_map;
	XySet unreached;
	std::vector<What> posses; // possessions
	Mission top_mission;
	int x, y;
public:
	Agent (int x=0, int y=0)
	: x (x), y (y) {}
	int get_x() const { return x; }
	int get_y() const { return y; }
	MapXY get_xy() const { return MapXY (x, y); }
	void set_xy (int _x, int _y) { x = _x; y = _y; }
	bool have (What what) const { return posses.end() != std::find (posses.begin(), posses.end(), what); }
	void take (What what) { posses.push_back (what); }
	Mission& get_mission() { return top_mission; }
	void update_knowledge (const char* surround)
	{
		unreached.erase (MapXY (x, y));
		for (int sy=0; sy<3; sy++) {
			for (int sx=0; sx<3; sx++) {
				int mx = sx - 1 + x;
				int my = sy - 1 + y;
				int at = surround [sy * 3 + sx];
				putchar (at);
				MapXY xy (mx, my); 
				if (at != '#' and !(x == mx and y == my) and map_map.find (xy) == map_map.end()) 
					// 周囲の移動可能地点がmap_mapになければ未踏に追加。
					unreached.insert (xy);
				map_map [xy] = at;
			}
			puts ("");
		}
	}
	void move (Direct direct) {
		auto&& xy = Directs [static_cast<int> (direct)];
		x += std::get<0> (xy);
		y += std::get<1> (xy);
	}
	MapXYs get_route (int x1, int y1, int x2, int y2) const {
		struct {
			int x2, y2;
			const MapMap& map_map;
			MapXYs ret, vec;
			void recur (int x, int y) {
				// printf ("get_route::recur(): x=%d, y=%d\n", x, y);
				MapXY xy (x, y);
				if (std::find (vec.begin(), vec.end(), xy) != vec.end())
					return;
				auto it = map_map.find (xy);
				if (it == map_map.end())
					return;
				int c = it->second;
				if (c == '#')
					return;
				if (x == x2 and y == y2) {
					if (!ret.size() or ret.size() > vec.size())
						ret = vec;
					return;
				}
				vec.push_back (xy);
				recur (x, y - 1);
				recur (x + 1, y);
				recur (x, y + 1);
				recur (x - 1, y);
				vec.pop_back();
			}
		} self = {x2, y2, map_map, MapXYs(), MapXYs()};
		self.recur (x1, y1);
		std::reverse (self.ret.begin(), self.ret.end());
		self.ret.pop_back();
		return std::move (self.ret);
	}
	bool can_move (int mx, int my) const {
		MapXY dirs[] = {MapXY (x + 1, y), MapXY (x, y + 1), MapXY (x - 1, y), MapXY (x, y - 1)};
		for (auto& xy : dirs) {
			int dx = std::get<0> (xy);
			int dy = std::get<1> (xy);
			if (dx == mx and dy == my and map_map.at (xy) != '#')
				return true;
		}
		return false;
	}
	double get_dist (MapXY xy1, MapXY xy2) const {
		int x1 = std::get<0> (xy1);
		int y1 = std::get<1> (xy1);
		int x2 = std::get<0> (xy2);
		int y2 = std::get<1> (xy2);
		int a = std::abs (x1 - x2);
		int b = std::abs (y1 - y2);
		return std::sqrt (a * a + b * b);
	}
	MapXY find (What what) const {
		MapXY cur_xy (x, y);
		MapXY ret_xy (x, y);
		double ret_d = 0.0;
		for (auto it=map_map.begin(); it != map_map.end(); ++it) {
			auto xy = it->first;
			auto c = it->second;
			if (c == static_cast<char> (what)) {
				auto d = get_dist (cur_xy, xy);
				if (d < ret_d) {
					ret_xy = xy;
					ret_d = d;
				}
			}
		}
		return ret_xy;
	}
	MapXY get_nearest_unreached() const {
		MapXY ret (x, y);
		double max = 0.0;
		for(auto xy : unreached) {
			int ux = std::get<0> (xy);
			int uy = std::get<1> (xy);
			int a = std::abs (ux - x);
			int b = std::abs (uy - y);
			double d = std::sqrt (a * a + b * b);
			if (d > max)
				ret = xy;
		}
		return ret;
	}
	What get_at (MapXY xy) const {
		if (map_map.find (xy) == map_map.end())
			return What::NONE;
		return static_cast<What> (map_map.at (xy));
	}
	What get_at() const { return get_at (get_xy()); }
};

#endif
