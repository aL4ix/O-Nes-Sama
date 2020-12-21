#ifndef RETRO_GRAPHICS_HPP
#define RETRO_GRAPHICS_HPP

#include <stdio.h>
#include <SDL2/SDL.h>
#include "RetroColor.hpp"

#define GFX_SOFTWARE_RENDER

//Platform-Dependent
extern SDL_Renderer* sdlRenderer;

class RetroGraphics
{
private:
    Color32 background;

    //Platform-Dependent
    SDL_Window* window;
    SDL_Surface* screenSurface;

    RetroGraphics(const RetroGraphics& rhs);
    RetroGraphics& operator=(const RetroGraphics& rhs);

public:
    RetroGraphics(const unsigned Width, const unsigned Height);
    ~RetroGraphics();

    inline void DrawBegin() const
    {
        //Platform-Dependent
        // Clear the window
        SDL_RenderClear(sdlRenderer);
    }
    inline void DrawEnd() const
    {
        //Platform-Dependent
        // Render the changes
        #ifndef GFX_SOFTWARE_RENDER
            SDL_RenderPresent(sdlRenderer);
        #else
            SDL_UpdateWindowSurface(window);
        #endif // GFX_SOFTWARE_RENDER
    }
    bool Init(const int width, const int height, const double zoom);
    SDL_Rect sdlRect;
    SDL_Texture* texture;
    bool Draw(const void* pixels, const size_t sizeInBytes);
    bool DrawPaletted(const unsigned char* pixels, const size_t sizeInBytes);
    bool loadColorPaletteFromFile(const char* FileName);
    bool loadColorPaletteFromArray(const unsigned char* Palette);
    Color32 colorPalette[64];
};

#endif // RETRO_GRAPHICS_HPP
