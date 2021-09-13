# Whore Master 7
Automatically exported from code.google.com/p/crazys-wm-mod

<table>
<tr><td>The main site for this project is</td><td>http://www.pinkpetal.org</td></tr>
<tr><td>The main Forum for this project is</td><td>http://www.pinkpetal.org/index.php?board=5.0</td></tr>
<tr><td>Bug reports can be discussed here</td><td>https://www.pinkpetal.org/index.php?topic=4067.0</td></tr>
<tr><td>General discussion of the game is here</td><td>http://www.pinkpetal.org/index.php?topic=953.0</td></tr>
<tr><td>Discord</td><td>https://discord.gg/2XsPBPW</td></tr>
</table>

# Outline
This repository contains the following subdirectories:
* src - Source code for the WhoreMaster game
* deps - Dependencies that are included in this repository. These are lua and tinyxml, 
and precompiled versions of SDL for windows
* cmake - CMake helper scripts to find dependency libraries
* Resources - Images, data files, etc
* Docs&Tools - Other useful stuff, such as an editor for scripts, 
interface xml generator, old change logs
* Saves - Empty folder; this is where WM will put save files


# Building
## Linux
WM comes with a CMake build file that should make building relatively easy on linux.
On Ubuntu-based distributions, the required dependencies can be obtained using
```sh
apt install libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev libboost-all-dev
apt install libavformat-dev libswscale-dev libpng-dev libjpeg-dev libtiff-dev libwebp-dev
```

To generate documentation (see below), you also need
```sh
apt install doxygen graphviz
```

Then configure, build, and run with
```sh
mkdir build ; cd build
cmake ..
cmake --build .
cd .. ; ./build/WhoreMaster
```

By doing an out-of-source build like this, you can easily wipe your
build by deleting the build directory. If you just want to keep your
configuration and just rebuild, delete all build output with the
command
```sh
make clean
```

To run the WhoreMaster binary, you need to be at the top of your
working tree (where this file can be found), otherwise a number of
hardcoded paths will be wrong.

Note that the code uses C++14 features, and as such requires a relatively 
recent version of GCC (>=7). If you get compile errors using a newer compiler,
please submit a bug report or pull request.

## Windows
Building with CMake is also possible on windows, but getting the dependencies set
up may be a bit more work.

### MingW
You need to install MingW and CMake and ensure that the executables are in your `PATH`.
The SDL dependencies are already bundled in the repository, and should be automatically
selected by CMake, but [boost](https://www.boost.org) needs to be downloaded manually.
The easiest way is to simply place the boost headers in your compilers `include` directory,
you do not need to build boost yourself, WM only uses the header files.

You can then open the CMake GUI and select as source directory, 
create a new directory as the build directory, and let cmake configure
and generate the build files. When asked for the build system type,
select `CodeBlocks - MinGW Makefiles`. You can then either open the
Code::Blocks project that has been generated in the build directory,
and use this IDE to perform the build process, or you open a command line,
navigate to that folder, and call `cmake --build .` To speed up the process,
you can add `-- -j{N}`, where `{N}` is the number of cores you want to use.

Note that MingW g++ appears to generate faulty code when using maximum `-O3`
optimization. When building in release mode, be sure to set `CMAKE_CXX_FLAGS_RELEASE`
to `-O2` to be safe. 

### Visual Studio
Currently untested, but in principle CMake should be able to generate Visual Studio
project files that can be used. If you get it working, document your steps here and
submit a PR.

## Doxygen (documentation)

To build the Doxygen docs, you need to have Doxygen and Graphviz
installed. (They are otherwise optional.) Configure as usual, and say

```sh
cmake --build . --target doxygen
```

to build the documentation. To read it, open `docs/html/index.html` in
your favourite browser.
