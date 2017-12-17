#ifndef CHARACTER_HPP_
#define CHARACTER_HPP_

#include <SDL2/SDL.h>
#include <vector>
#include "glm/vec3.hpp"

/* Time, in milliseconds, to perform an animation. */
#define ANIM_SPEED 150

class Map;

enum CharacterDirection {
    CD_UP = 0,
    CD_RIGHT,
    CD_DOWN,
    CD_LEFT,
};

class Character {
public:
    Character(int x, int y, int model_id);

    glm::vec3 WorldPosition(int t) const;
    glm::vec3 CamPosition(int t) const;
    glm::vec3 LookAtPosition(int t) const;

    bool MoveInDirection(int cm, Map* map, int time);

    int GetX() const { return x; }
    int GetY() const { return y; }

    const int model_id;

private:
    void Move(Map* map);
    void SetBehind();

    int x, y;
    int prev_x, prev_y;
    int prev_bx, prev_by;
    int behind_x, behind_y;
    int direction;

    int anim_start;
};

#endif
