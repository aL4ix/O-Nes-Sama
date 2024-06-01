#ifndef RETRO_GRAPHICS_HPP
#define RETRO_GRAPHICS_HPP

#include "RetroColor.hpp"
#include <SDL2/SDL.h>
#include <stdio.h>

#define GFX_SOFTWARE_RENDER

// Platform-Dependent
extern SDL_Renderer* sdlRenderer;

class RetroGraphics {
private:
    Color32 background;

    // Platform-Dependent
    SDL_Window* window;
    SDL_Surface* screenSurface;

    RetroGraphics(const RetroGraphics& rhs);
    RetroGraphics& operator=(const RetroGraphics& rhs);

public:
    RetroGraphics(const unsigned internalWidth, const unsigned internalHeight, const double zoom);
    ~RetroGraphics();

    inline void drawBegin() const
    {
        // Platform-Dependent
        //  Clear the window
        SDL_RenderClear(sdlRenderer);
    }
    inline void drawEnd() const
    {
// Platform-Dependent
//  Render the changes
#ifndef GFX_SOFTWARE_RENDER
        SDL_RenderPresent(sdlRenderer);
#else
        SDL_UpdateWindowSurface(window);
#endif // GFX_SOFTWARE_RENDER
    }
    SDL_Rect sdlRect;
    SDL_Texture* texture;
    bool draw(const void* pixels, const size_t sizeInBytes);
    bool drawPaletted(const unsigned char* pixels, const size_t sizeInBytes);
    bool loadColorPaletteFromFile(const char* fileName);
    bool loadColorPaletteFromArray(const unsigned char* palette);
    Color32 colorPalette[64];
};

#endif // RETRO_GRAPHICS_HPP
