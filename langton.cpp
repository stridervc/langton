#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#include <iostream>

#define WIDTH 1024
#define HEIGHT 600
#define FWIDTH 512
#define FHEIGHT 300
#define ZOOM 4

void	putpixel(SDL_Surface *surface, int x, int y, int r, int g, int b);
void	field2surface(char *field, SDL_Surface *surface);
void	heatmap2surface(unsigned int *heatmap, SDL_Surface *surface);
void	updatescreen(SDL_Surface *screen, SDL_Surface *field, SDL_Surface *zoom, SDL_Surface *heatmap);
void	updateZoom(int antx, int anty, char *field, SDL_Surface *zoomSurface, int &zoomx, int &zoomy);

int main(int argc, char **argv) {
	SDL_Surface			*screen = NULL;			// Whole screen
	SDL_Surface			*fieldSurface = NULL;
	SDL_Surface			*heatmapSurface = NULL;
	SDL_Surface			*zoomSurface = NULL;
	SDL_Event			event;

	bool				quit = false;
	char				field[FWIDTH*FHEIGHT];	// the playing field
	unsigned int		heatmap[FWIDTH*FHEIGHT];
	int					antx, anty;				// ant position
	int					zoomx, zoomy;			// current zoom start x,y
	char				antdir;					// ant direction

	SDL_Init(SDL_INIT_EVERYTHING);

	screen = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_SWSURFACE);
	SDL_WM_SetCaption("Langton's Ant", NULL);

	fieldSurface = SDL_CreateRGBSurface(SDL_HWSURFACE, FWIDTH, FHEIGHT, 32, 0, 0, 0, 0);
	heatmapSurface = SDL_CreateRGBSurface(SDL_HWSURFACE, FWIDTH, FHEIGHT, 32, 0, 0, 0, 0);
	zoomSurface = SDL_CreateRGBSurface(SDL_HWSURFACE, FWIDTH, FHEIGHT, 32, 0, 0, 0, 0);

	// init arrays
	for (int y = 0; y < FHEIGHT; y++) {
		for (int x = 0; x < FWIDTH; x++) {
			field[y*FWIDTH+x] = 0;
			heatmap[y*FWIDTH+x] = 0;
		}
	}

	// initial ant position
	antx = FWIDTH/2;
	anty = FHEIGHT/2;
	antdir = 3;
	zoomx = 0;
	zoomy = 0;

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

		// update various surfaces
		field2surface(field, fieldSurface);
		heatmap2surface(heatmap, heatmapSurface);
		updateZoom(antx, anty, field, zoomSurface, zoomx, zoomy);

		// display ant
		putpixel(fieldSurface, antx, anty, 255, 0, 0);
		updatescreen(screen, fieldSurface, zoomSurface, heatmapSurface);

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
			r = heatmap[y*FWIDTH+x] * 5;
			if (r > 255) {
				r = 255;
			}
			putpixel(surface, x, y, r, 0, 0);
		}
	}
}

void	updatescreen(SDL_Surface *screen, SDL_Surface *field, SDL_Surface *zoom, SDL_Surface *heatmap) {
	SDL_Rect	winpos;

	// field
	winpos.x = 0;
	winpos.y = 0;
	SDL_BlitSurface(field, NULL, screen, &winpos);

	// zoom
	winpos.x = FWIDTH+2;
	winpos.y = 0;
	SDL_BlitSurface(zoom, NULL, screen, &winpos);

	// heat map
	winpos.x = 0;
	winpos.y = FHEIGHT+2;
	SDL_BlitSurface(heatmap, NULL, screen, &winpos);

	SDL_Flip(screen);
}

void	updateZoom(int antx, int anty, char *field, SDL_Surface *zoomSurface, int &zoomx, int &zoomy) {
	int		x,y;
	int		fx, fy;
	char	fvalue;
	int		nx, ny;

	// blank the surface
	for (y = 0; y < FHEIGHT; y++) {
		for (x = 0; x < FWIDTH; x++) {
			putpixel(zoomSurface, x, y, 0, 0, 0);
		}
	}

	// should we recalculate our zoom x,y start position?
	if ((antx < zoomx) || (anty < zoomy) || (antx > zoomx + FWIDTH/ZOOM) || (anty > zoomy + FHEIGHT/ZOOM)) {
		zoomx = antx - FWIDTH/2/ZOOM;
		zoomy = anty - FHEIGHT/2/ZOOM;

		if (zoomx < 0)
			zoomx = 0;
		if (zoomy < 0)
			zoomy = 0;
		if (zoomx > FWIDTH - FWIDTH/2/ZOOM)
			zoomx = FWIDTH - FWIDTH/2/ZOOM;
		if (zoomy > FHEIGHT - FHEIGHT/2/ZOOM)
			zoomy = FHEIGHT - FHEIGHT/2/ZOOM;
	}

	// draw zoomed in field
	y = 0;
	for (fy = zoomy; fy < zoomy + FHEIGHT/ZOOM; fy++) {
		x = 0;
		for (fx = zoomx; fx < zoomx + FWIDTH/ZOOM; fx++) {
			fvalue = field[fy*FWIDTH+fx];
			for (ny = 0; ny < ZOOM; ny++) {
				for (nx = 0; nx < ZOOM; nx++) {
					putpixel(zoomSurface, x*ZOOM+nx, y*ZOOM+ny, fvalue*100, fvalue*100, fvalue*100);
				}
			}
			x++;
		}
		y++;
	}

	// draw ant
	int nantx = antx - zoomx;
	int nanty = anty - zoomy;
	for (y = 0; y < ZOOM; y++) {
		for (x = 0; x < ZOOM; x++) {
			putpixel(zoomSurface, nantx*ZOOM+x, nanty*ZOOM+y, 255, 0, 0);
		}
	}
}
