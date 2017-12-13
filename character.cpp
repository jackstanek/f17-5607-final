#include <cmath>

#include <SDL2/SDL.h>

#include "glm/vec3.hpp"

#include "character.hpp"
#include "game.hpp"
#include "map.hpp"

Character::Character(int x, int y, int model_id) :
    model_id(model_id),
    x(x),
    y(y),
    prev_x(x),
    prev_y(y),
    prev_bx(x),
    prev_by(y - 1),
    behind_x(x),
    behind_y(y - 1),
    direction(CD_UP),
    anim_start(0)
{
    SetBehind();
}

float blend(int t, float start, float end) {
    float tf = (float) t / ANIM_SPEED;
    float dv = end - start;

    if (tf > 1) {
        tf = 1;
    } else if (t < 0) {
        tf = 0;
    }

    return start + std::sin(tf * M_PI / 2) * dv;
}

glm::vec3 Character::CamPosition(int t) const
{
    int tc = t - anim_start;
    return glm::vec3(blend(tc, prev_bx, behind_x),
                     blend(tc, prev_by, behind_y),
                     1.0f);
}

glm::vec3 Character::WorldPosition(int t) const
{
    int tc = t - anim_start;
    return glm::vec3(blend(tc, prev_x, x),
                     blend(tc, prev_y, y),
                     0.0f);
}

glm::vec3 Character::LookAtPosition(int t) const
{
    return WorldPosition(t) + glm::vec3(0, 0, 0.33f);
}

void Character::MoveInDirection(int sym, Map* map, int time)
{
    /* Don't move if we are in an animation */
    if (time - anim_start < ANIM_SPEED) {
        return;
    }

    prev_x = x;
    prev_y = y;
    switch (sym) {
    case SDLK_a:
        direction = (direction + 3) % 4;
        break;
    case SDLK_d:
        direction = (direction + 1) % 4;
        break;
    case SDLK_s:
        direction = (direction + 2) % 4;
        break;
    case SDLK_w:
        Move(map);
    }

    anim_start = time;
    prev_bx = behind_x;
    prev_by = behind_y;
    SetBehind();
}

void Character::Move(Map* map)
{
    prev_x = x;
    prev_y = y;

    switch (direction) {
    case CD_UP:
        if (map->ValidMove(x, y + 1)) {
            y += 1;
        }
        break;
    case CD_DOWN:
        if (map->ValidMove(x, y - 1)) {
            y -= 1;
        }
        break;
    case CD_LEFT:
        if (map->ValidMove(x - 1, y)) {
            x -= 1;
        }
        break;
    case CD_RIGHT:
        if (map->ValidMove(x + 1, y)) {
            x += 1;
        }
        break;
    }

    /* Do behavior based on new space */
    int tile = map->TileAtPoint(x, y);
    if (IsKey(tile) && !map->KeyCollected(tile)) {
        map->CollectKey(map->TileAtPoint(x, y));
    }
}

void Character::SetBehind()
{
    switch (direction) {
    case CD_UP:
        behind_x = x;
        behind_y = y - 1;
        break;
    case CD_DOWN:
        behind_x = x;
        behind_y = y + 1;
        break;
    case CD_RIGHT:
        behind_x = x - 1;
        behind_y = y;
        break;
    case CD_LEFT:
        behind_x = x + 1;
        behind_y = y;
        break;
    }
}
