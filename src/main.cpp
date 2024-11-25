#include <iostream>
#include <cstdlib>
#include <cmath>
#include <ctime>

#ifdef _WIN32
#include <windows.h>
#elif __linux__
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#endif

const int WIDTH = 800;
const int HEIGHT = 600;
const int MAX_ITER = 500;

double centerX = 0.0, centerY = 0.0;
double scale = 4.0;

int computeMandelbrot(double x, double y) {
    double zx = 0.0, zy = 0.0;
    int iter = 0;
    while (zx * zx + zy * zy < 4.0 && iter < MAX_ITER) {
        double temp = zx * zx - zy * zy + x;
        zy = 2.0 * zx * zy + y;
        zx = temp;
        iter++;
    }
    return iter;
}

unsigned long getRandomColor() {
    return rand() % 0xFFFFFF;
}

#ifdef _WIN32
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void drawMandelbrot(HDC hdc) {
    srand(static_cast<unsigned>(time(nullptr)));
    for (int py = 0; py < HEIGHT; ++py) {
        for (int px = 0; px < WIDTH; ++px) {
            double x0 = centerX + (px - WIDTH / 2.0) * scale / WIDTH;
            double y0 = centerY + (py - HEIGHT / 2.0) * scale / HEIGHT;
            int iter = computeMandelbrot(x0, y0);
            unsigned long color = iter == MAX_ITER ? 0 : getRandomColor();
            SetPixel(hdc, px, py, RGB((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF));
        }
    }
}

int main() {
    HINSTANCE hInstance = GetModuleHandle(nullptr);
    WNDCLASS wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "MandelbrotClass";
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, "MandelbrotClass", "Fractalis | Mandel-brot Set Pattern", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, WIDTH, HEIGHT, nullptr, nullptr, hInstance, nullptr);

    if (!hwnd) return -1;

    ShowWindow(hwnd, SW_SHOW);
    HDC hdc = GetDC(hwnd);

    bool redraw = true;
    MSG msg = { };

    while (true) {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) return 0;
            if (msg.message == WM_KEYDOWN) {
                if (msg.wParam == VK_UP) centerY -= scale * 0.1;
                if (msg.wParam == VK_DOWN) centerY += scale * 0.1;
                if (msg.wParam == VK_LEFT) centerX -= scale * 0.1;
                if (msg.wParam == VK_RIGHT) centerX += scale * 0.1;
                if (msg.wParam == VK_ADD) scale /= 1.5; // Zoom in
                if (msg.wParam == VK_SUBTRACT) scale *= 1.5; // Zoom out
                redraw = true;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (redraw) {
            drawMandelbrot(hdc);
            redraw = false;
        }
    }

    ReleaseDC(hwnd, hdc);
    return 0;
}
#elif __linux__
void drawMandelbrot(Display* display, Window window, GC gc) {
    srand(static_cast<unsigned>(time(nullptr)));
    for (int py = 0; py < HEIGHT; ++py) {
        for (int px = 0; px < WIDTH; ++px) {
            double x0 = centerX + (px - WIDTH / 2.0) * scale / WIDTH;
            double y0 = centerY + (py - HEIGHT / 2.0) * scale / HEIGHT;
            int iter = computeMandelbrot(x0, y0);
            unsigned long color = iter == MAX_ITER ? 0 : getRandomColor();
            XSetForeground(display, gc, color);
            XDrawPoint(display, window, gc, px, py);
        }
    }
}

int main() {
    Display* display = XOpenDisplay(nullptr);
    if (!display) {
        std::cerr << "Unable to open X display" << std::endl;
        return -1;
    }

    int screen = DefaultScreen(display);
    Window window = XCreateSimpleWindow(display, RootWindow(display, screen),
        0, 0, WIDTH, HEIGHT, 1, BlackPixel(display, screen), WhitePixel(display, screen));

    XSelectInput(display, window, ExposureMask | KeyPressMask);
    XMapWindow(display, window);

    GC gc = XCreateGC(display, window, 0, nullptr);

    XEvent event;
    while (true) {
        XNextEvent(display, &event);
        if (event.type == Expose) {
            drawMandelbrot(display, window, gc);
        } else if (event.type == KeyPress) {
            KeySym key = XLookupKeysym(&event.xkey, 0);
            if (key == XK_Up) centerY -= scale * 0.1;
            if (key == XK_Down) centerY += scale * 0.1;
            if (key == XK_Left) centerX -= scale * 0.1;
            if (key == XK_Right) centerX += scale * 0.1;
            if (key == XK_plus) scale /= 1.5; // Zoom in
            if (key == XK_minus) scale *= 1.5; // Zoom out
            if (key == XK_q) break; // Quit
            drawMandelbrot(display, window, gc);
        }
    }

    XFreeGC(display, gc);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
    return 0;
}
#endif
