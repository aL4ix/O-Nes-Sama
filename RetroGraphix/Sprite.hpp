#ifndef SPRITE_HPP
#define SPRITE_HPP

#include <SDL2/SDL.h>
#include "Texture.hpp"
#include "GraphixEngine.hpp"
#include "Drawable.hpp"

class Sprite : public Drawable
{
private:
    const Texture* texture;

    //Platform-Dependant
    SDL_Rect sdlRect;
    SDL_RendererFlip sdlFlip;

public:
    Sprite();
    ~Sprite();

    inline void Draw() const
    {
        //Platform-Dependant
        //if(texture)
            if(SDL_RenderCopyEx(sdlRenderer, texture->texture, NULL, &sdlRect, 0., NULL, sdlFlip) < 0)
                printf("Failed to render sprite: %s\n", SDL_GetError());
    }
    inline void MoveX(const int X)
    {
        //Platform-Dependant
        sdlRect.x = X;
    }
    inline void MoveY(const int Y)
    {
        //Platform-Dependant
        sdlRect.y = Y;
    }
    inline void MoveTo(const int X, const int Y)
    {
        MoveX(X);
        MoveY(Y);
    }
    inline int GetX() const
    {
        //Platform-Dependant
        return sdlRect.x;
    }
    inline int GetY() const
    {
        //Platform-Dependant
        return sdlRect.y;
    }
    inline void SetWidth(const int Width)
    {
        //Platform-Dependant
        sdlRect.w = Width;
    }
    inline void SetHeight(const int Height)
    {
        //Platform-Dependant
        sdlRect.h = Height;
    }
    inline void FlipX()
    {
        //Platform-Dependant
        sdlFlip = SDL_FLIP_HORIZONTAL;
    }
    inline void FlipY()
    {
        //Platform-Dependant
        sdlFlip = SDL_FLIP_VERTICAL;
    }
    inline void FlipXY()
    {
        //Platform-Dependant
        sdlFlip = SDL_RendererFlip(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL);
    }
    inline void UnFlip()
    {
        //Platform-Dependant
        sdlFlip = SDL_FLIP_NONE;
    }
    inline void SetTexture(const Texture& Texture)
    {
        SetTextureAndDontUpdateSpriteSize(Texture);
        SetWidth(Texture.GetWidth());
        SetHeight(Texture.GetHeight());
    }
    inline void SetTextureAndDontUpdateSpriteSize(const Texture& Texture)
    {
        texture = &Texture;
    }
    inline const Texture* GetTexture() const
    {
        return texture;
    }
};

#endif // SPRITE_HPP
