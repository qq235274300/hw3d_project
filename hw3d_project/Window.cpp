#include "Window.h"
#include <sstream>
#include "resource.h"
#include "WindowThrowMacros.h"

Window::WindowClass Window::WindowClass::windowClass;


const char * Window::WindowClass::GetName() noexcept
{
	return WinClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return windowClass.hInst;
}

Window::WindowClass::WindowClass() noexcept
	:hInst(GetModuleHandle(nullptr))
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hIcon = static_cast<HICON>(LoadImage(GetInstance(),MAKEINTRESOURCE(IDI_ICON2),IMAGE_ICON,64,64,0));
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = GetName();
	wc.hIconSm = static_cast<HICON>(LoadImage(GetInstance(), MAKEINTRESOURCE(IDI_ICON2), IMAGE_ICON, 32, 32, 0));
	RegisterClassEx(&wc);
}

Window::WindowClass::~WindowClass() 
{
	UnregisterClass(GetName(), GetInstance());
}

Window::Window(int width, int height, const char* windowName) 
	:width(width),height(height)
{
	RECT ClientRect;
	ClientRect.left = 100;
	ClientRect.right = ClientRect.left + width;
	ClientRect.top = 100;
	ClientRect.bottom = ClientRect.top + height;
	if (AdjustWindowRect(&ClientRect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE) == 0)
	{
		throw CHWND_LAST_EXCEPT();
	};
	//TEST EXCEPTION
	//throw CHWND_EXCEPT(ERROR_ARENA_TRASHED);

	windowHandle = CreateWindow(WindowClass::GetName(), windowName, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT,
		ClientRect.right - ClientRect.left, ClientRect.bottom - ClientRect.top, nullptr, nullptr, WindowClass::GetInstance(), this);

	if (windowHandle == nullptr)
	{
		throw CHWND_LAST_EXCEPT();
	}

	ShowWindow(windowHandle, SW_SHOWDEFAULT);
	//Create GFX
	pGfx = std::make_unique<Graphics>(windowHandle);
	
}

void Window::SetTitle(const std::string & title)
{
	if (SetWindowText(windowHandle, title.c_str()) == 0)
	{
		throw CHWND_LAST_EXCEPT();
	}
}

std::optional<int> Window::ProcessWindowMessage()noexcept
{
	MSG message;
	while (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
	{
		if (message.message == WM_QUIT)
		{
			return  message.wParam;
		}
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
	return {};
}



Graphics& Window::GFX()
{
	if (!pGfx)
	{
		throw CHWND_NOGFX_EXCEPT();
	}
	return *pGfx;
}

LRESULT CALLBACK Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_NCCREATE)
	{
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&pWnd->HandleMsgThunk));
		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	switch (msg)
	{
	case  WM_CLOSE:
		PostQuitMessage(0);
		return 0;

	case  WM_KILLFOCUS:
		kbd.ClearState();
		break;


	//keyBoard Messgae
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if (!(lParam & 0x40000000) || kbd.AutoRepeatedIsEnabled())
		{
			kbd.OnKeyPressed(static_cast<unsigned char>(wParam));
		}
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		kbd.OnKeyReleased(static_cast<unsigned char>(wParam));
		break;
	case  WM_CHAR:
		kbd.OnChar(static_cast<unsigned char>(wParam));
		break;

	//Mouse Messgae
	case WM_MOUSEMOVE:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		if (pt.x >= 0 && pt.x < width && pt.y >= 0 && pt.y < height)
		{
			mouse.OnMouseMove(pt.x, pt.y);
			if (!mouse.IsInWindow())
			{
				SetCapture(hWnd);
				mouse.OnMouseEnter();
			}
		}
		else
		{
			if (wParam & (MK_LBUTTON | MK_RBUTTON))
			{
				mouse.OnMouseMove(pt.x, pt.y);
			}
			else
			{
				ReleaseCapture();
				mouse.OnMouseLeave();
			}
		}


		
		break;
	}
	case WM_LBUTTONDOWN:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLeftPressed(pt.x, pt.y);
		break;
	}
	case WM_RBUTTONDOWN:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnRightPressed(pt.x, pt.y);
		break;
	}
	case WM_LBUTTONUP:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLeftReleased(pt.x, pt.y);
		break;
	}

	case WM_RBUTTONUP:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnRightReleased(pt.x, pt.y);
		break;
	}

	case  WM_MOUSEWHEEL:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
		mouse.OnWheelDelta(pt.x, pt.y, delta);
		break;
	}

	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

Window::~Window()
{
	DestroyWindow(windowHandle);
}



std::string Window::Exception::TranslateErrorCode(HRESULT hr)noexcept
{
	char* pMsgBuf = nullptr;
	const DWORD nMsgLen = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&pMsgBuf), 0, nullptr
	);
	
	if (nMsgLen == 0)
	{
		return "Unidentified error code";
	}
	std::string errorString = pMsgBuf;
	LocalFree(pMsgBuf);
	return errorString;
}

Window::HrException::HrException(int line, const char* file, HRESULT hr) noexcept
	:
	Exception(line, file),
	hr(hr)
{}

const char* Window::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Window::HrException::GetType() const noexcept
{
	return "Chili Window Exception";
}


HRESULT Window::HrException::GetErrorCode() const noexcept
{
	return hr;
}

std::string Window::HrException::GetErrorDescription() const noexcept
{
	return Exception::TranslateErrorCode(hr);
}

const char* Window::NoGfxException::GetType() const noexcept
{
	return "Chili Window Exception [No Graphics]";
}