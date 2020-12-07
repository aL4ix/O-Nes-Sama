#include "Texture.hpp"

Texture::Texture()
{
    w = 0;
    h = 0;

    //Platform-Dependant
    texture = NULL;
}

Texture::~Texture()
{
    //Platform-Dependant
    if(texture)
        SDL_DestroyTexture(texture);
}

bool Texture::Create(const unsigned Width, const unsigned Height)
{
    texture = SDL_CreateTexture(sdlRenderer, sdlPixelFormat->format,
        SDL_TEXTUREACCESS_STREAMING, Width, Height);
    //SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    w = Width;
    h = Height;
    if(texture == NULL)
    {
        printf("%s\n", SDL_GetError());
        return false;
    }
    return true;
}
/*
unsigned Texture::GetWidth() const
{
    return w;
}

unsigned Texture::GetHeight() const
{
    return h;
}
*/
bool Texture::LoadFromMemory(const void* Pixels, const size_t SizeInBytes)
{
    #define T uint32_t

    const T* dataPixels = (T*)(Pixels);
    T* texturePixels = NULL;

    int pitch = 0;

    if(SDL_LockTexture(texture, NULL, (void**)&texturePixels, &pitch))
    {
        printf("%s\n", SDL_GetError());
        return false;
    }

    for(unsigned pos=0; pos<SizeInBytes/sizeof(T); pos++)
        texturePixels[pos] = dataPixels[pos];

    SDL_UnlockTexture(texture);
    return true;
}

bool Texture::LoadFromFile(const char* Path)
{
    //Platform-Dependant
    SDL_Surface* image = SDL_LoadBMP(Path);
    if(image == NULL)
    {
        printf("Failed to load surface: %s\n", SDL_GetError());
        return false;
    }
    texture = SDL_CreateTextureFromSurface(sdlRenderer, image);
    if(texture == NULL)
    {
        printf("Failed to load texture: %s\n", SDL_GetError());
        return false;
    }
    w = image->w;
    h = image->h;
    // Don't need the orignal texture, release the memory
	SDL_FreeSurface(image);

    return true;
}
