#include "window.h"
/**********************************************************************
    class: Window (window_win32.cpp)

    author: S. Hau
    date: November 3, 2017

    Windows implementation.
**********************************************************************/

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <windowsx.h> /* GET_X_LPARAM, GET_Y_LPARAM */
#undef ERROR

#include "locator.h"

namespace Core { namespace OS {

    //----------------------------------------------------------------------
    HINSTANCE   hInstance;
    HWND        hwnd;

    static      Window* window = nullptr;

    //----------------------------------------------------------------------
    bool        RegisterWindowClass( HINSTANCE hInstance, LPCSTR className );
    HANDLE      LoadImageFromFile( const char* path, UINT type );
    KeyMod      GetMouseKeyMod( WPARAM wParam );
    Key         GetKey( WPARAM wParam );
    KeyMod      GetKeyKeyMod();

    //----------------------------------------------------------------------
    LRESULT CALLBACK WindowProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
    {
        switch ( uMsg )
        {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        //----------------------------------------------------------------------
        // Handle resize
        case WM_SIZE:
        {
            U16 w = LOWORD( lParam );
            U16 h = HIWORD( lParam );
            window->_SetSize( w, h );
            Window::_GetCallbackHelper().callSizeChangedCallback( w, h );
            return 0;
        }

        //----------------------------------------------------------------------
        // Handle Mouse Input
        case WM_LBUTTONDOWN:
            SetCapture( hwnd ); // This ensures, that the corresponding BUTTONUP message is generated, even when the mouse is out of the window
            Window::_GetCallbackHelper().callMouseButtonCallback( MouseKey::LButton, KeyAction::DOWN, GetMouseKeyMod( wParam ) );
            return 0;
        //----------------------------------------------------------------------
        case WM_LBUTTONUP:
            ReleaseCapture();
            Window::_GetCallbackHelper().callMouseButtonCallback( MouseKey::LButton, KeyAction::UP, GetMouseKeyMod( wParam ) );
            return 0;
        //----------------------------------------------------------------------
        case WM_MBUTTONDOWN:
            SetCapture( hwnd );
            Window::_GetCallbackHelper().callMouseButtonCallback( MouseKey::MButton, KeyAction::DOWN, GetMouseKeyMod( wParam ) );
            return 0;
        //----------------------------------------------------------------------
        case WM_MBUTTONUP:
            ReleaseCapture();
            Window::_GetCallbackHelper().callMouseButtonCallback( MouseKey::MButton, KeyAction::UP, GetMouseKeyMod( wParam ) );
            return 0;
        //----------------------------------------------------------------------
        case WM_RBUTTONDOWN:
            SetCapture( hwnd );
            Window::_GetCallbackHelper().callMouseButtonCallback( MouseKey::RButton, KeyAction::DOWN, GetMouseKeyMod( wParam ) );
            return 0;
        //----------------------------------------------------------------------
        case WM_RBUTTONUP:
            ReleaseCapture();
            Window::_GetCallbackHelper().callMouseButtonCallback( MouseKey::RButton, KeyAction::UP, GetMouseKeyMod( wParam ) );
            return 0;
        //----------------------------------------------------------------------
        case WM_MOUSEWHEEL:
            Window::_GetCallbackHelper().callMouseWheelCallback( GET_WHEEL_DELTA_WPARAM( wParam ) > 0 ? 1 : -1 );
            return 0;
        //----------------------------------------------------------------------
        case WM_MOUSEMOVE:
            Window::_GetCallbackHelper().callCursorCallback( GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ) );
            return 0;

        //----------------------------------------------------------------------
        // Handle Keyboard Input
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            // Bit 30: 1 if message was automatically generated from the OS
            Window::_GetCallbackHelper().callKeyCallback( GetKey( wParam ), lParam & (1 << 30) ? KeyAction::REPEAT : KeyAction::DOWN, GetKeyKeyMod() );
            return 0;
        case WM_KEYUP:
        case WM_SYSKEYUP:
            Window::_GetCallbackHelper().callKeyCallback( GetKey( wParam ), KeyAction::UP, 0 );
            return 0;
        case WM_CHAR:
        case WM_DEADCHAR:
            Window::_GetCallbackHelper().callCharCallback( (char)wParam );
            return 0;

        //----------------------------------------------------------------------
        case WM_SETFOCUS:
            Window::_GetCallbackHelper().callGainFocusCallback();
            return 0;
        case WM_KILLFOCUS:
            Window::_GetCallbackHelper().callLooseFocusCallback();
            return 0;

        //----------------------------------------------------------------------
        default:
            return DefWindowProc( hwnd, uMsg, wParam, lParam );
        }
    }

    //----------------------------------------------------------------------
    void Window::processOSMessages()
    {
        MSG msg;
        while ( PeekMessage( &msg, NULL, NULL, NULL, PM_REMOVE ) )
        {
            if ( msg.message == WM_QUIT )
                m_shouldBeClosed = true;

            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
    }

    //----------------------------------------------------------------------
    void Window::create( const char* title, U32 width, U32 height )
    {
        ASSERT( m_created == false );
        m_width  = width;
        m_height = height;
        window = this;

        LPCSTR className = "DXWNDClassName";
        if ( not RegisterWindowClass( hInstance, className ) )
        {
            ERROR( "Window[Win32]::create(): Failed to register a window class." );
        }

        UINT screenResX = GetSystemMetrics( SM_CXSCREEN );
        UINT screenResY = GetSystemMetrics( SM_CYSCREEN );

        UINT x = (UINT) ( screenResX * 0.5f - (width * 0.5f) );
        UINT y = (UINT) ( screenResY * 0.5f - (height * 0.5f) );

        hwnd = CreateWindow( className, title,
                             WS_OVERLAPPEDWINDOW, 
                             x, y, 
                             width, height, 
                             NULL, NULL, hInstance, NULL );

        if ( not hwnd )
        {
            ERROR( "Window[Win32]::create(): Failed to create a window. " );
        }

        ShowWindow( hwnd, SW_SHOW );
        m_created = true;
    }

    //----------------------------------------------------------------------
    void Window::destroy()
    {
        if ( not DestroyWindow( hwnd ) )
        {
            ERROR( "Window[Win32]::destroy(): Failed to destroy the window. " );
        }
        m_created = false;
    }

    //----------------------------------------------------------------------
    void Window::setBorderlessFullscreen(bool enabled)
    {
        static U32 oldWindowSizeX = 800;
        static U32 oldWindowSizeY = 600;

        LONG lStyle = GetWindowLong( hwnd, GWL_STYLE );
        U32 newWidth, newHeight;
        if (enabled)
        {
            // Change style to borderless
            lStyle &= ~( WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU );

            // Save old position in case of reverting
            RECT rect;
            GetWindowRect( hwnd, &rect );
            oldWindowSizeX = (rect.right - rect.left);
            oldWindowSizeY = (rect.bottom - rect.top);

            // Maximize window
            newWidth  = GetSystemMetrics( SM_CXSCREEN );
            newHeight = GetSystemMetrics( SM_CYSCREEN );
        }
        else
        {
            // Revert style
            lStyle |= ( WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU );

            // Revert to old size
            newWidth  = oldWindowSizeX;
            newHeight = oldWindowSizeY;
        }

        SetWindowLongPtr( hwnd, GWL_STYLE, lStyle );
        SetWindowPos( hwnd, NULL, 0, 0, newWidth, newHeight, SWP_NOZORDER | SWP_NOOWNERZORDER );
    }

    //----------------------------------------------------------------------
    void Window::center() const
    {
        UINT screenResX = GetSystemMetrics( SM_CXSCREEN );
        UINT screenResY = GetSystemMetrics( SM_CYSCREEN );

        UINT x = (UINT) ( ( screenResX * 0.5f ) - ( m_width  * 0.5f ) );
        UINT y = (UINT) ( ( screenResY * 0.5f ) - ( m_height * 0.5f ) );

        SetWindowPos( hwnd, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER );
    }

    //----------------------------------------------------------------------
    void Window::setTitle( const char* newTitle ) const
    {
        SetWindowText( hwnd, newTitle );
    }

    //----------------------------------------------------------------------
    void Window::setCursorPosition( I16 x, I16 y ) const
    {
        POINT point { (LONG) x, (LONG) y };
        ClientToScreen( hwnd, &point );
        SetCursorPos( point.x, point.y );
    }

    //----------------------------------------------------------------------
    Point2D Window::getCursorPosition() const
    {
        POINT p;
        GetCursorPos( &p );
        ScreenToClient( hwnd, &p );
        return Point2D{ (I16)p.x, (I16)p.y };
    }

    //----------------------------------------------------------------------
    void Window::centerCursor() const
    {
        I32 width  = GetSystemMetrics( SM_CXCURSOR );
        I32 height = GetSystemMetrics( SM_CYCURSOR );

        I32 centerX = (I32) ( ( m_width  * 0.5f ) - ( width  * 0.5f ) );
        I32 centerY = (I32) ( ( m_height * 0.5f ) - ( height * 0.5f ) );

        setCursorPosition( centerX, centerY );
    }

    //----------------------------------------------------------------------
    void Window::showCursor( bool b ) const
    {
        ShowCursor( b );
    }

    //----------------------------------------------------------------------
    void Window::setCursor( const Path& path ) const
    {
        HCURSOR cursor = (HCURSOR) LoadImageFromFile( path, IMAGE_CURSOR );
        if (cursor == NULL)
        {
           WARN( "Window[Win32]::setCursor(): Could not load cursor '" + path.toString() + "'. Wrong Extension? (.cur)." );
           return;
        }

        // Set the windows class cursor (otherwise WM_SETCURSOR must be properly handled)
        SetClassLongPtr( hwnd, GCLP_HCURSOR, (LONG_PTR) cursor );
    }

    //----------------------------------------------------------------------
    void Window::setIcon( const Path& path ) const
    {
        HICON icon = (HICON) LoadImageFromFile( path, IMAGE_ICON );
        if (icon == NULL)
        {
           WARN( "Window[Win32]::setIcon(): Could not load icon '" + path.toString() + "'. Wrong Extension? (.ico)." );
           return;
        }

        SetClassLongPtr( hwnd, GCLP_HICON, (LONG_PTR) icon );
        SetClassLongPtr( hwnd, GCLP_HICONSM, (LONG_PTR) icon );
    }

    //----------------------------------------------------------------------
    void PrintLastErrorAsString()
    {
        //Get the error message, if any
        DWORD errorMessageID = GetLastError();
        LPSTR messageBuffer = nullptr;
        Size size = FormatMessageA( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                    NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL );

        printf( "Last Win32 Error: %s", messageBuffer );

        // Free buffer
        LocalFree( messageBuffer );
    }

    //----------------------------------------------------------------------
    HANDLE LoadImageFromFile( const char* path, UINT type )
    {
        HANDLE img = LoadImage( NULL, path, type, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE );
        return img;
    }

    //----------------------------------------------------------------------
    bool RegisterWindowClass( HINSTANCE hInstance, LPCSTR className )
    {
        WNDCLASSEX lpWndClass;
        lpWndClass.cbSize = sizeof( WNDCLASSEX );
        lpWndClass.style = ( CS_HREDRAW | CS_VREDRAW );
        lpWndClass.lpfnWndProc = WindowProc;
        lpWndClass.cbClsExtra = 0;
        lpWndClass.cbWndExtra = 0;
        lpWndClass.hInstance = hInstance;
        lpWndClass.hIcon = LoadIcon( NULL, IDI_APPLICATION );
        lpWndClass.hCursor = LoadCursor( NULL, IDC_ARROW );
        lpWndClass.hbrBackground = NULL;
        lpWndClass.lpszMenuName = NULL;
        lpWndClass.lpszClassName = className;
        lpWndClass.hIconSm = LoadIcon( NULL, IDI_APPLICATION );

        return RegisterClassEx( &lpWndClass );
    }

    //----------------------------------------------------------------------
    KeyMod GetMouseKeyMod( WPARAM wParam )
    {
        KeyMod mod = KeyModBits::NONE;
        if ( wParam & MK_CONTROL )
            mod |= KeyModBits::CONTROL;
        if ( wParam & MK_SHIFT )
            mod |= KeyModBits::SHIFT;
        if ( GetKeyState( VK_MENU ) & 0x8000 )
            mod |= KeyModBits::ALT;
        return mod;
    }

    //----------------------------------------------------------------------
    KeyMod GetKeyKeyMod()
    {
        KeyMod mod = KeyModBits::NONE;

        // Higher Order bit is set when pressed, lower order bit when toggled
        if ( GetKeyState(VK_CONTROL) & 0x8000 )
            mod |= KeyModBits::CONTROL;
        if ( GetKeyState( VK_SHIFT ) & 0x8000 )
            mod |= KeyModBits::SHIFT;
        if ( GetKeyState( VK_MENU ) & 0x8000 )
            mod |= KeyModBits::ALT;
        return mod;
    }

    //----------------------------------------------------------------------
    Key GetKey( WPARAM wParam )
    {
        // Simple 1:1 Mapping of window keycodes
        return (Key) wParam;
    }

} } // end namespaces


#endif