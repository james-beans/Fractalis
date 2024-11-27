# Fractalis
A window with a mandel-brot pattern inside it.

## Features:
- Cross-platform (Windows and Linux only)
- Animates automatically

## Issues:
- Not enough features to show off properly.

## Controls:
- No controls are fully supported yet. When I add support for commands I will put them here.

## Dependencies:
- Windows SDK (might be installed already but check even if)
- MinGW (for compiling using gcc and g++)
- [X11](https://en.wikipedia.org/wiki/X_Window_System) CPP Headers

## How to compile:

> [!WARNING]
> Wayland is way to complicated for me to do yet. It will only work with [X11](https://en.wikipedia.org/wiki/X_Window_System). I will not update it to work with Wayland; you will need to port if you want to use it with Wayland.

> [!WARNING]
> If using another thing other than [X11](https://en.wikipedia.org/wiki/X_Window_System) for windows to work on Linux (like Wayland) you will need to switch to [X11](https://en.wikipedia.org/wiki/X_Window_System) (this requires a restart in some cases) or port [the code](/src/main.cpp) to your desired platform for windows to work.

> [!TIP]
> If you want to compile by yourself, clone this repo, install the dependencies and run the build commands below:

- Windows:
    - Use the `g++ src/main.cpp -lgdi32 -mwindows -o dist/fractalis.exe` command.

- Linux:
    - Use the `g++ src/main.cpp -lX11 -o dist/fractalis` command with [X11](https://en.wikipedia.org/wiki/X_Window_System).
