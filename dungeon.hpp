#ifndef DUNGEON_HPP_
#define DUNGEON_HPP_

#include <iostream>
#include <random>
#include <vector>

#include "map.hpp"

struct Room
{
    int x, y;
    int width, height;
};

class Dungeon
{
public:
    enum Direction {
        North,
        South,
        West,
        East,
    };

    Dungeon(int);
    Dungeon(int, int, int);
    void generate_dungeon(int);
    void print();
    int get_cell(int, int);
    std::vector<int> m_cells;
    int startx, starty;
private:
    void set_cell(int, int, int);
    bool add_room(int, int, int);
    bool make_room(int, int, int, bool);
    bool make_hallway(int, int, int);
    bool place_room(Room, int);
    bool place_object(int);
    int random_int(int);
    int random_int(int, int);
    bool random_bool();
    int m_width, m_height;
    
    std::vector<Room> m_rooms;
    std::vector<Room> m_exits;
};

#endif
