#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#define WIDTH 1024
#define HEIGHT 600

int main(int argc, char **argv) {
	SDL_Surface			*screen = NULL;			// Whole screen
	SDL_Event			event;

	bool				quit = false;

	SDL_Init(SDL_INIT_EVERYTHING);

	screen = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_SWSURFACE);
	SDL_WM_SetCaption("Langton's Ant", NULL);

	while (!quit) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				quit = true;
			}
		}
		SDL_Delay(1);	// Don't hog the CPU too much
	}

	SDL_Quit();
}
