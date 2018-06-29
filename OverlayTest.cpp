// This file is public domain software.
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>

HICON GetShortcutIcon(INT iIcon, INT cx, INT cy, COLORREF crMask = RGB(255, 0, 255))
{
    const INT iShortcut = 30;
    HDC hDC;
    HICON hNewIcon = NULL;
    HMODULE hMod = GetModuleHandle(NULL);
    HICON hIcon = (HICON)LoadImage(hMod, MAKEINTRESOURCE(iIcon), IMAGE_ICON, cx, cy, 0);
    HICON hShortcut = (HICON)LoadImage(hMod, MAKEINTRESOURCE(iShortcut), IMAGE_ICON, cx, cy, 0);
    HIMAGELIST himl = ImageList_Create(cx, cy, ILC_COLOR32 | ILC_MASK, 1, 1);

    if (!hIcon || !hShortcut || !himl)
        goto skip;

    hDC = CreateCompatibleDC(NULL);
    if (hDC)
    {
        // create 32bpp bitmap
        BITMAPINFO bi;
        ZeroMemory(&bi, sizeof(bi));
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth = cx;
        bi.bmiHeader.biHeight = cy;
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        LPVOID pvBits;
        HBITMAP hbm = CreateDIBSection(hDC, &bi, DIB_RGB_COLORS, &pvBits, NULL, 0);
        if (hbm)
        {
            // draw the icon image
            HGDIOBJ hbmOld = SelectObject(hDC, hbm);
            {
                HBRUSH hbr = CreateSolidBrush(crMask);
                RECT rc = { 0, 0, cx, cy };
                FillRect(hDC, &rc, hbr);
                DeleteObject(hbr);

                DrawIconEx(hDC, 0, 0, hIcon, cx, cy, 0, NULL, DI_NORMAL);
                DrawIconEx(hDC, 0, 0, hShortcut, cx, cy, 0, NULL, DI_NORMAL);
            }
            SelectObject(hDC, hbmOld);

            INT iAdded = ImageList_AddMasked(himl, hbm, crMask);
            hNewIcon = ImageList_GetIcon(himl, iAdded, ILD_NORMAL | ILD_TRANSPARENT);

            DeleteObject(hbm);
        }
        DeleteDC(hDC);
    }

skip:
    DestroyIcon(hIcon);
    DestroyIcon(hShortcut);
    ImageList_Destroy(himl);

    return hNewIcon;
}

HICON g_hIcon = NULL;

BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    INT cx = GetSystemMetrics(SM_CXICON);
    INT cy = GetSystemMetrics(SM_CYICON);
    g_hIcon = GetShortcutIcon(22, cx, cy);
    SendDlgItemMessage(hwnd, ico1, STM_SETICON, (WPARAM)g_hIcon, 0);
    return TRUE;
}

void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    switch (id)
    {
    case IDOK:
        DestroyIcon(g_hIcon);
        EndDialog(hwnd, IDOK);
        break;
    case IDCANCEL:
        DestroyIcon(g_hIcon);
        EndDialog(hwnd, IDCANCEL);
        break;
    }
}

INT_PTR CALLBACK
DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        HANDLE_MSG(hwnd, WM_INITDIALOG, OnInitDialog);
        HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
    }
    return 0;
}

INT WINAPI
WinMain(HINSTANCE   hInstance,
        HINSTANCE   hPrevInstance,
        LPSTR       lpCmdLine,
        INT         nCmdShow)
{
    DialogBox(hInstance, MAKEINTRESOURCE(1), NULL, DialogProc);
}
