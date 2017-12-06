#ifndef MAP_HPP_
#define MAP_HPP_

#include <cassert>
#include <vector>
#include <stdint.h>

#include "glad/glad.h"

#include "character.hpp"

enum TileType {
    TT_OPEN = 0,
    TT_START,
    TT_GOAL,
    TT_KEY_A,
    TT_KEY_B,
    TT_KEY_C,
    TT_KEY_D,
    TT_KEY_E,
    TT_DOOR_A,
    TT_DOOR_B,
    TT_DOOR_C,
    TT_DOOR_D,
    TT_DOOR_E,
    TT_WALL,
    TT_N_TYPES
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

    static Map* ParseMapFile(const char* path);
    Character* NewPlayerAtStart(int model_id) const;

    void RenderMap();
    void RenderCharacter();

    inline bool InMap(uint32_t ix, uint32_t iy) const {
        return (ix < w)
            && (iy < h);
    }

    inline bool Traversable(uint32_t ix, uint32_t iy) const {
        int tile = TileAtPoint(ix, iy);
        return tile <= TT_KEY_E ||
            (tile <= TT_DOOR_E && KeyCollected(tile));
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

private:
    Map(int w, int h);
    void SetTile(int x, int y, int type);

    std::vector<bool> keys_collected;
    int key_collected_count, total_key_count;

    uint32_t w, h, size;
    int startx, starty;
    int* tiles;
};

#endif
