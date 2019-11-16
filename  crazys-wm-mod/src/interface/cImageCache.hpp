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

#ifndef CRAZYS_WM_MOD_CIMAGECACHE_HPP
#define CRAZYS_WM_MOD_CIMAGECACHE_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include "cSurface.h"
#include "cAnimatedSurface.h"

struct sColor;

struct sImageCacheKey {
    std::string file_name;
    int width;
    int height;
    bool transparency;
    bool keep_ratio;

    bool operator==(const sImageCacheKey&) const;
};

// surfaces
struct SurfaceDeleter {
    void operator()(SDL_Surface* surface);
};

using surface_ptr_t = std::unique_ptr<SDL_Surface, SurfaceDeleter>;

class cCachedSurface {
    surface_ptr_t m_Surface;
public:
    cCachedSurface(surface_ptr_t surface, std::string name);
    ~cCachedSurface() = default;

    const std::string& name() const { return m_Name; }

    std::size_t width() const;
    std::size_t height() const;
    std::size_t bytes() const;

    SDL_Surface* surface() const;

private:
    std::string m_Name;
};

using cached_surface_ptr_t = std::shared_ptr<cCachedSurface> ;

namespace std
{
    template<> struct hash<sImageCacheKey>
    {
        std::size_t operator()(sImageCacheKey const& s) const noexcept;
    };
}

/*!
 * \brief Responsible for keeping a cache of images to reduce the number of times
 * we need to reload from disk.
 * \details This replaces the old `CResourceManager`.
 */
class cImageCache {
public:
    ~cImageCache();
    void PrintStats() const;

    // Generating new surfaces.
    cSurface CreateSurface(int width, int height, sColor color, bool make_unique=false);
    cSurface LoadImage(std::string filename, int width=-1, int height=-1, bool transparency=false, bool keep_ratio=true, SDL_Rect* clip=nullptr);
    /// TODO imporve this one
    cSurface GetSubImage(int width, int height, const cSurface& source, SDL_Rect src_rect);
    cSurface CreateTextSurface(TTF_Font* font, std::string text, sColor color, bool antialias);

    cAnimatedSurface LoadGif(std::string filename, int target_width, int target_height);
    cAnimatedSurface LoadAni(std::string gfx_file, const std::string& ani_file, int target_width, int target_);

    // Modifying functions
    cSurface FillRect(const cSurface& source, SDL_Rect area, sColor color);
    cSurface BlitSurface(const cSurface& target, SDL_Rect* target_area, const cSurface& source, SDL_Rect* src_area);

    // Low Level Functions
    surface_ptr_t CreateSDLSurface(int width, int height, bool transparent);
    surface_ptr_t CloneSDLSurface(SDL_Surface* sf);

    void Cull();
private:
    cSurface AddToCache(sImageCacheKey key, surface_ptr_t surface, std::string name);

    surface_ptr_t ResizeImage(surface_ptr_t input, int width, int height, bool keep_ratio);

    using cache_map_t = std::unordered_map<sImageCacheKey, cached_surface_ptr_t>;
    cache_map_t m_SurfaceCache;

    std::size_t m_AddedSize = 0;             // counts how many bytes were added since the last culling

    // stats
    std::size_t m_ImageFoundCount  = 0;      // how often did we look up successfully
    std::size_t m_ImageLoadCount   = 0;      // how often did we have to reload data
    std::size_t m_ImageCreateCount = 0;      // how many monochrome surfaces were created
    std::size_t m_ImageResizeCount = 0;
    std::size_t m_ImageCullCount   = 0;
};


#endif //CRAZYS_WM_MOD_CIMAGECACHE_HPP
