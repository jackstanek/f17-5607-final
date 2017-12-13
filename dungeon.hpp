#ifndef DUNGEON_HPP_
#define DUNGEON_HPP_

#include <iostream>
#include <random>
#include <vector>

struct Room
{
	int x, y;
	int width, height;
};

class Dungeon
{
public:
	enum TileType {
		TT_OPEN = ' ',
		TT_START = 'U',
		TT_GOAL = 'D',
		// TT_KEY_A,
		// TT_KEY_B,
		// TT_KEY_C,
		// TT_KEY_D,
		// TT_KEY_E,
		// TT_DOOR_A,
		// TT_DOOR_B,
		// TT_DOOR_C,
		// TT_DOOR_D,
		// TT_DOOR_E,
		TT_WALL = '#',
		//TT_N_TYPES,
		TT_FLOOR = '.',
		TT_HALLWAY = ',',
		TT_CLOSED_DOOR = '+',
		TT_OPEN_DOOR = '-'
	};

	enum Direction
	{
		North,
		South,
		West,
		East,
	};

public:
	Dungeon(int);
	Dungeon(int, int, int);
	void generate_dungeon(int);
	void print();
private:
	char get_cell(int, int);
	void set_cell(int, int, char);
	bool add_room(int, int, int);
	bool make_room(int, int, int, bool);
	bool make_hallway(int, int, int);
	bool place_room(Room, char);
	bool place_object(char);
	int random_int(int);
	int random_int(int, int);
	bool random_bool();
	int m_width, m_height;
	std::vector<char> m_cells;
	std::vector<Room> m_rooms;
	std::vector<Room> m_exits;
};

#endif
