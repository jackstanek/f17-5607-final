#ifndef GAME_HPP_
#define GAME_HPP_

#include <SDL2/SDL.h>
#include <vector>
#include <queue>
#include "glad/glad.h"

#include "framebuffer.hpp"
#include "character.hpp"
#include "map.hpp"
#include "model.hpp"

enum VBOs {
    VBO_MODELS = 0,
    VBO_QUAD,
    N_VBOS
};

class Game {
public:
    Game(int);
    ~Game();

    void Render();

    void OnKeyDown(const SDL_KeyboardEvent& ev);

    inline bool GameWon() const {
        return map->TileAtPoint(player->GetX(),
                                player->GetY()) == TT_GOAL;
    }

    void ChangeMap();
    void Update();

private:
    void RenderMap();
    void RenderCharacter();

    SDL_Window* window;
    SDL_GLContext context;

    GLuint model_vao, quad_vao;
    GLuint vbo[N_VBOS];
    GLint uniView, uniProj;
    GLuint tex0, tex1;
    GLint texturedShader, quadShader;

    int wall_id, key_id, char_id, floor_id, goal_id;
    Character* player;

    int time;
	int difficulty;
    Map* map;
    ModelPool* mp;

    std::vector<RenderPass*> render_passes;
    
    int nextAction = -1;
    int nextTime; // on Dragonball Z
};

#endif
