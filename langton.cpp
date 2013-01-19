#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#include <iostream>

#define WIDTH 1024
#define HEIGHT 600

void	putpixel(SDL_Surface *surface, int x, int y, int r, int g, int b);
void	field2surface(char *field, SDL_Surface *surface);

int main(int argc, char **argv) {
	SDL_Surface			*screen = NULL;			// Whole screen
	SDL_Event			event;

	bool				quit = false;
	char				field[WIDTH*HEIGHT];	// the playing field
	int					antx, anty;				// ant position
	char				antdir;					// ant direction

	SDL_Init(SDL_INIT_EVERYTHING);

	screen = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_SWSURFACE);
	SDL_WM_SetCaption("Langton's Ant", NULL);

	// initial ant position
	antx = WIDTH/2;
	anty = HEIGHT/2;
	antdir = 3;

	// init field
	for (int y = 0; y < HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {
			field[y*WIDTH+x] = 0;
		}
	}

	while (!quit) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				quit = true;
			}
		}
		
		// move the ant
		char	newfieldval;
		switch (field[anty*WIDTH+antx]) {
			case 0:
				newfieldval = 1;
				if (++antdir > 3)
					antdir = 0;
				break;
			case 1:
				newfieldval = 0;
				if (--antdir < 0)
					antdir = 3;
				break;
		}
		field[anty*WIDTH+antx] = newfieldval;

		switch (antdir) {
			case 0:
				if (anty == 0) {
					anty = HEIGHT-1;
				} else {
					anty--;
				}
				break;

			case 1:
				if (antx >= WIDTH-1) {
					antx = 0;
				} else {
					antx++;
				}
				break;

			case 2:
				if (anty >= HEIGHT-1) {
					anty = 0;
				} else {
					anty++;
				}
				break;

			case 3:
				if (antx == 0) {
					antx = WIDTH-1;
				} else {
					antx--;
				}
				break;
		}

		// display field
		field2surface(field, screen);
		// display ant
		putpixel(screen, antx, anty, 255, 0, 0);
		SDL_Flip(screen);

		SDL_Delay(1);	// Don't hog the CPU too much
	}

	SDL_Quit();
}

void	putpixel(SDL_Surface *surface, int x, int y, int r, int g, int b) {
	Uint32 pixel = SDL_MapRGB(surface->format, r, g, b);
	Uint32 *pixels = (Uint32 *)surface->pixels;

	SDL_LockSurface(surface);
	pixels[(y*surface->w)+x] = pixel;
	SDL_UnlockSurface(surface);
}

void	field2surface(char *field, SDL_Surface *surface) {
	int		x,y;

	for (y = 0; y < HEIGHT; y++) {
		for (x = 0; x < WIDTH; x++) {
			switch(field[y*WIDTH+x]) {
				case 1:
					putpixel(surface, x, y, 100, 100, 100);
					break;
				default:
					putpixel(surface, x, y, 0, 0, 0);
			}
		}
	}
}
