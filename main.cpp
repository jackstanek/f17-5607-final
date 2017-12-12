#include <cstdio>
#include <SDL2/SDL.h>

#include "game.hpp"

int main()
{
    Game* g = new Game("test-secondmap");
    bool running = true;
    bool quit = false;

    while (running && !quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {  //inspect all events in the queue
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                    quit = true;
                }
                g->OnKeyDown(event.key);
            }
        }
        g->Render();

        if (running) {
            running = !g->GameWon();
        }
    }

    g->ChangeMap("test_map.txt");

    running = true;
    while (running && !quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {  //inspect all events in the queue
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                }
                g->OnKeyDown(event.key);
            }
        }
        g->Render();

        if (running) {
            running = !g->GameWon();
        }
    }

    delete g;
}
