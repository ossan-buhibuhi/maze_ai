// state.hpp

#ifndef MAP_HPP 
#define MAP_HPP
#include <tuple>
#include <string>
#include <vector>

using MapXY = std::tuple<int, int>;
using MapXYs = std::vector<MapXY>;

enum class What { APPLE = '$', BLOCK = '#', SPACE = ' ', NONE = '?' };
enum class Direct { NOTRH, EAST, SOUTH, WEST, NONE };
const MapXY Directs [4] = { MapXY (0, 1), MapXY (1, 0), MapXY (0, -1), MapXY (-1, 0) };

class Map {
	int width, height;
	std::string str;
public:
	Map() 
	: width (-1), height (-1)
	{}
	void set (int w, int h, const char* s)
	{
		str = s;
		width = w;
		height = h;
		assert (str.length() == size_t (width * height));
	}
	MapXY find (char c) {
		auto at = str.find (c);
		return MapXY (at % width, at / width);
	}
	bool is_inside (int x, int y)
		{ return x >= 0 and x < width and y >=0 and y < height; }
	char get_at (int x, int y) {
		if (!is_inside (x, y))
			return '#';
		return str [y * width + x];
	}
	void set_at (int x, int y, char c) {
		assert (is_inside (x, y));
		str [y * width + x] = c;
	}
public:
	static Direct get_direction (int x1, int y1, int x2, int y2) {
		for (int i=0; i<static_cast<int>(Direct::NONE); i++) {
			auto&& direct = Directs [i];
			int dx = std::get<0> (direct);
			int dy = std::get<1> (direct);
			if (x1 + dx == x2 and y1 + dy == y2)
				return static_cast<Direct> (i);
		}
		return Direct::NONE;
	}
};

#endif
