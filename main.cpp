#include <windows.h>
#include <string>
#include <stdexcept>

// This function processes messages sent to a window.
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Entry Point of desktop application
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"CppSampleWindowClass";

    WNDCLASSEX wc = { 0 };

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, L"Window Class Registration Failed!", L"Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles (e.g., WS_EX_CLIENTEDGE).
        CLASS_NAME,                     
        L"C++ Windows Desktop App",     // Window title text.
        WS_OVERLAPPEDWINDOW,            // Window style (standard title bar, border, min/max buttons).

        // Size and position. CW_USEDEFAULT lets Windows decide.
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // Parent window handle (NULL for a top-level window).
        NULL,       // Menu handle (NULL for no menu).
        hInstance,  // Instance handle of the application.
        NULL        // Additional application data (not used here).
    );

    if (hwnd == NULL) {
        MessageBox(NULL, L"Window Creation Failed!", L"Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    // Display the window.
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd); // Ensures the window is painted immediately.

    // Run the message loop.
    // This loop retrieves and dispatches messages sent to the application's windows.
    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg); // Translates virtual-key messages into character messages.
        DispatchMessage(&msg);  // Dispatches a message to a window procedure.
    }

    return (int)msg.wParam;
}

// This function is called by the system to process messages for the window.
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_PAINT: {
        // Sent when a window needs to be repainted (e.g., exposed, resized).
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps); // Prepares the specified window for painting.

        // Define the text to display.
        std::wstring text = L"Hello, World! from C++";
        // Draw text onto the window's client area.
        // TextOut(HDC hdc, int x, int y, LPCWSTR lpString, int cchString)
        TextOut(hdc, 10, 10, text.c_str(), static_cast<int>(text.length()));

        EndPaint(hwnd, &ps); // Ends window painting and releases the display device context.
        return 0; // Message handled.
    }

    // You can add more message handlers here as needed (e.g., WM_COMMAND for buttons, WM_LBUTTONDOWN for mouse clicks).

    default:
        // For any messages not explicitly handled, pass them to the default window procedure.
        // This ensures default processing for messages like resizing, moving, etc.
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}