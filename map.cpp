#include <cassert>
#include <fstream>
#include <stdint.h>

#include "map.hpp"

Map::Map(int w, int h) :
    keys_collected(5),
    key_collected_count(0),
    w(w),
    h(h),
    tiles(new int[w * h])
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

Map* Map::ParseMapFile(const char* path)
{
    std::ifstream map_file;
    map_file.open(path);

    if (!map_file) {
        return nullptr;
    }

    uint32_t w = 0, h = 0;
    map_file >> w >> h;
    assert(w > 0 && h > 0);

    Map* m = new Map(w, h);
    int type;
    char curr;
    int door_ct = 0, key_ct = 0;

    for (uint32_t j = 0; j < h; j++) {
        for (uint32_t i = 0; i < w; i++) {
            map_file >> curr;
            switch (curr) {
            case '0':
                type = TT_OPEN;
                break;
            case 'W':
                type = TT_WALL;
                break;
            case 'A':
                type = TT_DOOR_A;
                door_ct++;
                break;
            case 'B':
                type = TT_DOOR_B;
                door_ct++;
                break;
            case 'C':
                type = TT_DOOR_C;
                door_ct++;
                break;
            case 'D':
                type = TT_DOOR_D;
                door_ct++;
                break;
            case 'E':
                type = TT_DOOR_E;
                door_ct++;
                break;
            case 'a':
                type = TT_KEY_A;
                key_ct++;
                break;
            case 'b':
                type = TT_KEY_B;
                key_ct++;
                break;
            case 'c':
                type = TT_KEY_C;
                key_ct++;
                break;
            case 'd':
                type = TT_KEY_D;
                key_ct++;
                break;
            case 'e':
                type = TT_KEY_E;
                key_ct++;
                break;
            case 'S':
                m->startx = i;
                m->starty = h - j - 1;
                type = TT_START;
                break;
            case 'G':
                type = TT_GOAL;
                break;
            case '\n':
                continue;
            default:
                return nullptr;
                break;
            }

            m->SetTile(i, h - j - 1, type);
        }
    }

    assert(key_ct == door_ct);
    m->total_key_count = key_ct;
    return m;
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
    int index = -1;
    if (IsKey(key)) {
        index = KeyIndex(key);
    } else if (IsDoor(key)) {
        index = DoorIndex(key);
    }
    assert(index >= 0);

    return keys_collected[index];
}

Character* Map::NewPlayerAtStart(int model_id) const
{
    return new Character(startx, starty, model_id);
}
