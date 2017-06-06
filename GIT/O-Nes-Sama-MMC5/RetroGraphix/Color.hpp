#ifndef COLOR_HPP
#define COLOR_HPP

#include <cinttypes>
#include <SDL2/SDL.h>
#include <stdio.h>

//Platform-Dependant
extern SDL_PixelFormat* sdlPixelFormat;

template <class T>
class Color
{
private:
    T nativeColor;

    //Platform-Dependant
    static unsigned char r;
    static unsigned char g;
    static unsigned char b;

public:
    Color()
    {
    }

    explicit inline Color(const T Color)
    {
        SetColor(Color);
    }

    inline Color(const unsigned char R, const unsigned char G,
        const unsigned char B)
    {
        SetColor(R, G, B);
    }

    inline unsigned char GetR() const
    {
        //Platform-Dependant
        SDL_GetRGB(nativeColor, sdlPixelFormat, &r, &g, &b);
        return r;
    }
    inline unsigned char GetG() const
    {
        //Platform-Dependant
        SDL_GetRGB(nativeColor, sdlPixelFormat, &r, &g, &b);
        return g;
    }
    inline unsigned char GetB() const
    {
        //Platform-Dependant
        SDL_GetRGB(nativeColor, sdlPixelFormat, &r, &g, &b);
        return b;
    }
    inline void SetColor(const T Color)
    {
        nativeColor = Color;
    }
    inline void SetColor(const unsigned char R, const unsigned char G,
        const unsigned char B)
    {
        //Platform-Dependant
        if(sdlPixelFormat)
            nativeColor = SDL_MapRGB(sdlPixelFormat, R, G, B);
        else
            printf("Error: PixelFormat in Color is NULL\n");
    }
    inline void SetColor(const unsigned char R, const unsigned char G,
        const unsigned char B, const unsigned char A)
    {
        //Platform-Dependant
        if(sdlPixelFormat)
            nativeColor = SDL_MapRGBA(sdlPixelFormat, R, G, B, A);
        else
            printf("Error: PixelFormat in Color is NULL\n");
    }
    inline T GetColor() const
    {
        return nativeColor;
    }
};

//Platform-Dependant
template<class T>
unsigned char Color<T>::r;
template<class T>
unsigned char Color<T>::g;
template<class T>
unsigned char Color<T>::b;

class Color16 : public Color<unsigned short>
{
    using Color::Color;
};

class Color32 : public Color<unsigned>
{
friend class GraphixEngine;

public:
    using Color::Color;
    static Color32 Transparent;
};


#endif // COLOR_HPP
