#ifndef GRAPHIXENGINE_HPP
#define GRAPHIXENGINE_HPP

/**
 * @class GraphixEngine
 * @author aLaix
 * @date 04/11/2015
 * @file GraphixEngine.hpp
 * @brief This is a Graphics Engine for SDL, taking in account this could be ported to:
 *        OpenGL or software render, like in the ol' days.
 */

#include <stdio.h>
#include <SDL2/SDL.h>
#include "Drawable.hpp"
#include "Color.hpp"

#define GFX_SOFTWARE_RENDER

//Platform-Dependant
extern SDL_Renderer* sdlRenderer;

class GraphixEngine
{
private:
    Color32 background;

    //Platform-Dependant
    SDL_Window* window;
    SDL_Surface* screenSurface;

    GraphixEngine(const GraphixEngine& rhs);
    GraphixEngine& operator=(const GraphixEngine& rhs);

public:
    GraphixEngine(const unsigned Width, const unsigned Height);
    ~GraphixEngine();

    inline void DrawBegin() const
    {
        //Platform-Dependant
        // Clear the window
        SDL_RenderClear(sdlRenderer);
    }
    inline void DrawEnd() const
    {
        //Platform-Dependant
        // Render the changes
        #ifndef GFX_SOFTWARE_RENDER
            SDL_RenderPresent(sdlRenderer);
        #else
            SDL_UpdateWindowSurface(window);
        #endif // GFX_SOFTWARE_RENDER
    }
    void SetBackgroundColor(const Color32& Color)
    {
        //Platform-Dependant
        // Set color of renderer
        SDL_SetRenderDrawColor(sdlRenderer, Color.GetR(), Color.GetG(),
            Color.GetB(), 255);
    }

};

#endif // GRAPHIXENGINE_HPP
