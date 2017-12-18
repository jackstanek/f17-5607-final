#include <cassert>
#include <fstream>
#include <stdint.h>

#include "map.hpp"

Map::Map(int w, int h) :
    keys_collected(5),
    key_collected_count(0),
    w(w),
    h(h),
    tiles(std::vector<int>(w * h, TT_OPEN))
{
    for (int i = 0; i < 5; i++) {
        keys_collected[i] = false;
    }
}

int Map::GetWidth() const
{
    return w;
}

int Map::GetHeight() const
{
    return h;
}

int Map::TileAtPoint(int x, int y) const
{
    return tiles[y * w + x];
}

void Map::SetTile(int x, int y, int type)
{
    tiles[y * w + x] = type;
}

Map* Map::ParseMapFile(int complexity)
{
    //std::ifstream map_file;
    //map_file.open(path);
    //
    //if (!map_file) {
    //    return nullptr;
    //}

    //uint32_t w = 0, h = 0;
    //map_file >> w >> h;
    //assert(w > 0 && h > 0);

    Map* m = new Map(236, 60);
    //int type;
    //char curr;
    int door_ct = 0, key_ct = 0;

	Dungeon dungeon = Dungeon(m->w, m->h, complexity);
	
	
	m->startx = dungeon.startx;
	m->starty = dungeon.starty;
	
	//dungeon.print();
	
	for (unsigned int i = 0; i < m->w; ++i) {
		for (unsigned int j = 0; j < m->h; ++j) {
			//std::cout << dungeon.get_cell(i, j) << std::endl;
			m->SetTile(i, j, dungeon.get_cell(i, j));
		}
	}

	std::cout << std::endl;
    
    assert(key_ct == door_ct);
    m->total_key_count = key_ct;
    return m;
}

void Map::print_map()
{
	unsigned int min_x = w, min_y = h;
	unsigned int max_x = 0, max_y = 0;
	for (unsigned int y = 0; y < h; ++y) {
        for (unsigned int x = 0; x < w; ++x) {
			if (TileAtPoint(x, y) != ' ') {
				if (x < min_x) {
					min_x = x;
				}
				if (y < min_y) {
					min_y = y;
				}
				if (x > max_x) {
					max_x = x;
				}
				if (y > max_y) {
					max_y = y;
				}
			}
		}
	}
	
	//printf("x: min %d max %d, y: min %d, max %d\n", min_x, max_x, min_y, max_y);
	printf("width = %d, height = %d\n", max_x - min_x, max_y - min_y);
	for (unsigned int y = 0; y <= max_y - min_y; ++y) {
        for (unsigned int x = 0; x <= max_x - min_x; ++x) {
            std::cout << static_cast<char>(TileAtPoint(max_x - x, y + min_y));
        }
        std::cout << std::endl;
    }
}

void Map::CollectKey(int key)
{
    if (key_collected_count < total_key_count) {
        key_collected_count++;
    }
    keys_collected[KeyIndex(key)] = true;
}

bool Map::KeyCollected(int key) const
{
    //int index = -1;
    //if (IsKey(key)) {
    //    index = KeyIndex(key);
    //} else if (IsDoor(key)) {
    //    index = DoorIndex(key);
    //}
    //assert(index >= 0);

    //return keys_collected[index];
    return true;
}

Character* Map::NewPlayerAtStart(int model_id) const
{
    return new Character(startx, starty, model_id);
}
