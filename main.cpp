#include <windows.h>
#include <string>
#include <stdexcept>

// Global handle for the text area (edit control)
HWND g_hEditControl = NULL;

// Global variables for automated typing
int s_currentCharOffset = 0; // Offset from 'a' (0 for 'a', 1 for 'b', ..., 25 for 'z')
UINT_PTR s_timerId = 0;      // Timer ID for our typing simulation

// This function processes messages sent to a window.
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Function to start the typing simulation
void StartTyping(HWND hwndParent) {
    if (g_hEditControl == NULL) {
        return; // Edit control not created yet
    }

    s_currentCharOffset = 0; // Reset offset to 'a'
    SetFocus(g_hEditControl); // Give focus to the edit control

    // Set a timer to send a character every 1000 milliseconds (1 second)
    // The main window (hwndParent) will receive WM_TIMER messages
    s_timerId = SetTimer(hwndParent, 1, 1000, NULL); // Timer ID 1, 1000ms interval, no callback function
}

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
        L"C++ Windows Desktop App with Text Area",     // Window title text.
        WS_OVERLAPPEDWINDOW,            // Window style (standard title bar, border, min/max buttons).

        // Position. CW_USEDEFAULT lets Windows decide.
        CW_USEDEFAULT, CW_USEDEFAULT, 
        600, 400,   // width, height

        NULL,       // Parent window handle (NULL for a top-level window).
        NULL,       // Menu handle (NULL for no menu).
        hInstance,  // Instance handle of the application.
        NULL        // Additional application data (not used here).
    );

    if (hwnd == NULL) {
        MessageBox(NULL, L"Window Creation Failed!", L"Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    // Create the multiline text area (Edit Control).
    // This is a child window of the main window.
    g_hEditControl = CreateWindowEx(
        WS_EX_CLIENTEDGE, // Extended window style: sunken border
        L"EDIT",          // Predefined Windows class name for an edit control
        L"",              // Initial text
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | // Basic styles: child, visible, vertical scrollbar
        ES_MULTILINE | ES_AUTOVSCROLL | ES_WANTRETURN, // Edit control specific styles: multiline, auto vertical scroll, process Enter key
        10, 10,           // X, Y position relative to parent (main window)
        560, 340,         // Width, Height (initial size, will be adjusted on WM_SIZE)
        hwnd,             // Parent window handle
        (HMENU)1001,      // Control ID (unique identifier for this control, useful for WM_COMMAND)
        hInstance,        // Instance handle of the application
        NULL              // No additional creation parameters
    );

    // Check if edit control creation was successful.
    if (g_hEditControl == NULL) {
        MessageBox(NULL, L"Edit Control Creation Failed!", L"Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    StartTyping(hwnd);

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

    case WM_CREATE:
        break;

    case WM_SIZE: {
        // We need to resize the edit control to fill the client area.
        int newWidth = LOWORD(lParam);  // New width of the client area
        int newHeight = HIWORD(lParam); // New height of the client area

        // Adjust the position and size of the edit control
        // We'll leave a small margin (e.g., 10 pixels) around the text area
        const int margin = 10;
        if (g_hEditControl) {
            MoveWindow(g_hEditControl,
                margin,                   // X position
                margin,                   // Y position
                newWidth - (2 * margin),  // Width
                newHeight - (2 * margin), // Height
                TRUE);                    // Repaint the window
        }
        break;
    }

    case WM_TIMER:
        // Check if this is our typing timer
        if (wParam == s_timerId) {
            // Calculate the character to send: 'a' + (offset % 26)
            // This will cycle from 'a' to 'z' and then loop back.
            wchar_t charToSend = L'a' + (s_currentCharOffset % 26);
            std::wstring charStr(1, charToSend); // Convert char to a wstring for EM_REPLACESEL

            // Set the selection to the end of the current text
            SendMessage(g_hEditControl, EM_SETSEL, -1, -1);
            // Replace the (empty) selection with the new character (effectively appends)
            SendMessage(g_hEditControl, EM_REPLACESEL, 0, (LPARAM)charStr.c_str());
            // Scroll the caret into view (makes sure the text area scrolls)
            SendMessage(g_hEditControl, EM_SCROLLCARET, 0, 0);

            s_currentCharOffset++; // Move to the next character/offset
        }
        break;


    case WM_DESTROY:
        if (s_timerId != 0) {
            KillTimer(hwnd, s_timerId);
        }
        PostQuitMessage(0);
        return 0;


    case WM_PAINT: {
        // Sent when a window needs to be repainted (e.g., exposed, resized).
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps); // Prepares the specified window for painting.

        EndPaint(hwnd, &ps); // Ends window painting and releases the display device context.
        return 0; // Message handled.
    }

    // You can add more message handlers here as needed (e.g., WM_COMMAND for buttons, WM_LBUTTONDOWN for mouse clicks).

    default:
        // For any messages not explicitly handled, pass them to the default window procedure.
        // This ensures default processing for messages like resizing, moving, etc.
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}