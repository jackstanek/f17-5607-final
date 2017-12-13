#include <iostream>
#include <random>
#include <vector>

#include "dungeon.hpp"

Dungeon::Dungeon(int complexity)
{
	m_width = 236;
	m_height = 60;
	m_cells = std::vector<char>(m_width * m_height, TT_OPEN);
	m_rooms = std::vector<Room>();
	m_exits = std::vector<Room>();
	generate_dungeon(complexity);
}

// not sure if having variable width and height is a good idea
// and may be unneeded as complexity is typically the drawing cap
Dungeon::Dungeon(int width, int height, int complexity)
{
	m_width = width;
	m_height = height;
	m_cells = std::vector<char>(m_width * m_height, TT_OPEN);
	m_rooms = std::vector<Room>();
	m_exits = std::vector<Room>();
	generate_dungeon(complexity);
}

// generates dungeon layout
void Dungeon::generate_dungeon(int complexity)
{
	// failure to make initial room
	if (!make_room(m_width / 2, m_height / 2, random_int(4), true))
	{
		return;
	}
	// iterate until you reach the desired complexity (number of rooms/hallways)
	for (int i = 0; i < complexity; ++i)
	{
		if (m_exits.empty())
		{
			break;
		}
		int r = random_int(m_exits.size());
		int x = random_int(m_exits[r].x, m_exits[r].x + m_exits[r].width - 1);
		int y = random_int(m_exits[r].y, m_exits[r].y + m_exits[r].height - 1);
		
		for (int j = 0; j < 4; ++j)
		{
			if (add_room(x, y, j))
			{
				break;
			}
		}
	}
	// need to check that there is path between start and goal, if not reroll map
	// I'm not certain if every map is solvable currently

	// need to resolve something if no start is placed
	// haven't had an issue with this on non-trivial map sizes
	if (!place_object(TT_START))
	{
		return;
	}
	// need to resolve something if no goal is placed
	// haven't had an issue with this on non-trivial map sizes
	if (!place_object(TT_GOAL))
	{
		return;
	}
	// hallways are distinct as generators, but drawn as floors
	for (unsigned int i = 0; i < m_cells.size(); ++i)
	{
		// remove TT_CLOSED_DOOR if we get door animation working
		if (m_cells.at(i) == TT_FLOOR || m_cells.at(i) == TT_HALLWAY || m_cells.at(i) == TT_CLOSED_DOOR)
		{
			m_cells.at(i) = TT_FLOOR;
		}
	}
}

void Dungeon::print()
{
	for (int y = 0; y < m_height; ++y)
	{
		for (int x = 0; x < m_width; ++x)
		{
			std::cout << get_cell(x, y);
		}
		std::cout << std::endl;
	}
}

char Dungeon::get_cell(int x, int y)
{
	if (x < 0 || y < 0 || x >= m_width || y >= m_height)
		return TT_OPEN;
	return m_cells.at(x + y * m_width);
}

void Dungeon::set_cell(int x, int y, char cell)
{
	m_cells.at(x + y * m_width) = cell;
}

bool Dungeon::add_room(int x, int y, int dir)
{
	int dx = 0;
	int dy = 0;

	if (dir == North)
	{
		dy = 1;
	}
	else if (dir == South)
	{
		dy = -1;
	}
	else if (dir == West)
	{
		dx = 1;
	}
	else if (dir == East)
	{
		dx = -1;
	}
	// can only add rooms at floor and hallway cells
	if (get_cell(x + dx, y + dy) != TT_FLOOR && get_cell(x + dx, y + dy) != TT_HALLWAY)
	{
		return false;
	}
	// to make room, or not to make room
	if (random_bool())
	{
		if (make_room(x, y, dir, true))
		{
			set_cell(x, y, TT_CLOSED_DOOR);
			return true;
		}
	}
	// if it chooses not to make a room, it tries to make a hallway
	else if (make_hallway(x, y, dir))
	{
		if (get_cell(x + dx, y + dy) == TT_FLOOR)
		{
			set_cell(x, y, TT_CLOSED_DOOR);
		}
		else
		{
			set_cell(x, y, TT_HALLWAY);
		}
		return true;
	}
	return false;
}

bool Dungeon::make_room(int x, int y, int dir, bool start_room = false)
{
	// can tweak these if rooms seem too small or big
	static const int min_size = 3;
	static const int max_size = 6;

	Room room { 0,0,0,0 };

	if (dir == North)
	{
		room = Room{ x - room.width / 2, y - room.height, random_int(min_size, max_size), random_int(min_size, max_size) };
	}
	else if (dir == South)
	{
		room = Room{ x - room.width / 2, y + 1, random_int(min_size, max_size), random_int(min_size, max_size) };
	}
	else if (dir == West)
	{
		room = Room{ x - room.width, y - room.height / 2, random_int(min_size, max_size), random_int(min_size, max_size) };
	}
	else if (dir == East)
	{
		room = Room{ x + 1, y - room.height / 2, random_int(min_size, max_size), random_int(min_size, max_size) };
	}

	if (place_room(room, TT_FLOOR))
	{
		m_rooms.emplace_back(room);
		// ignore incoming direction, as its already been visited
		if (dir != South || start_room)
		{
			m_exits.emplace_back(Room{ room.x, room.y - 1, room.width, 1 });
		}
		if (dir != North || start_room)
		{
			m_exits.emplace_back(Room{ room.x, room.y + room.height, room.width, 1 });
		}
		if (dir != East || start_room)
		{
			m_exits.emplace_back(Room{ room.x - 1, room.y, 1, room.height });
		}
		if (dir != West || start_room)
		{
			m_exits.emplace_back(Room{ room.x + room.width, room.y, 1, room.height });
		}
		return true;
	}
	return false;
}

bool Dungeon::make_hallway(int x, int y, int dir)
{
	// can tweak these to improve look if need
	static const int min_length = 4;
	static const int max_length = 10;

	Room hallway;
	hallway.x = x;
	hallway.y = y;
	// x hallway
	if (random_bool())
	{
		hallway.width = random_int(min_length, max_length);
		hallway.height = 1;

		if (dir == North)
		{
			hallway.y = y - 1;

			if (random_bool())
			{
				hallway.x = x - hallway.width + 1;
			}
		}
		else if (dir == South)
		{
			hallway.y = y + 1;

			if (random_bool())
			{
				hallway.x = x - hallway.width + 1;
			}
		}
		else if (dir == West)
		{
			hallway.x = x - hallway.width;
		}
		else if (dir == East)
		{
			hallway.x = x + 1;
		}

		if (place_room(hallway, TT_HALLWAY))
		{
			if (dir != South && hallway.width != 1)
			{
				m_exits.emplace_back(Room{ hallway.x, hallway.y - 1, hallway.width, 1 });
			}
			if (dir != North && hallway.width != 1)
			{
				m_exits.emplace_back(Room{ hallway.x, hallway.y + hallway.height, hallway.width, 1 });
			}
			return true;
		}
	}
	// y hallway
	else 
	{
		hallway.width = 1;
		hallway.height = random_int(min_length, max_length);

		if (dir == North)
		{
			hallway.y = y - hallway.height;
		}
		else if (dir == South)
		{
			hallway.y = y + 1;
		}
		else if (dir == West)
		{
			hallway.x = x - 1;

			if (random_bool())
			{
				hallway.y = y - hallway.height + 1;
			}
		}
		else if (dir == East)
		{
			hallway.x = x + 1;

			if (random_bool())
			{
				hallway.y = y - hallway.height + 1;
			}
		}

		if (place_room(hallway, TT_HALLWAY))
		{
			if (dir != East && hallway.height != 1)
			{
				m_exits.emplace_back(Room{ hallway.x - 1, hallway.y, 1, hallway.height });
			}
			if (dir != West && hallway.height != 1)
			{
				m_exits.emplace_back(Room{ hallway.x + hallway.width, hallway.y, 1, hallway.height });
			}
			return true;
		}
	}
	return false;
}

bool Dungeon::place_room(Room room, char cell)
{
	// make sure room is not out of bounds
	if (room.x < 1 || room.y < 1 || room.x + room.width > m_width - 1 || room.y + room.height > m_height - 1)
	{
		return false;
	}

	for (int y = room.y; y < room.y + room.height; ++y)
	{
		for (int x = room.x; x < room.x + room.width; ++x)
		{
			if (get_cell(x, y) != TT_OPEN)
			{
				return false;
			}
		}
	}

	for (int y = room.y - 1; y < room.y + room.height + 1; ++y)
	{
		for (int x = room.x - 1; x < room.x + room.width + 1; ++x)
		{
			// place walls around room
			if (x == room.x - 1 || y == room.y - 1 || x == room.x + room.width || y == room.y + room.height)
			{
				set_cell(x, y, TT_WALL);
			}
			else
			{
				set_cell(x, y, cell);
			}
		}
	}
	return true;
}

bool Dungeon::place_object(char cell)
{
	if (m_rooms.empty())
	{
		return false;
	}

	// get random room
	int r = random_int(m_rooms.size());
	// get random x,y inside room
	int x = random_int(m_rooms.at(r).x + 1, m_rooms.at(r).x + m_rooms.at(r).width - 1);
	int y = random_int(m_rooms.at(r).y + 1, m_rooms.at(r).y + m_rooms.at(r).height - 1);

	if (get_cell(x, y) == TT_FLOOR)
	{
		set_cell(x, y, cell);
		//m_rooms.erase(m_rooms.begin() + r);
		return true;
	}
	return false;
}

int Dungeon::random_int(int max)
{
	std::random_device random_device;
	std::mt19937 mt(random_device());
	std::uniform_int_distribution<> distribution(0, max - 1);
	return distribution(mt);
}

int Dungeon::random_int(int min, int max)
{
	std::random_device random_device;
	std::mt19937 mt(random_device());
	std::uniform_int_distribution<> distribution(0, max - min);
	return distribution(mt) + min;
}

bool Dungeon::random_bool()
{
	std::random_device random_device;
	std::mt19937 mt(random_device());
	std::bernoulli_distribution distribution(0.5);
	return distribution(mt);
}
