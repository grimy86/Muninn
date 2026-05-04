#include "Overlay.h"
#include <windows.h>

#ifndef POS_OFFSET
#define POS_OFFSET 0x28 // replace with your actual offset
#endif // !POS_OFFSET

inline HWND g_overlay;
inline int g_screenW, g_screenH;

void CreateOverlay() {
    g_screenW = GetSystemMetrics(SM_CXSCREEN);
    g_screenH = GetSystemMetrics(SM_CYSCREEN);
    HINSTANCE hInstance = GetModuleHandleA(nullptr);

    g_overlay = CreateWindowExA(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_NOACTIVATE,
        "STATIC", nullptr, WS_POPUP | WS_VISIBLE,
        0, 0, g_screenW, g_screenH,
        nullptr, nullptr, hInstance, nullptr
    );

    SetLayeredWindowAttributes(g_overlay, RGB(0, 0, 0), 0, LWA_COLORKEY);
}

// Read enemy positions with RPM
Vec3 readPos(uintptr_t entityAddr) {
	return *(Vec3*)(entityAddr + POS_OFFSET);
}

// Draw the dot with GDI
void drawDot(HDC hdc, int x, int y) {
	HBRUSH brush = CreateSolidBrush(RGB(255, 0, 0));
	RECT r = { x - 4, y - 4, x + 4, y + 4 };
	FillRect(hdc, &r, brush);
	DeleteObject(brush);
}