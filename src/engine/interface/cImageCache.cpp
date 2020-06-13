/*
* Copyright 2009, 2010, The Pink Petal Development Team.
* The Pink Petal Devloment Team are defined as the game's coders
* who meet on http://pinkpetal.org     // old site: http://pinkpetal .co.cc
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "cImageCache.hpp"
#include "cSurface.h"
#include "cColor.h"
#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_anigif.h>
#include <SDL_ttf.h>
#include <CLog.h>
#include <cassert>
#include <utility>
#include <iostream> // TODO replace with real logging!

// hashing (https://www.boost.org/doc/libs/1_55_0/doc/html/hash/reference.html#boost.hash_combine)
std::size_t combine() { return 0; }

template<class T, class... Args>
std::size_t combine(const T& b, const Args&... args)
{
    std::size_t seed = combine(args...);
    return seed ^ std::hash<T>{}(b) + 0x9e3779b9 + (seed << 6u) + (seed >> 2u);
}


std::size_t std::hash<sImageCacheKey>::operator()(sImageCacheKey const& s) const noexcept
{
    return combine(s.file_name, s.transparency, s.width, s.height, s.keep_ratio);
}

bool sImageCacheKey::operator==(const sImageCacheKey& o) const
{
    return std::tie(file_name, transparency, width, height, keep_ratio) == std::tie(
            o.file_name, o.transparency, o.width, o.height, o.keep_ratio);
}

cSurface cImageCache::CreateSurface(int width, int height, sColor color, bool transparent)
{
    std::string    fake_name = color.to_hex();
    sImageCacheKey key{fake_name, width, height, transparent, false};
    auto           lookup = m_SurfaceCache.find(key);
    if(lookup != m_SurfaceCache.end()) {
        // otherwise, return from cache
        m_ImageFoundCount++;
        return cSurface{lookup->second, m_GFX};
    }

    // CreateSDLSurface guarantees that sf is not nullptr, so we don't need to do any additional
    // checking here.
    auto sf = CreateSDLSurface(width, height, transparent);
    if(transparent)
        SDL_FillRect(sf.get(), nullptr, SDL_MapRGBA(sf->format, color.r, color.g, color.b, SDL_ALPHA_TRANSPARENT));
    else
        SDL_FillRect(sf.get(), nullptr, SDL_MapRGB(sf->format, color.r, color.g, color.b));

    return AddToCache(std::move(key), std::move(sf), std::move(fake_name));
}

cSurface cImageCache::LoadImage(std::string filename, int width, int height, bool transparency, bool keep_ratio, SDL_Rect* clip)
{
    sImageCacheKey key{filename, width, height, transparency, keep_ratio};
    auto           lookup = m_SurfaceCache.find(key);
    if(lookup != m_SurfaceCache.end()) {
        // otherwise, return from cache
        m_ImageFoundCount++;
        return cSurface{lookup->second, m_GFX};
    }

    // not cached, need to load
    auto loaded = surface_ptr_t{IMG_Load(filename.c_str())};
    if (!loaded) {
        throw std::runtime_error("Could not load image " + filename + ": " + IMG_GetError());
    }

    ++m_ImageLoadCount;

    // decide whether we need to resize
    if(width != -1 || height != -1) {
        loaded = ResizeImage(std::move(loaded), width, height, keep_ratio);
    }

    // set blend mode based on transparency
    SDL_SetSurfaceBlendMode(loaded.get(), transparency ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE);

    return AddToCache(std::move(key), std::move(loaded), std::move(filename));
}


cSurface cImageCache::GetSubImage(int width, int height, const cSurface& source, SDL_Rect src_rect)
{
    std::string fake_name;
    std::stringstream ss;
    ss << source.GetFileName() << "@[" << src_rect.x << ":" << src_rect.x + src_rect.x + src_rect.w << ","
                               << src_rect.y << ":" << src_rect.y + src_rect.h << "]";
    fake_name = ss.str();

    // TODO transparency; keep ratio
    sImageCacheKey key{fake_name, width, height, false, true};
    auto           lookup = m_SurfaceCache.find(key);
    if(lookup != m_SurfaceCache.end()) {
        // otherwise, return from cache
        m_ImageFoundCount++;
        return cSurface{lookup->second, m_GFX};
    }

    auto sf = CreateSDLSurface(src_rect.w, src_rect.h, false);
    // Draw the current frame using the frame rectangle.
    SDL_BlitSurface(source.RawSurface()->surface(), &src_rect, sf.get(), nullptr);
    sf = ResizeImage(std::move(sf), width, height, true);

    return AddToCache(std::move(key), std::move(sf), std::move(fake_name));
}


surface_ptr_t cImageCache::ResizeImage(surface_ptr_t input, int width, int height, bool keep_ratio)
{
    if (width != input->w || height != input->h)
    {
        if(width  == -1) { width = input->w; }
        if(height == -1) { height = input->h; }
        if (keep_ratio)  {
            double scale_x = (double)width / (double)input->w;
            double scale_y = (double)height / (double)input->h;
            double s = std::min(scale_x, scale_y);
            width = input->w * s;
            height = input->h * s;
        }

        auto target = CreateSDLSurface(width, height, true);
        SDL_Rect inp{0, 0, input->w, input->h};
        SDL_Rect tgt{0, 0, target->w, target->h};
        // First adapt pixel format, then scale
        auto intermed = CreateSDLSurface(input->w, input->h, true);
        SDL_BlitSurface(input.get(), &inp, intermed.get(), nullptr);

        if(SDL_BlitScaled(intermed.get(), &inp, target.get(), &tgt)) {
            g_LogFile.error("interface", "Could not scale surface: ", SDL_GetError());
        }
        SDL_BlendMode mode = SDL_BLENDMODE_INVALID;
        SDL_GetSurfaceBlendMode(input.get(), &mode);
        SDL_SetSurfaceBlendMode(target.get(), mode);
        m_ImageResizeCount++;
        return target;
    }

    return input;
}

cImageCache::~cImageCache()
{
    PrintStats();
}

void cImageCache::PrintStats() const
{
    g_LogFile.debug_table("interface", "ImageCache Stats",
            "Current", m_SurfaceCache.size(),
            "Loaded", m_ImageLoadCount,
            "Found", m_ImageFoundCount,
            "Resized", m_ImageResizeCount,
            "Created", m_ImageCreateCount,
            "Culled", m_ImageCullCount
            );
}

void cImageCache::Cull()
{
    std::size_t freeable_size = 0;
    std::size_t total = 0;
    for(auto& entry : m_SurfaceCache) {
        if(entry.second.use_count() == 1) {
            freeable_size += entry.second->bytes();
        }
        total += entry.second->bytes();
    }

    // if we cannot free more than 50MB, leave stuff in cache.
    if(freeable_size < 1024*1024*50)
        return;

    g_LogFile.info("interface", "Culling ", freeable_size / 1024 / 1024, "MB of ", total / 1024 / 1024);

    // TODO keep some cullable surfaces in cache based on usage
    for(auto entry = m_SurfaceCache.begin(); entry != m_SurfaceCache.end();) {
        if(entry->second.use_count() == 1) {
            entry = m_SurfaceCache.erase(entry);
            m_ImageCullCount++;
        } else {
            ++entry;
        }
    }
}

cSurface cImageCache::AddToCache(sImageCacheKey key, surface_ptr_t surface, std::string name)
{
    auto cached = std::make_shared<cCachedSurface>(std::move(surface), std::move(name));

    if(m_AddedSize > 10'000'000) {
        Cull();
        m_AddedSize = 0;
    }
    m_AddedSize += cached->bytes();
    m_SurfaceCache.emplace(std::move(key), cached);
    return cSurface(std::move(cached), m_GFX);
}

surface_ptr_t cImageCache::CreateSDLSurface(int width, int height, bool transparent)
{
    assert(width > 0);
    assert(height > 0);
    if(width <= 0 || height <= 0) {
        throw std::logic_error("Invalid size specified for SDLSurface");
    }
    m_ImageCreateCount++;
    auto sf = SDL_CreateRGBSurface(0, width, height, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
    if(!sf) {
        throw std::runtime_error("Could not create SDL surface");
    }
    if(transparent) {
        SDL_SetSurfaceBlendMode(sf, SDL_BLENDMODE_BLEND);
    }
    return surface_ptr_t{sf};
}

surface_ptr_t cImageCache::CloneSDLSurface(SDL_Surface * sf)
{
    m_ImageCreateCount++;
    return surface_ptr_t{SDL_ConvertSurface(sf, sf->format, sf->flags)};
}

cSurface cImageCache::FillRect(const cSurface& source, SDL_Rect area, sColor color)
{
    surface_ptr_t surface;
    const auto& raw = *source.RawSurface();
    // make a copy of the old surface
    auto target = CloneSDLSurface(raw.surface());
    // draw the rect
    SDL_FillRect(target.get(), &area, SDL_MapRGB(raw.surface()->format, color.r, color.g, color.b));

    // and create a new cache entry
    std::stringstream name;
    name << "CustomSurface#";
    name << m_ImageCreateCount;
    return AddToCache(sImageCacheKey{std::move(name.str()), (int)raw.width(), (int)raw.height(), false, false}, std::move(target), source.GetFileName());
}

cSurface cImageCache::BlitSurface(const cSurface& target, SDL_Rect* target_area, const cSurface& source, SDL_Rect* src_area)
{
    surface_ptr_t surface;
    const auto& raw = *target.RawSurface();
    // make a copy of the old surface
    auto new_surface = CloneSDLSurface(raw.surface());
    // blit the other surface
    SDL_BlitSurface(source.RawSurface()->surface(), src_area, new_surface.get(), target_area);
    // and create a new cache entry
    std::stringstream name;
    name << "CustomSurface#";
    name << m_ImageCreateCount;
    return AddToCache(sImageCacheKey{std::move(name.str()), (int)raw.width(), (int)raw.height(), false, false}, std::move(new_surface), target.GetFileName());
}

cAnimatedSurface cImageCache::LoadGif(std::string filename, int target_width, int target_height)
{
    // TODO
    int frames = AG_LoadGIF(filename.c_str(), nullptr, 0);
    if (frames) {
        auto gpAG = std::make_unique<AG_Frame[]>(frames);
        AG_LoadGIF(filename.c_str(), gpAG.get(), frames);
        // convert to display format. This can increase the memory requirements about 4x, but allows
        // us to work with a simple array of frames from now on.
        AG_NormalizeSurfacesToDisplayFormat(gpAG.get(), frames);

        // extract the frames to "surfaces"
        std::vector<sAnimationFrame> surfaces;
        for(int i = 0; i < frames; ++i) {
            std::stringstream name;
            name << filename << "@" << i;
            auto surface = AddToCache(sImageCacheKey{std::move(name.str()), target_width, target_height, false, false},
                                      ResizeImage(surface_ptr_t{gpAG[i].surface}, target_width, target_height, true), filename);
            surfaces.push_back({std::move(surface), gpAG[i].delay});
        }
        return cAnimatedSurface{std::move(surfaces)};
    }

    return {};
}

cAnimatedSurface cImageCache::LoadAni(std::string gfx_file, const std::string& ani_file, int target_width, int target_height)
{
    int numFrames, speed, aniwidth, aniheight;
    std::ifstream input;
    input.open(ani_file);
    if (!input)
    {
        throw std::runtime_error("Invalid data file for animation - " + ani_file );
    }
    else
    {
        input >> numFrames >> speed >> aniwidth >> aniheight;
        auto base_image = LoadImage(std::move(gfx_file), -1, -1, true);

        auto cols = base_image.GetWidth() / aniwidth;
        auto rows = base_image.GetHeight() / aniheight;
        SDL_Rect rect;
        rect.w = aniwidth;
        rect.h = aniheight;

        std::vector<sAnimationFrame> surfaces;
        for(auto row = 0; row < rows; ++row) {
            for(auto col = 0; col < cols; ++col) {
                rect.x = col*rect.w;
                rect.y = row*rect.h;
                auto sf = GetSubImage(target_width, target_height, base_image, rect);
                surfaces.push_back(sAnimationFrame{std::move(sf), speed});
            }
        }
        return cAnimatedSurface{std::move(surfaces)};
    }
}

cSurface cImageCache::CreateTextSurface(TTF_Font* font, std::string text, sColor color, bool antialias)
{
    assert(font);

    std::string id = text + color.to_hex();
    sImageCacheKey key{id, -1, TTF_FontHeight(font), antialias, false};
    auto           lookup = m_SurfaceCache.find(key);
    if(lookup != m_SurfaceCache.end()) {
        // otherwise, return from cache
        m_ImageFoundCount++;
        return cSurface{lookup->second, m_GFX};
    }

    auto sdlColor = SDL_Color{color.r, color.g, color.b, 0};
    auto new_image = antialias ? TTF_RenderUTF8_Blended(font, text.c_str(), sdlColor)
                               : TTF_RenderUTF8_Solid(font, text.c_str(), sdlColor);
    if(new_image)
        return AddToCache(std::move(key), surface_ptr_t{new_image}, std::move(text));
    else
        return cSurface();
}

cImageCache::cImageCache(CGraphics* gfx) : m_GFX(gfx)
{
}

void SurfaceDeleter::operator()(SDL_Surface * surface)
{
    SDL_FreeSurface(surface);
}

std::size_t cCachedSurface::width() const
{
    return surface()->w;
}

std::size_t cCachedSurface::height() const
{
    return surface()->h;
}

cCachedSurface::cCachedSurface(surface_ptr_t surface, std::string name) :
    m_Name(move(name)),
    m_Surface(move(surface))
{

}

size_t cCachedSurface::bytes() const
{
    return m_Surface->w * m_Surface->h * m_Surface->format->BytesPerPixel;
}

SDL_Surface* cCachedSurface::surface() const
{
    return m_Surface.get();
}