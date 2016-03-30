/*
 *
 */

#include <stdio.h>
#include <SDL2/SDL.h>
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

#ifdef _SDL_TTF_H
	/* created image from font string */
	int loadFromRenderedText(const char *text, SDL_Color textColor);
#endif
	/* deallocates texture */
	void free(void);

	/* set color modulation */
	void setColor(Uint8 red, Uint8 green, Uint8 blue);

	/* set blending */
	void setBlendMode(SDL_BlendMode blending);

	/* set alpha modulation */
	void setAlpha(Uint8 alpha);

	/* renders texture at given point */
	void render(int x, int y, SDL_Rect *clip = NULL, double angle = 0.0,
		SDL_Point *center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

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


/* the application time based timer */
class LTimer
{
public:
	/* initialize variable */
	LTimer();

	/* the various clock actions */
	void start();
	void stop();
	void pause();
	void unpause();

	/* get the timer's time */
	Uint32 getTicks();

	/* check the status of timer */
	int isStarted();
	int isPaused();

private:
	/* the clock time when the timer started */
	Uint32 mStartTicks;

	/* the ticks stored when the timer was paused */
	Uint32 mPausedTicks;

	/* the timer status */
	int mPaused;
	int mStarted;
};


/* the dot that will move around on the screen */
class Dot
{
public:
	/* the dimensions of the dot */
	static const int DOT_WIDTH = 20;
	static const int DOT_HEIGHT = 20;

	/* maximum axis velocity of the dot */
	static const int DOT_VEL = 10;

	/* initializes the variables */
	Dot();

	/* takes key presses and adjusts the dot's velocity */
	void handleEvent(SDL_Event &event);

	/* moves the dot */
	void move();

	/* shows the dot on the screen */
	void render();

private:
	/* the X and Y offsets of the dot */
	int mPosX, mPosY;

	/* the velocity of the dot */
	int mVelX, mVelY;
};



/* the window we'll be rendering to */
SDL_Window *gWindow = NULL;

/* the window renderer */
SDL_Renderer *gRenderer = NULL;

/* scence textures */
LTexture gDotTexture;





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

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue)
{
	/* modulate texture */
	SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::setBlendMode(SDL_BlendMode blending)
{
	/* set blending function */
	SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha)
{
	/* modulate texture alpha */
	SDL_SetTextureAlphaMod(mTexture, alpha);
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
			SDL_MapRGB(loadedSurface->format, 0x00, 0xFF, 0xFF));

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

#ifdef _SDL_TTF_H
int LTexture::loadFromRenderedText(const char *text, SDL_Color textColor)
{
	/* get rid of preexisting trexture */
	free();

	/* render text surface */
	SDL_Surface *textSurface =
		TTF_RenderText_Solid(gFont, text, textColor);

	if (textSurface == NULL)
		fprintf(stderr, "Unable to render text surface! "
			"SDL_ttf Error: %s\n", TTF_GetError());
	else {
		/* create texture from surface pixels */
		mTexture = SDL_CreateTextureFromSurface(
				gRenderer, textSurface);

		if (mTexture == NULL)
			fprintf(stderr,
				"Unable to create texture form rendered text! "
				"SDL Error: %s\n", SDL_GetError());
		else {
			/* get image dimensions */
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		/* get rid of old loaded surface */
		SDL_FreeSurface(textSurface);
	}

	/* return success */
	return (mTexture == NULL ? -1 : 0);
}
#endif


void LTexture::free(void)
{
	//printf("LTexture::free()\n");
	/* free texture if it exists */
	if (mTexture != NULL) {
		//printf("free() ~ width = %d, height = %d\n", mWidth, mHeight);
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;

		mWidth = 0;
		mHeight = 0;
	}
}


void LTexture::render(int x, int y, SDL_Rect *clip, double angle,
	SDL_Point *center, SDL_RendererFlip flip)
{
	/* set rendering space and render to screen */
	SDL_Rect renderQuad = {x, y, mWidth, mHeight};

	/* set clip rendering dimensions */
	if (clip != NULL) {
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	/* render to screen */
	SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad,
		angle, center, flip);
}

int LTexture::getWidth(void)
{
	return mWidth;
}

int LTexture::getHeight(void)
{
	return mHeight;
}




LTimer::LTimer()
{
	/* initialize the variables */
	mStartTicks = 0;
	mPausedTicks = 0;

	mPaused = false;
	mStarted = false;
}

void LTimer::start()
{
	/* start the timer */
	mStarted = 1;

	/* unpause the timer */
	mPaused = 0;

	/* get the current clock time */
	mStartTicks = SDL_GetTicks();
	mPausedTicks = 0;
}

void LTimer::stop()
{
	/* stop the timer */
	mStarted = 0;

	/* unpause the timer */
	mPaused = 0;

	/* clear tick variables */
	mStartTicks = 0;
	mPausedTicks = 0;
}

void LTimer::pause()
{
	/* if the timer is running and isnt already paused */
	if (mStarted && !mPaused) {
		/* pause the timer */
		mPaused = 1;

		/* calculate the paused ticks */
		mPausedTicks = SDL_GetTicks() - mStartTicks;
		mStartTicks = 0;
	}
}

void LTimer::unpause()
{
	/* if the timer is running and paused */
	if (mStarted && mPaused) {
		/* unpause the timer */
		mPaused = 0;

		/* reset the starting ticks */
		mStartTicks = SDL_GetTicks() - mPausedTicks;

		/* reset the paused ticks */
		mPausedTicks = 0;
	}
}

Uint32 LTimer::getTicks()
{
	/* the actual timer time */
	Uint32 time = 0;

	/* if the timer is running */
	if (mStarted) {
		/* if the timer is paused */
		if (mPaused)
			/* return the number of ticks when the timer was paused */
			time = mPausedTicks;
		else
			/* return the current time minus the start time */
			time = SDL_GetTicks() - mStartTicks;
	}
	return time;
}

int LTimer::isStarted()
{
	return mStarted;
}

int LTimer::isPaused()
{
	return (mPaused && mStarted);
}


Dot::Dot()
{
	/* initialize the offsets */
	mPosX = 0;
	mPosY = 0;

	/* initialize the velocity */
	mVelX = 0;
	mVelY = 0;
}


void Dot::handleEvent(SDL_Event &event)
{
	/* if a key was pressed */
	if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {
		printf("\tkey pressed\n");

		/* adjust the velocity */
		switch (event.key.keysym.sym) {
		case SDLK_UP:
			mVelY -= DOT_VEL;
			break;
		case SDLK_DOWN:
			mVelY += DOT_VEL;
			break;
		case SDLK_LEFT:
			mVelX -= DOT_VEL;
			break;
		case SDLK_RIGHT:
			mVelX += DOT_VEL;
			break;
		}

	/* if a key was released */
	} else if (event.type == SDL_KEYUP && event.key.repeat == 0) {
		printf("\t\tkey released\n");

		/* adjust the velocity */
		switch (event.key.keysym.sym) {
		case SDLK_UP:
			mVelY += DOT_VEL;
			break;
		case SDLK_DOWN:
			mVelY -= DOT_VEL;
			break;
		case SDLK_LEFT:
			mVelX += DOT_VEL;
			break;
		case SDLK_RIGHT:
			mVelX -= DOT_VEL;
			break;
		}
	}
}

void Dot::move()
{
	/* move the dot left or right */
	mPosX += mVelX;

	/* if the dot went too far to the left or right */
	if ((mPosX < 0) || (mPosX + DOT_WIDTH > SCREEN_WIDTH))
		/* move back */
		mPosX -= mVelX;

	/* move the dot up or down */
	mPosY += mVelY;

	/* if the dot went to far up or down */
	if ((mPosY < 0) || (mPosY + DOT_HEIGHT > SCREEN_HEIGHT))
		/* move back */
		mPosY -= mVelY;
}

void Dot::render()
{
	printf("render X = %d, Y = %d\n", mPosX, mPosY);
	/* show the dot */
	gDotTexture.render(mPosX, mPosY);
}





/* starts up SDL and creates window */
int init_sdl(void);

/* loads medis */
int load_media(void);

/* frees media and shuts down SDL */
void close_sdl(void);




int
main(int argc, char *argv[])
{
	int quit = 0;
	/* event handler */
	SDL_Event event;

	/* the dot that will be moving around the screen */
	Dot dot;

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

			/* handle user key press */
			dot.handleEvent(event);
		}

		/* move the dot */
		dot.move();


		/* clear screen */
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(gRenderer);

		/* render objects */
		dot.render();

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

	/* create vsynced renderer for window */
	gRenderer = SDL_CreateRenderer(gWindow, -1,
			SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
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


#ifdef _SDL_TTF_H
		/* initialize SDL_ttf */
		if (TTF_Init() == -1) {
			fprintf(stderr, "SDL_ttf could not initialize! "
				"SDL_ttf Error: %s\n", TTF_GetError());
			return -1;
		}
#endif
	}

	return 0;
}

int
load_media(void)
{
	/* load dot texture */
	if (gDotTexture.loadFromFile("dot.bmp") == -1) {
		fprintf(stderr, "Failed to load dot texture!\n");
		return -1;
	}

	return 0;
}


void
close_sdl(void)
{
	printf("close_sdl()\n");

	/* free loaded image */
	gDotTexture.free();


	/* destroy window */
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;


	/* Quit SDL subsystem */
#ifdef _SDL_TTF_H
	TTF_Quit();
#endif
	IMG_Quit();
	SDL_Quit();
}






