#include <cstdio>
#include <SDL2/SDL.h>
#include <vector>

#include "game.hpp"

int main(int argc, char ** argv)
{
    std::vector<const char*> maps = {"test-secondmap","test_map.txt"};
    unsigned int currentMapIterator = 0;
    Game* g;
    if (argc == 1) {
		g = new Game(1);
	}
	else {
		g = new Game(atoi(argv[1]));
	}
    
    //bool running = true;
    bool quit = false;

    while (!quit) {
        if (currentMapIterator != 0 && g->GameWon()) g->ChangeMap();
        SDL_Event event;
        while (SDL_PollEvent(&event)) {  //inspect all events in the queue
            if (event.type == SDL_QUIT) {
                //running = false;
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    //running = false;
                    quit = true;
                }
                g->OnKeyDown(event.key);
            }
        }
        g->Update();
        g->Render();
        if (g->GameWon()) currentMapIterator++;
    }
    delete g;
}
