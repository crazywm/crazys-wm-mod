/*
 * Copyright 2009, 2010, The Pink Petal Development Team.
 * The Pink Petal Devloment Team are defined as the game's coders 
 * who meet on http://pinkpetal.org
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

#include "interface/cFont.h"
#include <SDL_ttf.h>
#include "CLog.h"
#include "interface/CGraphics.h"
#include <vector>
#include <numeric>
#include <cassert>
#include <algorithm>
#include "interface/cColor.h"

cFont::cFont(CGraphics* gfx) : m_GFX(gfx), m_TextColor(0, 0, 0)
{
}

void cFont::SetColor(unsigned char r, unsigned char g, unsigned char b)
{
    m_TextColor.r = r;
    m_TextColor.g = g;
    m_TextColor.b = b;
}

/*
 * old version for reference (and in case I balls it up
 */
bool cFont::LoadFont(const std::string& font, int size)
{
    m_Font = nullptr;
    g_LogFile.debug("interface", "loading font: '", font, "' at size ", size);

    m_Font.reset(TTF_OpenFont(font.c_str(), size));
    if (!m_Font)
    {
        g_LogFile.error("interface", "Error in LoadFont for font file: ", font);
        g_LogFile.error("interface", TTF_GetError());
        return false;
    }
    return true;
}

int cFont::GetFontHeight()
{
    return TTF_FontHeight(m_Font.get());
}

int cFont::GetFontLineSkip() const
{
    return TTF_FontLineSkip(m_Font.get());
}

void cFont::SetFontBold(bool Bold)
{
    TTF_SetFontStyle(m_Font.get(), Bold ? TTF_STYLE_BOLD : TTF_STYLE_NORMAL);
}

void cFont::GetSize(const std::string& text, int& width, int& height) const
{
    TTF_SizeText(m_Font.get(), text.c_str(), &width, &height);
}

cSurface cFont::RenderText(std::string text) const
{
    return m_GFX->GetImageCache().CreateTextSurface(m_Font.get(), std::move(text),
            m_TextColor, true);

}

cSurface cFont::RenderMultilineText(std::string text, int max_width) const
{
    // pad the sides a bit, it was otherwise slightly overflowing
    max_width = max_width - 10;

    return render_lines(max_width, break_lines(std::move(text), max_width));
}

// Break up a string into individual lines at newlines.
std::vector<std::string> hard_linebreaks(std::string const& text)
{
  std::vector<std::string> lines;
  std::string line;

  for(size_t i = 0; i < text.size(); ++i)
    {
      if(text[i] == '\n')       // \n -- Unix style
        {
          lines.emplace_back(std::move(line));
          line.clear();
        }
      else if(text[i] == '\r') // \r -- Mac style
        {
          lines.emplace_back(std::move(line));
          line.clear();
          if(i+1 < text.size() && text[i+1] == '\n') // \r\n -- Windows style
            ++i;
        }
      else
        line.push_back(text[i]);
    }

  assert(std::none_of(line.begin(), line.end(),
                      [](char ch) {return ch == '\n' || ch == '\r';}));

  lines.emplace_back(std::move(line));

  return lines;
}

// Break up a string into lines to fit in a column `max_width` pixels
// wide. 
//
// To keep things simple we break at the last end-of-word that doesn't
// overfill the line. (There are more advanced algorithms out there.)
//
// Hard line breaks ('\n' etc) are assumed to have been dealt with.
std::vector<std::string>
cFont::find_soft_linebreaks(std::string const& text, int max_width) const
{
  // first separate into lines according to width
  std::vector<std::string> lines;
  std::string temp(text);    // current line of text
  temp += " ";    // makes sure that all the text will be displayed
  int next_word_boundary = 0;    // current index into the string
  int last_split_point = 0;    // holds the previous index into the string

  constexpr const auto npos = std::string::npos;
  
  // -- Get until ' '
  while (next_word_boundary != -1)
    {
      std::string strSub;
      next_word_boundary = temp.find(' ', last_split_point + 1);        // -- Find the next " "

      // word boundary
      strSub = temp.substr(0, next_word_boundary);
      int charwidth, charheight;
      GetSize(strSub, charwidth, charheight);
      // of we exceed the line, or this is the last word, use the last split point as split position
      if (charwidth >= max_width || next_word_boundary == -1)
        {
          // if we don't have any preceeding words, there is no point in splitting
          if(last_split_point == 0) {
            lines.push_back(strSub);  // put this long word into a signle line
            if (next_word_boundary != -1)
              temp = temp.substr(next_word_boundary + 1, npos);
            last_split_point = 0;
          } else {
            strSub = temp.substr(0, last_split_point);
            lines.push_back(strSub);    // -- Puts strSub into the lines vector
            if (next_word_boundary != -1) temp = temp.substr(last_split_point + 1, npos);
            last_split_point = 0;
          }
        }
      else
        // if it fits, include this word in the next segment
        last_split_point = next_word_boundary;
    }
  return lines;
};

std::vector<std::string>
cFont::break_lines(std::string text, int max_width) const
{
  std::vector<std::string> lines;

  for(auto const& line : hard_linebreaks(std::move(text)))
    for(auto l : find_soft_linebreaks(line, max_width))
      lines.emplace_back(std::move(l));

  return lines;
}

cSurface
cFont::render_lines(int max_width, std::vector<std::string> const& lines) const
{
    int lineskip = GetFontLineSkip();
    int height = lines.size()*lineskip;

    // create a surface to render all the text to
    auto surface = m_GFX->CreateSurface(max_width, height, m_TextColor, true);
    assert(surface);

    for (unsigned int i = 0; i < lines.size(); i++)
    {
        if(!lines[i].empty()) {
            auto line = m_GFX->GetImageCache().CreateTextSurface(
                    m_Font.get(), lines[i], m_TextColor, true);
            if(!line)
                continue;
            SDL_Rect dst = {0, static_cast<Sint16>(i * lineskip),
                            static_cast<Uint16>(line.GetWidth()), static_cast<Uint16>(line.GetHeight())};
            surface = surface.BlitOther(line, nullptr, &dst);
        }
    }
    return surface;
}

cSurface cFont::RenderTable(const std::string& text, int max_width) const {
    // split into cells
    struct sCellData {
        int tabs;
        std::string content;
        cSurface render;
        bool is_last;
    };

    int tab = 0;
    int num_tabs = 0;
    std::vector<sCellData> cells;
    std::string next_cell;

    auto add_cell = [&, this](std::string text, bool last) {
        num_tabs = std::max(tab+1, num_tabs);
        if(text.empty()) {
            cells.push_back(sCellData{tab, std::move(text), cSurface(), last});
            return;
        }

        auto render = m_GFX->GetImageCache().CreateTextSurface(
                m_Font.get(), text, m_TextColor, true);

        // SDL_SetSurfaceBlendMode(render.RawSurface()->surface(), SDL_BLENDMODE_NONE);
        cells.push_back(sCellData{tab, std::move(text), std::move(render), last});
    };

    // split text into cells
    for(auto& c : text) {
        if(c == '\t') {
            add_cell(std::move(next_cell), false);
            next_cell = "";
            ++tab;
        } else if(c == '\n') {
            add_cell(std::move(next_cell), true);
            next_cell = "";
            tab = 0;
        } else {
            next_cell.push_back(c);
        }
    }
    if(!next_cell.empty())
        add_cell(std::move(next_cell), true);

    // now determine tab positions
    std::vector<int> tab_widths(num_tabs, 0);
    for(auto& cell : cells) {
        if(!cell.render || cell.is_last)
            continue;
        tab_widths[cell.tabs] = std::max(tab_widths[cell.tabs], cell.render.GetWidth());
    }

    // determine the offsets
    std::vector<int> tab_start(num_tabs, 0);
    for(auto i = 0; i < num_tabs - 1; ++i) {
        tab_start[i+1] = tab_start[i] + tab_widths[i];
    }

    // at this point, we have the chance to split overlong lines
    for(int i = 0; i < cells.size(); ++i) {
        if(!cells[i].render) continue;
        int x_pos = tab_start[cells[i].tabs] + cells[i].render.GetWidth();
        if(x_pos > max_width && cells[i].content.size() > 1) {
            // split
            int split_point = cells[i].content.size() / 2;
            while(split_point > 0 && cells[i].content[split_point] != ' ')
                --split_point;
            std::string first = cells[i].content.substr(0, split_point);
            std::string last = cells[i].content.substr(split_point);

            auto render = m_GFX->GetImageCache().CreateTextSurface(
                    m_Font.get(), first, m_TextColor, true);

            cells[i] = sCellData{tab, std::move(first), std::move(render), true};

            render = m_GFX->GetImageCache().CreateTextSurface(
                    m_Font.get(), last, m_TextColor, true);
            cells.insert(cells.begin() + i + 1, sCellData{0, std::move(last), std::move(render), true} );
        }
    }

    // count the lines
    size_t num_lines =
       std::accumulate(begin(cells), end(cells),
                       size_t{0},
                       [](size_t n, sCellData const& cell){
                          return n + (cell.is_last ? 1 : 0);
                       });

    int lineskip = GetFontLineSkip();
    int height = (num_lines+1)*lineskip;

    // create a surface to render all the text too
    auto message = m_GFX->CreateSurface(max_width, height, m_TextColor, true);

    int line = 0;
    for(const auto& cell : cells) {
        if(!cell.render) {
            if(cell.is_last)
                line += 1;
            continue;
        }

        int x = tab_start.at(cell.tabs);
        SDL_Rect dst = {static_cast<Sint16>(x),
                        static_cast<Sint16>(line * lineskip),
                        static_cast<Uint16>(cell.render.GetWidth()),
                        static_cast<Uint16>(cell.render.GetHeight())};
        message = message.BlitOther(cell.render, nullptr, &dst);

        if(cell.is_last)
            line += 1;
    }
    return message;
}


void FontDeleter::operator()(TTF_Font * font)
{
    if(font)
        TTF_CloseFont(font);
}
