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
    prev_fx(x),
    prev_fy(y + 1),
    front_x(x),
    front_y(y + 1),
    direction(CD_UP),
    rot(-3.1415/2),
    anim_start(0)
{
    SetFront();
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
    return glm::vec3(blend(tc, prev_x, x),
                     blend(tc, prev_y, y),
                     0.0f);
}

glm::vec3 Character::WorldPosition(int t) const
{
    int tc = t - anim_start;
    return glm::vec3(blend(tc, prev_x, x) + (front_x - x),
                     blend(tc, prev_y, y) + (front_y - y),
                     0.0f);
}

glm::vec3 Character::LookAtPosition(int t) const
{
    int tc = t - anim_start;
    return glm::vec3(blend(tc, prev_fx, front_x),
                     blend(tc, prev_fy, front_y),
                     0.0f);
}

float Character::Rotation() const
{
    return rot;
}

bool Character::MoveInDirection(int sym, Map* map, int time)
{
    /* Don't move if we are in an animation */
    if (time - anim_start < ANIM_SPEED) {
        return false;
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
			break;
    }

    rot = (direction + 1) * (-3.1415/2);
    anim_start = time;
    prev_fx = front_x;
    prev_fy = front_y;
    SetFront();
    return true;
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

void Character::SetFront()
{
    switch (direction) {
    case CD_UP:
        front_x = x;
        front_y = y + 1;
        break;
    case CD_DOWN:
        front_x = x;
        front_y = y - 1;
        break;
    case CD_RIGHT:
        front_x = x + 1;
        front_y = y;
        break;
    case CD_LEFT:
        front_x = x - 1;
        front_y = y;
        break;
    }

}
