#ifndef MAP_HPP_
#define MAP_HPP_

#include <cassert>
#include <vector>
#include <stdint.h>

#include "glad/glad.h"

#include "character.hpp"
#include "dungeon.hpp"

enum TileType {
    TT_OPEN = ' ',
    TT_START = 'S',
    TT_GOAL = 'G',
    TT_KEY_A = 'A',
    TT_KEY_B = 'B',
    TT_KEY_C = 'C',
    TT_KEY_D = 'D',
    TT_KEY_E = 'E',
    TT_DOOR_A = 'a',
    TT_DOOR_B = 'b',
    TT_DOOR_C = 'c',
    TT_DOOR_D = 'd',
    TT_DOOR_E = 'e',
    TT_WALL = '#',
    TT_N_TYPES = 'N',
    TT_FLOOR = '.',
    TT_HALLWAY = ',',
    TT_CLOSED_DOOR = '+',
    TT_OPEN_DOOR = '-'
};

inline bool IsDoor(int tile) {
    return tile >= TT_DOOR_A && tile <= TT_DOOR_E;
}

inline bool IsKey(int tile) {
    return tile >= TT_KEY_A && tile <= TT_KEY_E;
}

inline int DoorIndex(int tile) {
    assert(IsDoor(tile));
    return tile - TT_DOOR_A;
}

inline int KeyIndex(int tile) {
    assert(IsKey(tile));
    return tile - TT_KEY_A;
}

class Map {
public:
    int TileAtPoint(int x, int y) const;
    int GetWidth() const;
    int GetHeight() const;

    static Map* ParseMapFile();
    Character* NewPlayerAtStart(int model_id) const;

    void RenderMap();
    void RenderCharacter();

    inline bool InMap(uint32_t ix, uint32_t iy) const {
        return (ix < w)
            && (iy < h);
    }

    inline bool Traversable(uint32_t ix, uint32_t iy) const {
        int t = TileAtPoint(ix, iy);
        return t == TT_START || t == TT_GOAL || t == TT_KEY_A || t ==TT_KEY_B ||
				t == TT_KEY_C || t == TT_KEY_D || t ==  TT_KEY_E || t == TT_FLOOR;
            //(tile <= TT_DOOR_E && KeyCollected(tile));
    }

    inline bool ValidMove(int x, int y) const {
        return InMap(x, y) && Traversable(x, y);
    }

    inline float PctComplete() const {
        if (total_key_count > 0) {
            return (float) key_collected_count / total_key_count;
        } else {
            return 1.0f;
        }
    }

    bool KeyCollected(int key) const;
    void CollectKey(int key);
    
    void print_map();

private:
    Map(int w, int h);
    void SetTile(int x, int y, int type);

    std::vector<bool> keys_collected;
    int key_collected_count, total_key_count;

    uint32_t w, h, size;
    int startx, starty;
    std::vector<int> tiles;
};

#endif
