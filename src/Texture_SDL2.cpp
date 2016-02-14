//
//  Texture_SDL2.cpp
//  HumbleASMJSDemo
//
//  Created by Edward Rudd on 2/25/15.
//
//

#include "Texture_SDL2.h"

#include <SDL_LodePNG.h>
#include <SDL_ttf.h>

Texture_SDL2::Texture_SDL2(Renderer_SDL2& rend, const std::string& filename) : m_texture(0)
{
    SDL_Surface *surface = SDL_LoadPNG(filename.c_str());
    consume_surface(rend, surface);
}

Texture_SDL2::Texture_SDL2(Renderer_SDL2& rend, const std::string& filename, const std::string& text, int size, SDL_Color color) : m_texture(0)
{
    TTF_Font *font = TTF_OpenFont(filename.c_str(), size);
    if (font) {
        SDL_Surface *surface = TTF_RenderText_Blended(font, text.c_str(), color);
        consume_surface(rend, surface);
        TTF_CloseFont(font);
    }
}

void Texture_SDL2::consume_surface(Renderer_SDL2& rend, SDL_Surface *surface)
{
    if (surface) {
        m_texture = SDL_CreateTextureFromSurface(rend.m_rend, surface);
        if (m_texture) {
            m_size.x = surface->w;
            m_size.y = surface->h;
        }
        SDL_FreeSurface(surface);
    }
}

Texture_SDL2::~Texture_SDL2()
{
    if (m_texture) {
        SDL_DestroyTexture(m_texture);
    }
}
