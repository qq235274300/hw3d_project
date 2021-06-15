#pragma once

#include "ChiliWin.h"
#include "ChiliException.h"
#include "KeyBoard.h"
#include "Mouse.h"
#include <optional>
#include "Graphics.h"

class Window
{

public:
	class Exception : public ChiliException
	{
		using ChiliException::ChiliException;
	public:
		static std::string TranslateErrorCode(HRESULT hr) noexcept;
	};
	class HrException : public Exception
	{
	public:
		HrException(int line, const char* file, HRESULT hr) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorDescription() const noexcept;
	private:
		HRESULT hr;
	};
	class NoGfxException : public Exception
	{
	public:
		using Exception::Exception;
		const char* GetType() const noexcept override;
	};


private:
	class WindowClass
	{
	public:
		static const char* GetName()noexcept;
		static HINSTANCE GetInstance()noexcept;
	private:
		WindowClass()noexcept;
		~WindowClass();
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;
		
	private:
		static  constexpr const char*  WinClassName = "It is a Window";
		static WindowClass windowClass;
		HINSTANCE hInst;
	};

public:
	Window( int width, int height,const char* windowName);
	~Window();
	Window(const Window& wnd) = delete;
	Window& operator=(const Window& wnd) = delete;
	void SetTitle(const std::string& title);
	static std::optional<int> ProcessWindowMessage()noexcept;
	Graphics& GFX();
private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)noexcept;
public:
	KeyBoard kbd;
	Mouse mouse;
private:
	int width;
	int height;
	HWND windowHandle;
	std::unique_ptr<Graphics> pGfx;

};

