#ifndef RETRO_COLOR_HPP
#define RETRO_COLOR_HPP

#include "../Logging/Logger.h"
#include <SDL2/SDL.h>
#include <cinttypes>
#include <stdio.h>

// Platform-Dependent
extern SDL_PixelFormat* sdlPixelFormat;

template <class T>
class RetroColor {
private:
    T nativeColor;

    // Platform-Dependent
    static unsigned char r;
    static unsigned char g;
    static unsigned char b;

public:
    RetroColor()
    {
    }

    explicit inline RetroColor(const T RetroColor)
    {
        SetColor(RetroColor);
    }

    inline RetroColor(const unsigned char R, const unsigned char G,
        const unsigned char B)
    {
        SetColor(R, G, B);
    }

    inline unsigned char GetR() const
    {
        // Platform-Dependent
        SDL_GetRGB(nativeColor, sdlPixelFormat, &r, &g, &b);
        return r;
    }
    inline unsigned char GetG() const
    {
        // Platform-Dependent
        SDL_GetRGB(nativeColor, sdlPixelFormat, &r, &g, &b);
        return g;
    }
    inline unsigned char GetB() const
    {
        // Platform-Dependent
        SDL_GetRGB(nativeColor, sdlPixelFormat, &r, &g, &b);
        return b;
    }
    inline void SetColor(const T RetroColor)
    {
        nativeColor = RetroColor;
    }
    inline void SetColor(const unsigned char R, const unsigned char G,
        const unsigned char B)
    {
        // Platform-Dependent
        if (sdlPixelFormat)
            nativeColor = SDL_MapRGB(sdlPixelFormat, R, G, B);
        else
            Log.error("RetroColor pixelFormat in Color is NULL");
    }
    inline void SetColor(const unsigned char R, const unsigned char G,
        const unsigned char B, const unsigned char A)
    {
        // Platform-Dependent
        if (sdlPixelFormat)
            nativeColor = SDL_MapRGBA(sdlPixelFormat, R, G, B, A);
        else
            Log.error("RetroColor pixelFormat in Color is NULL");
    }
    inline T GetColor() const
    {
        return nativeColor;
    }
};

// Platform-Dependent
template <class T>
unsigned char RetroColor<T>::r;
template <class T>
unsigned char RetroColor<T>::g;
template <class T>
unsigned char RetroColor<T>::b;

class Color16 : public RetroColor<unsigned short> {
    using RetroColor::RetroColor;
};

class Color32 : public RetroColor<unsigned> {
    friend class RetroGraphics;

public:
    using RetroColor::RetroColor;
    static Color32 Transparent;
};

#endif // RETRO_COLOR_HPP
