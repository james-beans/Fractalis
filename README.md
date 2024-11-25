# Fractalis
A window with a mandel-brot pattern inside it.

## Issues:
- Loads way to slow and doesn't scale correctly when fullscreen on Windows.

## Controls:
- **Arrow Keys**: Pan (up, down, left, right).
- `+/-` **or** Add/Subtract: Zoom in and out.
- `q` **(Linux)**: Quit the application.


## How to compile:

> [!WARNING]
> Wayland is way to complicated for me to do yet. It will only work with X11.

> [!TIP]
> If you want to compile by yourself, clone this repo and 

- Windows:
    - Use the `g++ src/main.cpp -lgdi32 -o dist/fractalis.exe` command.

- Linux:
    - Use the `g++ src/main.cpp -lX11 -o dist/fractalis` command with X11.
