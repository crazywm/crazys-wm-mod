#
# Copyright 2009, 2010, The Pink Petal Development Team.
# The Pink Petal Devloment Team are defined as the game's coders 
# who meet on http://pinkpetal.co.cc
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

CPPFILES=$(wildcard *.cpp)
LUA_CFILES=$(wildcard lua/src/*.c)


WM_OFILES=build/SDL_anigif.o ${CPPFILES:%.cpp=build/%.o}
LUA_OFILES=${LUA_CFILES:%.c=build/%.o}

OFILES=${WM_OFILES}
CFLAGS=-DLINUX -DLUA_USE_MKSTEMP -I/usr/include/SDL -I/usr/include/lua5.1 -Wno-enum-compare -Wno-unused-result -O1 -fpermissive
CPPFLAGS=${CFLAGS} -std=c++11

build/%.o: %.c
	g++ $(CFLAGS) -c $< -o $@

build/%.o: %.cpp
	g++ $(CPPFLAGS) -c $< -o $@

all: WhoreMaster

build:
	mkdir build

WhoreMaster: build ${OFILES}
	g++ ${OFILES} -lSDL -lSDL_ttf -lSDL_gfx -lSDL_image -llua5.1 -o WhoreMaster

Revision.h:  ${OFILES}
	mk_rev

makefile.deps:
	g++ -M ${CPPFLAGS} ${CPPFILES} > makefile.deps

#include makefile.deps

RESFILES=Resources/Data/config.xml Resources/Characters/*girlsx \
	Resources/Data/*.itemsx Resources/Interface/*.xml \
	Resources/Characters/*/*.xml Resources/Characters/*/*.lua 


nix:
	dos2unix *.h *cpp ${RESFILES}

dos:
	unix2dos *.h *cpp ${RESFILES}

ctags:
	ctags *.h *.cpp

clean-deps:
	rm -f makefile.deps

clean: clean-deps
	rm -f build/*.o
	

install: WhoreMaster
	cp WhoreMaster ~/wm
	cp Resources/*.png ~/wm/Resources
	cp Resources/Data/* ~/wm/Resources/Data
	cp Resources/Interface/* ~/wm/Resources/Interface

