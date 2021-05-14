#include "Engine/Config/Bootstrap.h"

#include "Engine/Framework/System/SystemWindow.h"

#if defined(HD_PLATFORM_WIN64)

#include "Engine/Debug/Assert.h"
#include "Engine/Framework/Memory/AllocationScope.h"
#include "Engine/Framework/Memory/FrameworkMemoryInterface.h"
#include "Engine/Framework/String/String.h"
#include "Engine/Framework/System/SystemCommands.h"
#include "Engine/Framework/Utils/CommandBuffer.h"

namespace hd
{
    namespace sys
    {
        wchar_t const* g_ClassName = L"DroidEngineWindowClass";

        SystemWindowPlatform::SystemWindowPlatform(char8_t const* title, uint32_t width, uint32_t height)
            : m_Handle{}
            , m_Params{}
        {
            m_Params.Width = width;
            m_Params.Height = height;

            mem::AllocationScope scratchScope(mem::GetScratchAllocator());

            str::String titleString(scratchScope, title);

            HMODULE moduleHandle = ::GetModuleHandleW(nullptr);
            WNDCLASSEXW windowClass{};
            BOOL windowClassRegistered = ::GetClassInfoExW(moduleHandle, g_ClassName, &windowClass);

            if (!windowClassRegistered)
            {
                WNDCLASS windowClass{};

                windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
                windowClass.lpfnWndProc = static_cast<WNDPROC>(HandleMessageInternal);
                windowClass.cbClsExtra = 0;
                windowClass.cbWndExtra = 0;
                windowClass.hInstance = moduleHandle;
                windowClass.hIcon = ::LoadIconW(nullptr, IDI_WINLOGO);
                windowClass.hCursor = ::LoadCursorW(nullptr, IDC_ARROW);
                windowClass.hbrBackground = nullptr;
                windowClass.lpszMenuName = nullptr;
                windowClass.lpszClassName = g_ClassName;
                ATOM classAtom = ::RegisterClassW(&windowClass);
                hdEnsure(classAtom != 0, u8"Cannot register window class <%>.", g_ClassName);
            }

            DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
            RECT windowRect = { 0, 0, static_cast<int32_t>(width), static_cast<int32_t>(height) };
            ::AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);
            m_Handle = ::CreateWindowExW(
                dwExStyle,
                g_ClassName,
                titleString.AsWide(scratchScope),
                WS_OVERLAPPEDWINDOW,
                CW_USEDEFAULT,
                0,
                windowRect.right - windowRect.left,
                windowRect.bottom - windowRect.top,
                nullptr,
                nullptr,
                moduleHandle,
                &m_Params);

            hdCheckLastError(m_Handle != nullptr, u8"Cannot create Win32 window.");

            ::ShowWindow(m_Handle, SW_HIDE);
            ::UpdateWindow(m_Handle);
        }

        SystemWindowPlatform::~SystemWindowPlatform()
        {
            ::SetWindowLongPtrW(m_Handle, GWLP_USERDATA, 0);
            ::DestroyWindow(m_Handle);
        }

        HWND SystemWindowPlatform::GetNativeHandle() const
        {
            return m_Handle;
        }

        void SystemWindow::SetVisible(bool value)
        {
            ::ShowWindow(m_Handle, value ? SW_SHOW : SW_HIDE);
            ::UpdateWindow(m_Handle);
        }

        void SystemWindow::ProcessSystemEvents(util::CommandBuffer& systemCommands)
        {
            m_Params.Commands = &systemCommands;

            MSG message{ nullptr };
            while (::PeekMessageW(&message, m_Handle, 0, 0, PM_REMOVE))
            {
                ::TranslateMessage(&message);
                ::DispatchMessageW(&message);
            }

            m_Params.Commands = nullptr;
        }

        uint32_t const& SystemWindow::GetWidth()
        {
            return m_Params.Width;
        }

        uint32_t const& SystemWindow::GetHeight()
        {
            return m_Params.Height;
        }

        LRESULT CALLBACK SystemWindowPlatform::HandleMessageInternal(HWND windowHandle, UINT messageCode, WPARAM wParameter, LPARAM lParameter)
        {
            if (messageCode == WM_CREATE)
            {
                ::SetWindowLongPtrW(windowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(reinterpret_cast<CREATESTRUCT*>(lParameter)->lpCreateParams));
                return 0;
            }

            SystemWindow::WindowParameters* params = reinterpret_cast<SystemWindow::WindowParameters*>(::GetWindowLongPtr(windowHandle, GWLP_USERDATA));
            if (params == nullptr || params->Commands == nullptr)
            {
                return ::DefWindowProcW(windowHandle, messageCode, wParameter, lParameter);
            }

            switch (messageCode)
            {
            case WM_ACTIVATE:
            {
                WindowActivateCommand& command = WindowActivateCommand::WriteTo(*params->Commands);
                command.Active = !(LOWORD(wParameter) == WA_INACTIVE);

                return 0;
            }

            case WM_SIZE:
            {
                auto newClientWidth{ LOWORD(lParameter) };
                auto newClientHeight{ HIWORD(lParameter) };

                if (wParameter == SIZE_MINIMIZED)
                {
                    params->IsMinimized = true;

                    WindowActivateCommand& command = WindowActivateCommand::WriteTo(*params->Commands);
                    command.Active = false;
                }
                else if (wParameter == SIZE_MAXIMIZED)
                {
                    if (params->IsMinimized)
                    {
                        params->IsMinimized = false;

                        WindowActivateCommand& command = WindowActivateCommand::WriteTo(*params->Commands);
                        command.Active = true;
                    }

                    WindowResizeCommand& command = WindowResizeCommand::WriteTo(*params->Commands);
                    command.Width = newClientWidth;
                    command.Height = newClientHeight;

                    params->Width = newClientWidth;
                    params->Height = newClientHeight;
                }
                else if (wParameter == SIZE_RESTORED)
                {
                    if (params->IsResizing)
                    {
                        params->StoredWidth = newClientWidth;
                        params->StoredHeight = newClientHeight;
                        params->WasResized = true;
                    }
                    else
                    {
                        if (params->IsMinimized)
                        {
                            params->IsMinimized = false;

                            WindowActivateCommand& command = WindowActivateCommand::WriteTo(*params->Commands);
                            command.Active = true;
                        }
                        else
                        {
                            WindowResizeCommand& command = WindowResizeCommand::WriteTo(*params->Commands);
                            command.Width = newClientWidth;
                            command.Height = newClientHeight;

                            params->Width = newClientWidth;
                            params->Height = newClientHeight;
                        }
                    }
                }
                return 0;
            }
            case WM_ENTERSIZEMOVE:
            {
                params->IsResizing = true;
                return 0;
            }

            case WM_EXITSIZEMOVE:
            {
                params->IsResizing = false;
                if (params->WasResized)
                {
                    params->WasResized = false;

                    WindowResizeCommand& command = WindowResizeCommand::WriteTo(*params->Commands);
                    command.Width = params->StoredWidth;
                    command.Height = params->StoredHeight;
                }
                return 0;
            }

            case WM_CLOSE:
            {
                WindowClosedCommand::WriteTo(*params->Commands);
                return 0;
            }

            case WM_MENUCHAR:
            {
                return MAKELRESULT(0, MNC_CLOSE);
            }

            case WM_GETMINMAXINFO:
            {
                reinterpret_cast<MINMAXINFO*>(lParameter)->ptMinTrackSize.x = 200;
                reinterpret_cast<MINMAXINFO*>(lParameter)->ptMinTrackSize.y = 200;
                return 0;
            }

            case WM_LBUTTONDOWN:
            {
                MouseButtonCommand& command = MouseButtonCommand::WriteTo(*params->Commands);
                command.X = GET_X_LPARAM(lParameter);
                command.Y = GET_Y_LPARAM(lParameter);
                command.ButtonID = 0;
                command.Pressed = true;
                return 0;
            }

            case WM_MBUTTONDOWN:
            {
                MouseButtonCommand& command = MouseButtonCommand::WriteTo(*params->Commands);
                command.X = GET_X_LPARAM(lParameter);
                command.Y = GET_Y_LPARAM(lParameter);
                command.ButtonID = 1;
                command.Pressed = true;
                return 0;
            }

            case WM_RBUTTONDOWN:
            {
                MouseButtonCommand& command = MouseButtonCommand::WriteTo(*params->Commands);
                command.X = GET_X_LPARAM(lParameter);
                command.Y = GET_Y_LPARAM(lParameter);
                command.ButtonID = 2;
                command.Pressed = true;
                return 0;
            }

            case WM_LBUTTONUP:
            {
                MouseButtonCommand& command = MouseButtonCommand::WriteTo(*params->Commands);
                command.X = GET_X_LPARAM(lParameter);
                command.Y = GET_Y_LPARAM(lParameter);
                command.ButtonID = 0;
                command.Pressed = false;
                return 0;
            }

            case WM_MBUTTONUP:
            {
                MouseButtonCommand& command = MouseButtonCommand::WriteTo(*params->Commands);
                command.X = GET_X_LPARAM(lParameter);
                command.Y = GET_Y_LPARAM(lParameter);
                command.ButtonID = 1;
                command.Pressed = false;
                return 0;
            }

            case WM_RBUTTONUP:
            {
                MouseButtonCommand& command = MouseButtonCommand::WriteTo(*params->Commands);
                command.X = GET_X_LPARAM(lParameter);
                command.Y = GET_Y_LPARAM(lParameter);
                command.ButtonID = 2;
                command.Pressed = false;
                return 0;
            }

            case WM_MOUSEMOVE:
            {
                MouseMoveCommand& command = MouseMoveCommand::WriteTo(*params->Commands);
                command.X = GET_X_LPARAM(lParameter);
                command.Y = GET_Y_LPARAM(lParameter);
                return 0;
            }

            case WM_MOUSEWHEEL:
            {
                MouseWheelCommand& command = MouseWheelCommand::WriteTo(*params->Commands);
                command.X = GET_X_LPARAM(lParameter);
                command.Y = GET_Y_LPARAM(lParameter);
                command.Offset = GET_WHEEL_DELTA_WPARAM(wParameter) / 120;
                return 0;
            }

            case WM_KEYDOWN:
            {
                if ((lParameter & (1 << 30)) == 0)
                {
                    KeyboardCommand& command = KeyboardCommand::WriteTo(*params->Commands);
                    command.KeyID = static_cast<uint8_t>(wParameter);
                    command.Pressed = true;
                }
                return 0;
            }

            case WM_KEYUP:
            {
                KeyboardCommand& command = KeyboardCommand::WriteTo(*params->Commands);
                command.KeyID = static_cast<uint8_t>(wParameter);
                command.Pressed = false;
                return 0;
            }

            default:
            {
                return ::DefWindowProcW(windowHandle, messageCode, wParameter, lParameter);
            }
            }
        }
    }
}

#endif