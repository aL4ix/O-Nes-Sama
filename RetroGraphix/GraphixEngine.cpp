#include "GraphixEngine.hpp"

//Platform-Dependant
SDL_Renderer* sdlRenderer = NULL;

GraphixEngine::GraphixEngine(const unsigned ScreenWidth, const unsigned ScreenHeight) :
    screenSurface(NULL)
{
    //Platform-Dependant
    window = NULL;
    //Initialize SDL
	if(SDL_Init(SDL_INIT_EVERYTHING | SDL_INIT_AUDIO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return;
	}
    //The window we'll be rendering to
    window = SDL_CreateWindow("O-Nes-Sama", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, ScreenWidth, ScreenHeight, SDL_WINDOW_SHOWN);
    if(window == NULL)
    {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return;
    }
    #ifndef GFX_SOFTWARE_RENDER
        sdlRenderer = SDL_CreateRenderer(window, -1, 0);
    #else
        SDL_Surface* surface = SDL_GetWindowSurface(window);
        sdlRenderer = SDL_CreateSoftwareRenderer(surface);
    #endif // GFX_SOFTWARE_RENDER
    if (sdlRenderer == NULL)
	{
		printf("Failed to create renderer : %s\n", SDL_GetError());
		return;
	}
    // Set size of renderer to the same as window
	/*SDL_RenderSetLogicalSize(sdlRenderer, ScreenWidth, ScreenHeight);
    SDL_RendererInfo* info;
    SDL_GetRendererInfo(sdlRenderer, info);
    for(int i=0; i<info->num_texture_formats; i++)
        if(info->texture_formats[i] == SDL_PIXELFORMAT_RGBA1555);
            printf("YATTA");*/

    //unsigned format = SDL_GetWindowPixelFormat(window);
    sdlPixelFormat = SDL_AllocFormat(SDL_PIXELFORMAT_ARGB8888);
    Color32::Transparent.SetColor(0, 0, 0, 0);

}

GraphixEngine::~GraphixEngine()
{
    //Platform-Dependant
    if(sdlRenderer)
        SDL_DestroyRenderer(sdlRenderer);
    if(window)
        SDL_DestroyWindow(window);
    SDL_Quit();
}
