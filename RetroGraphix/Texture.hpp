#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <cstddef>
#include <SDL2/SDL.h>
#include "GraphixEngine.hpp"

class Texture
{
friend class Sprite;

private:
    unsigned w;
    unsigned h;

    //Platform-Dependant
    SDL_Texture* texture;

    Texture(const Texture& rhs);
    Texture& operator=(const Texture& rhs);

public:
    Texture();
    ~Texture();

    bool Create(const unsigned Width, const unsigned Height);
    inline unsigned GetWidth() const
    {
        return w;
    }
    unsigned GetHeight() const
    {
        return h;
    }
    bool LoadFromFile(const char* Path);
    bool LoadFromMemory(const void* Pixels, const size_t Size);
};

#endif // TEXTURE_HPP
