#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#include <iostream>

#define WIDTH 1024
#define HEIGHT 600

// field size
#define FWIDTH 512
#define FHEIGHT 300

void	putpixel(SDL_Surface *surface, int x, int y, int r, int g, int b);
void	field2surface(char *field, SDL_Surface *surface);
void	heatmap2surface(unsigned int *heatmap, SDL_Surface *surface);
void	updatescreen(SDL_Surface *screen, SDL_Surface *field, SDL_Surface *heatmap);

int main(int argc, char **argv) {
	SDL_Surface			*screen = NULL;			// Whole screen
	SDL_Surface			*fieldSurface = NULL;
	SDL_Surface			*heatmapSurface = NULL;
	SDL_Event			event;

	bool				quit = false;
	char				field[FWIDTH*FHEIGHT];	// the playing field
	unsigned int		heatmap[FWIDTH*FHEIGHT];
	int					antx, anty;				// ant position
	char				antdir;					// ant direction

	SDL_Init(SDL_INIT_EVERYTHING);

	screen = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_SWSURFACE);
	SDL_WM_SetCaption("Langton's Ant", NULL);

	fieldSurface = SDL_CreateRGBSurface(SDL_HWSURFACE, FWIDTH, FHEIGHT, 32, 0, 0, 0, 0);
	heatmapSurface = SDL_CreateRGBSurface(SDL_HWSURFACE, FWIDTH, FHEIGHT, 32, 0, 0, 0, 0);

	// initial ant position
	antx = FWIDTH/2;
	anty = FHEIGHT/2;
	antdir = 3;

	// init field and heatmap
	for (int y = 0; y < FHEIGHT; y++) {
		for (int x = 0; x < FWIDTH; x++) {
			field[y*FWIDTH+x] = 0;
			heatmap[y*FWIDTH+x] = 0;
		}
	}

	// draw border
	for (int x = 0; x < WIDTH; x++) {
		putpixel(screen, x, FHEIGHT, 255, 255, 255);
	}
	for (int y = 0; y < HEIGHT; y++) {
		putpixel(screen, FWIDTH, y, 255, 255, 255);
	}

	while (!quit) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				quit = true;
			}
		}
		
		// move the ant
		heatmap[anty*FWIDTH+antx]++;
		char	newfieldval;
		switch (field[anty*FWIDTH+antx]) {
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
		field[anty*FWIDTH+antx] = newfieldval;

		switch (antdir) {
			case 0:
				if (anty == 0) {
					anty = FHEIGHT-1;
				} else {
					anty--;
				}
				break;

			case 1:
				if (antx >= FWIDTH-1) {
					antx = 0;
				} else {
					antx++;
				}
				break;

			case 2:
				if (anty >= FHEIGHT-1) {
					anty = 0;
				} else {
					anty++;
				}
				break;

			case 3:
				if (antx == 0) {
					antx = FWIDTH-1;
				} else {
					antx--;
				}
				break;
		}

		// display field
		field2surface(field, fieldSurface);
		heatmap2surface(heatmap, heatmapSurface);

		// display ant
		putpixel(fieldSurface, antx, anty, 255, 0, 0);
		updatescreen(screen, fieldSurface, heatmapSurface);

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

	for (y = 0; y < FHEIGHT; y++) {
		for (x = 0; x < FWIDTH; x++) {
			switch(field[y*FWIDTH+x]) {
				case 1:
					putpixel(surface, x, y, 100, 100, 100);
					break;
				default:
					putpixel(surface, x, y, 0, 0, 0);
			}
		}
	}
}

void	heatmap2surface(unsigned int *heatmap, SDL_Surface *surface) {
	int		x,y;
	unsigned int 	r = 0;

	for (y = 0; y < FHEIGHT; y++) {
		for (x = 0; x < FWIDTH; x++) {
			r = heatmap[y*FWIDTH+x] * 10;
			if (r > 255) {
				r = 255;
			}
			putpixel(surface, x, y, r, 0, 0);
		}
	}
}

void	updatescreen(SDL_Surface *screen, SDL_Surface *field, SDL_Surface *heatmap) {
	SDL_Rect	winpos;

	// field
	winpos.x = 0;
	winpos.y = 0;
	SDL_BlitSurface(field, NULL, screen, &winpos);

	// heat map
	winpos.x = FWIDTH+2;
	winpos.y = 0;
	SDL_BlitSurface(heatmap, NULL, screen, &winpos);

	SDL_Flip(screen);
}
