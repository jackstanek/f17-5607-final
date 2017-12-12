#ifndef GAME_HPP_
#define GAME_HPP_

#include <SDL2/SDL.h>
#include "glad/glad.h"

#include "character.hpp"
#include "map.hpp"
#include "model.hpp"

class Game {
public:
    Game(const char* path);
    ~Game();

    void Render();

    void OnKeyDown(const SDL_KeyboardEvent& ev);

    inline bool GameWon() const {
        return map->TileAtPoint(player->GetX(),
                                player->GetY()) == TT_GOAL;
    }

    void ChangeMap(const char* path);

private:
    void RenderMap();
    void RenderCharacter();

    SDL_Window* window;
    SDL_GLContext context;

    GLuint vao;
    GLuint vbo[1];
    GLint uniView, uniProj;
    GLuint tex0, tex1;
    GLint phongShader, texturedShader;

    int wall_id, key_id, char_id, floor_id, goal_id;
    Character* player;

    int time;

    Map* map;
    ModelPool* mp;
};

#endif
