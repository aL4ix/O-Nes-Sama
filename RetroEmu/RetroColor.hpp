#ifndef RETRO_COLOR_HPP
#define RETRO_COLOR_HPP

#include "../Logging/Logger.hpp"
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

    explicit inline RetroColor(const T retroColor)
    {
        setColor(retroColor);
    }

    inline RetroColor(const unsigned char r, const unsigned char g,
        const unsigned char b)
    {
        setColor(r, g, b);
    }

    inline unsigned char getR() const
    {
        // Platform-Dependent
        SDL_GetRGB(nativeColor, sdlPixelFormat, &r, &g, &b);
        return r;
    }
    inline unsigned char getG() const
    {
        // Platform-Dependent
        SDL_GetRGB(nativeColor, sdlPixelFormat, &r, &g, &b);
        return g;
    }
    inline unsigned char getB() const
    {
        // Platform-Dependent
        SDL_GetRGB(nativeColor, sdlPixelFormat, &r, &g, &b);
        return b;
    }
    inline void setColor(const T retroColor)
    {
        nativeColor = retroColor;
    }
    inline void setColor(const unsigned char r, const unsigned char g,
        const unsigned char b)
    {
        // Platform-Dependent
        if (sdlPixelFormat)
            nativeColor = SDL_MapRGB(sdlPixelFormat, r, g, b);
        else
            Log.error("RetroColor pixelFormat in Color is NULL");
    }
    inline void setColor(const unsigned char r, const unsigned char g,
        const unsigned char b, const unsigned char a)
    {
        // Platform-Dependent
        if (sdlPixelFormat)
            nativeColor = SDL_MapRGBA(sdlPixelFormat, r, g, b, a);
        else
            Log.error("RetroColor pixelFormat in Color is NULL");
    }
    inline T getColor() const
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
