/*
 *
 */


#include <stdio.h>
#include <SDL2/SDL_image.h>


/* screen dimension constants */
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

/* testure wrapper class */
class LTexture
{
public:
	/* initializes variables */
	LTexture();

	/* dealloates memory */
	~LTexture();

	/* loads image at specified path */
	int loadFromFile(const char *path);

	/* deallocates texture */
	void free(void);

	/* renders texture at given point */
	void render(int x, int y, SDL_Rect *clip = NULL);

	/* gets image dimensions */
	int getWidth(void);
	int getHeight(void);

private:
	/* the actual hardware texture */
	SDL_Texture *mTexture;

	/* image dimensions */
	int mWidth;
	int mHeight;
};





/* the window we'll be rendering to */
SDL_Window *gWindow = NULL;

/* the window renderer */
SDL_Renderer *gRenderer = NULL;

/* scene sprites */
SDL_Rect gSpriteClips[4];
LTexture gSpriteSheetTexture;


/* starts uo SDL and creates window */
int init_sdl(void);

/* loads medis */
int load_media(void);

/* frees media and shuts down SDL */
void close_sdl(void);






LTexture::LTexture()
{
	/* initialize */
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	printf("LTexture::~LTexture()\n");
	/* deallocate */
	free();
}

int LTexture::loadFromFile(const char *path)
{
	/* get rid of preexisting trexture */
	free();

	/* the final texture */
	SDL_Texture *newTexture = NULL;

	/* load image at specified path */
	SDL_Surface *loadedSurface = IMG_Load(path);
	if (loadedSurface == NULL)
		fprintf(stderr, "Unable to load image %s! "
			"SDL_image Error: %s\n", path, IMG_GetError());
	else {
		/* color key image */
		SDL_SetColorKey(loadedSurface, SDL_TRUE,
			SDL_MapRGB(loadedSurface->format, 0xFF, 0x00, 0x00));

		/* create texture from surface pixels */
		newTexture = SDL_CreateTextureFromSurface(
				gRenderer, loadedSurface);

		if (newTexture == NULL)
			fprintf(stderr, "Unable to create texture form %s! "
				"SDL Error: %s\n", path, SDL_GetError());
		else {
			/* get image dimensions */
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		/* get rid of old loaded surface */
		SDL_FreeSurface(loadedSurface);
	}

	/* return success */
	mTexture = newTexture;
	return (mTexture == NULL ? -1 : 0);
}


void LTexture::free(void)
{
	printf("LTexture::free()\n");
	/* free texture if it exists */
	if (mTexture != NULL) {
		printf("free() ~ width = %d, height = %d\n", mWidth, mHeight);
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;

		mWidth = 0;
		mHeight = 0;
	}
}


void LTexture::render(int x, int y, SDL_Rect *clip)
{
	/* set rendering space and render to screen */
	SDL_Rect renderQuad = {x, y, mWidth, mHeight};

	/* set clip rendering dimensions */
	if (clip != NULL) {
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	/* render to screen */
	SDL_RenderCopy(gRenderer, mTexture, clip, &renderQuad);
}

int LTexture::getWidth(void)
{
	return mWidth;
}

int LTexture::getHeight(void)
{
	return mHeight;
}






int
main(int argc, char *argv[])
{
	int quit = 0;
	SDL_Event event;

	/* starts up SDL and create window */
	if (init_sdl() == -1) {
		fprintf(stderr, "Failed to initialize SDL!\n");
		return -1;
	}

	/* load media */
	if (load_media() == -1) {
		fprintf(stderr, "Failed to load media!\n");
		return -2;
	}

	/* main loop */
	while (quit == 0) {
		/* handle events on queue */
		while (SDL_PollEvent(&event) != 0) {
			/* users request quit */
			if (event.type == SDL_QUIT)
				quit = 1;
		}

		/* clear screen */
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(gRenderer);

		/* render top left sprite*/
		gSpriteSheetTexture.render(0, 0, &gSpriteClips[0]);

		/* render top right sprite */
		gSpriteSheetTexture.render(
			SCREEN_WIDTH - gSpriteClips[1].w, 0, &gSpriteClips[1]);

		/* render bottom left sprite */
		gSpriteSheetTexture.render(
			0, SCREEN_HEIGHT - gSpriteClips[2].h, &gSpriteClips[2]);

		/* render bottom right sprite */
		gSpriteSheetTexture.render(
			SCREEN_WIDTH - gSpriteClips[3].w,
			SCREEN_HEIGHT - gSpriteClips[3].h,
			&gSpriteClips[3]);

		

		/* update screen */
		SDL_RenderPresent(gRenderer);
	}

	/* free resources and close */
	close_sdl();

	return 0;
}


int
init_sdl(void)
{
	int flags;

	/* initialize SDL */
	if (SDL_Init(SDL_INIT_VIDEO) == -1) {
		fprintf(stderr, "SDL could not initialize! SDL Error: %s\n",
			SDL_GetError());
		return -1;
	}

	/* set texture filtering to linear */
	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		fprintf(stderr,
			"Warning: Linear texture filtering not enabled!\n");

	/* create window */
	gWindow = SDL_CreateWindow("SDL Tutorial",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (gWindow == NULL) {
		fprintf(stderr, "window could not be created! SDL Error: %s\n",
			SDL_GetError());
		return -1;
	}

	/* create renderer for window */
	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
	if (gRenderer == NULL) {
		fprintf(stderr, "Renderer could not be created! "
			"SDL Error: %s\n", SDL_GetError());
		return -1;
	} else {
		/* initialize renderer color */
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

		/* initialize PNG loading */
		flags = IMG_INIT_PNG;
		if ((IMG_Init(flags) & flags) == 0) {
			fprintf(stderr, "SDL_image could not initialize! "
				"SDL_image Error: %s\n", IMG_GetError());
			return -1;
		}
	}

	return 0;
}

int
load_media(void)
{
	int success = 0;

	/* load sprite sheet texture */
	if (gSpriteSheetTexture.loadFromFile("dots_1.png") == -1) {
		fprintf(stderr, "Failed to load sprite sheet texture!\n");
		success = -1;
	} else {
		/* set top left sprite */
		gSpriteClips[0].x = 0;
		gSpriteClips[0].y = 0;
		gSpriteClips[0].w = 100;
		gSpriteClips[0].h = 100;

		/* set top right sprite */
		gSpriteClips[1].x = 100;
		gSpriteClips[1].y = 0;
		gSpriteClips[1].w = 100;
		gSpriteClips[1].h = 100;

		/* set bottom left sprite */
		gSpriteClips[2].x = 0;
		gSpriteClips[2].y = 100;
		gSpriteClips[2].w = 100;
		gSpriteClips[2].h = 100;

		/* set bottom right sprite */
		gSpriteClips[3].x = 100;
		gSpriteClips[3].y = 100;
		gSpriteClips[3].w = 100;
		gSpriteClips[3].h = 100;
	}
	

	return success;
}


void
close_sdl(void)
{
	printf("close_sdl()\n");

	/* free loaded image */
	gSpriteSheetTexture.free();

	/* destroy window */
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	/* Quit SDL subsystem */
	IMG_Quit();
	SDL_Quit();
}





