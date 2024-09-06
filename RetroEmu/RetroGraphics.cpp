#include "RetroGraphics.hpp"

// Platform-Dependent
/*extern*/ SDL_Renderer* sdlRenderer = NULL;

RetroGraphics::RetroGraphics(const unsigned internalWidth, const unsigned internalHeight, const double zoom)
    : screenSurface(NULL)
{
    const unsigned screenWidth = internalWidth * zoom;
    const unsigned screenHeight = internalHeight * zoom;

    // Platform-Dependent
    window = NULL;
    // Initialize SDL
    if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
        Log.error("RetroGraphics SDL Video could not initialize: %s", SDL_GetError());
        return;
    }
    // The window we'll be rendering to
    window = SDL_CreateWindow("O-Nes-Sama", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        Log.error("RetroGraphics window could not be created: %s", SDL_GetError());
        return;
    }
#ifndef GFX_SOFTWARE_RENDER
    sdlRenderer = SDL_CreateRenderer(window, -1, 0);
#else
    SDL_Surface* surface = SDL_GetWindowSurface(window);
    sdlRenderer = SDL_CreateSoftwareRenderer(surface);
#endif // GFX_SOFTWARE_RENDER
    if (sdlRenderer == NULL) {
        Log.error("RetroGraphics failed to create renderer: %s", SDL_GetError());
        return;
    }
    // Set size of renderer to the same as window
    /*SDL_RenderSetLogicalSize(sdlRenderer, ScreenWidth, ScreenHeight);
    SDL_RendererInfo* info;
    SDL_GetRendererInfo(sdlRenderer, info);
    for(int i=0; i<info->num_texture_formats; i++)
        if(info->texture_formats[i] == SDL_PIXELFORMAT_RGBA1555);
            Log.debug(LogCategory::otherRetroGraphics, "YATTA");*/

    // unsigned format = SDL_GetWindowPixelFormat(window);
    sdlPixelFormat = SDL_AllocFormat(SDL_PIXELFORMAT_RGB888);
    Color32::Transparent.setColor(0, 0, 0, 0);

    // Init
    sdlRect.x = 0;
    sdlRect.y = 0;

    texture = SDL_CreateTexture(sdlRenderer, sdlPixelFormat->format,
        SDL_TEXTUREACCESS_STREAMING, internalWidth, internalHeight);
    sdlRect.w = screenWidth;
    sdlRect.h = screenHeight;
    if (texture == NULL) {
        Log.error("RetroGraphics failed to init: %s", SDL_GetError());
        throw false;
    }
}

bool RetroGraphics::draw(const void* pixels, const size_t sizeInBytes)
{
    // Platform-Dependent
#define T uint32_t
    const T* dataPixels = (T*)(pixels);
    T* texturePixels = NULL;
    int pitch = 0;
    if (SDL_LockTexture(texture, NULL, (void**)&texturePixels, &pitch)) {
        Log.error("RetroGraphics failed to lock texture: %s", SDL_GetError());
        return false;
    }
    for (unsigned pos = 0; pos < sizeInBytes / sizeof(T); pos++)
        texturePixels[pos] = dataPixels[pos];
    SDL_UnlockTexture(texture);

    if (SDL_RenderCopyEx(sdlRenderer, texture, NULL, &sdlRect, 0., NULL, SDL_FLIP_NONE) < 0) {
        Log.error("RetroGraphics failed to draw: %s", SDL_GetError());
    }
    return true;
#undef T
}

bool RetroGraphics::loadColorPaletteFromFile(const char* fileName)
{
    FILE* file;
    file = fopen(fileName, "rb");
    if (!file)
        return false;
    for (int i = 0; i < 64; i++) {
        unsigned char r = fgetc(file);
        unsigned char g = fgetc(file);
        unsigned char b = fgetc(file);
        colorPalette[i].setColor(r, g, b);
    }
    fclose(file);
    return true;
}

bool RetroGraphics::loadColorPaletteFromArray(const unsigned char* palette)
{
    for (int i = 0; i < 64; i++) {
        unsigned char r = palette[i * 3 + 0];
        unsigned char g = palette[i * 3 + 1];
        unsigned char b = palette[i * 3 + 2];
        colorPalette[i].setColor(r, g, b);
    }
    return true;
}

bool RetroGraphics::drawPaletted(const unsigned char* palettedPixels, const size_t sizeInBytes)
{
    Color32 framebuffer[sizeInBytes];
    for (size_t pos = 0; pos < sizeInBytes; pos++) {
        framebuffer[pos].setColor(colorPalette[palettedPixels[pos]].getColor());
    }
    return draw((void*)framebuffer, sizeInBytes * 4);
}

RetroGraphics::~RetroGraphics()
{
    // Platform-Dependent
    if (sdlRenderer)
        SDL_DestroyRenderer(sdlRenderer);
    if (window)
        SDL_DestroyWindow(window);
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}
