// Fractalis - src/main.cpp

// Main base libs
#include <iostream>
#include <cmath>
#include <ctime>

// Platform specific libs
#ifdef _WIN32
#include <windows.h>
#elif __linux__
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif

// Variable values
const int WIDTH = 800;
const int HEIGHT = 600;
const int MAX_ITER = 1000;

double centerX = -0.5, centerY = 0.0;
double scale = 3.0;
double zoomFactor = 1.0;
double zoomSpeed = 0.98;
double animationTime = 0.0;
double animationSpeed = 0.01;

// Random colour
unsigned long getRandomColor() {
    return (rand() % 256) << 16 | (rand() % 256) << 8 | (rand() % 256);
}

// Makes the Mandelbrot
int computeMandelbrot(double x0, double y0) {
    double x = 0.0, y = 0.0;
    int iter = 0;
    while (x * x + y * y <= 4.0 && iter < MAX_ITER) {
        double xtemp = x * x - y * y + x0;
        y = 2.0 * x * y + y0;
        x = xtemp;
        iter++;
    }
    return iter;
}

// Windows specific code
#ifdef _WIN32

// Draws the Mandelbrot
void drawMandelbrot(HDC hdc) {
    animationTime += animationSpeed;
    zoomFactor *= zoomSpeed;

    centerX = 0.0 + sin(animationTime) * 0.5;
    centerY = 0.0 + cos(animationTime) * 0.5;

    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP memBitmap = CreateCompatibleBitmap(hdc, WIDTH, HEIGHT);
    SelectObject(memDC, memBitmap);

    unsigned long* pixels = new unsigned long[WIDTH * HEIGHT];

    for (int py = 0; py < HEIGHT; ++py) {
        for (int px = 0; px < WIDTH; ++px) {
            double x0 = centerX + (px - WIDTH / 2.0) * scale * zoomFactor / WIDTH;
            double y0 = centerY + (py - HEIGHT / 2.0) * scale * zoomFactor / HEIGHT;
            int iter = computeMandelbrot(x0, y0);
            unsigned long color = iter == MAX_ITER ? 0 : getRandomColor();
            pixels[py * WIDTH + px] = color;
        }
    }

    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = WIDTH;
    bi.bmiHeader.biHeight = HEIGHT;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    SetDIBits(memDC, memBitmap, 0, HEIGHT, pixels, &bi, DIB_RGB_COLORS);

    BitBlt(hdc, 0, 0, WIDTH, HEIGHT, memDC, 0, 0, SRCCOPY);

    delete[] pixels;
    DeleteObject(memBitmap);
    DeleteDC(memDC);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HDC hdc;
    switch (uMsg) {
    case WM_CREATE:
        hdc = GetDC(hwnd);
        break;
    case WM_PAINT:
        drawMandelbrot(hdc);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_SIZE:  // Prevent resizing by ignoring the message
        return 0;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

// Main function
int main() {

    // Creates a Window
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(0);
    wc.lpszClassName = "MandelbrotWindow"; // ANSI string
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, "MandelbrotWindow", "Fractalis | Mandelbrot Set",
        WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, WIDTH, HEIGHT, 0, 0, 0, 0);
    ShowWindow(hwnd, SW_SHOWNORMAL);
    
    // Prevent resizing
    SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_SIZEBOX);
    
    MSG msg;
    while (GetMessage(&msg, 0, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

// Linux specific code
#elif __linux__

// Draws the Mandelbrot
void drawMandelbrot(Display* display, Window window, GC gc) {
    animationTime += animationSpeed;
    zoomFactor *= zoomSpeed;

    centerX = 0.0 + sin(animationTime) * 0.5;
    centerY = 0.0 + cos(animationTime) * 0.5;

    XImage* image = XCreateImage(display, DefaultVisual(display, DefaultScreen(display)),
                                 DefaultDepth(display, DefaultScreen(display)), ZPixmap, 0,
                                 (char*)malloc(WIDTH * HEIGHT * 4), WIDTH, HEIGHT, 32, 0);

    unsigned long* pixels = (unsigned long*)image->data;

    for (int py = 0; py < HEIGHT; ++py) {
        for (int px = 0; px < WIDTH; ++px) {
            double x0 = centerX + (px - WIDTH / 2.0) * scale * zoomFactor / WIDTH;
            double y0 = centerY + (py - HEIGHT / 2.0) * scale * zoomFactor / HEIGHT;
            int iter = computeMandelbrot(x0, y0);
            unsigned long color = iter == MAX_ITER ? 0 : getRandomColor();
            pixels[py * WIDTH + px] = color;
        }
    }

    XPutImage(display, window, gc, image, 0, 0, 0, 0, WIDTH, HEIGHT);
    XFlush(display);
    free(image->data);
}

int main() {

    // Creates window
    Display* display = XOpenDisplay(NULL);
    if (!display) {
        std::cerr << "Unable to open X display." << std::endl;
        return 1;
    }

    Window root = DefaultRootWindow(display);
    
    // Create a window with fixed size
    XSetWindowAttributes attrs;
    attrs.override_redirect = True;  // Disable the window manager controls
    attrs.border_pixel = 0;
    attrs.background_pixel = WhitePixel(display, 0);

    Window window = XCreateWindow(display, root, 0, 0, WIDTH, HEIGHT, 0, CopyFromParent, CopyFromParent, CopyFromParent, CWOverrideRedirect, &attrs);
    XMapWindow(display, window);
    XSelectInput(display, window, ExposureMask | KeyPressMask);

    GC gc = XCreateGC(display, window, 0, NULL);
    XEvent event;

    while (true) {
        XNextEvent(display, &event);
        if (event.type == Expose) {
            drawMandelbrot(display, window, gc);
        } else if (event.type == KeyPress) {
            break;
        }
    }

    XFreeGC(display, gc);
    XCloseDisplay(display);
    return 0;
}
#endif
