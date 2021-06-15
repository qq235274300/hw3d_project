
#include "Window.h"
#include "WindowsMessageMap.h"
#include <sstream>
#include "App.h"




int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{

	try
	{
		//Window window(1000, 1000, "Fish Window");
		////Message Pump
		//MSG message;
		//BOOL MResult;
		//while ((MResult = GetMessage(&message, nullptr, 0, 0)) > 0)
		//{
		//	TranslateMessage(&message);
		//	DispatchMessage(&message);
		//	
		//	while (!window.mouse.IsEmpty())
		//	{
		//		const auto e = window.mouse.Read();
		//		if (e.GetType() == Mouse::Event::Type::Move)
		//		{
		//			std::ostringstream oss;
		//			oss << "MousePos : ("<<e.GetPosX()<<","<<e.GetPosY()<<")";
		//			window.SetTitle(oss.str());
		//		}
		//	}
		//	if (window.kbd.KeyIsPressed(VK_MENU))
		//	{
		//		MessageBox(nullptr, "Someshit Happend!", "Space Key was Pressed", MB_OK | MB_ICONEXCLAMATION);
		//	}
		//};

		//if (MResult == -1)
		//{
		//	return -1;
		//}
		//
		//return message.wParam;
		
		App{}.Go();

	}
	catch (const ChiliException& e)
	{
		MessageBox(nullptr,e.what(),e.GetType(),MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e)
	{
		MessageBox(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBox(nullptr, "No details available", "Unkonw Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	
	return -1;
}