#include "App.h"
#include <sstream>
#include <iomanip>
#include <tuple>
#include "Box.h"
#include "Melon.h"
#include "Pyramid.h"
#include <memory>
#include <algorithm>
#include "ChiliMath.h"

App::App()
	: window(800,600,"Fish Window")
{
	/* const auto fun1 = [](std::string Message) {
		std::tuple<std::string, float, float> tmpTuple;
		if (Message.size() > 0)
		{
			tmpTuple = std::make_tuple("hello", 1.0f, 2.0f);
		}
		else
		{
			tmpTuple = std::make_tuple("hello", 0.f, 0.0f);
		}

		return tmpTuple;
	};

	std::tuple<std::string, float, float> tuple1 	=  fun1("hello");
	const auto n1 = std::get<0>(tuple1);
	const auto n2 = std::get<1>(tuple1);*/

	class Factory
	{
	public:
		Factory(Graphics& gfx)
			:
			gfx(gfx)
		{}
		std::unique_ptr<Drawable> operator()()
		{
			switch (typedist(rng))
			{
			case 0:
				return std::make_unique<Pyramid>(
					gfx, rng, adist, ddist,
					odist, rdist
					);
			case 1:
				return std::make_unique<Box>(
					gfx, rng, adist, ddist,
					odist, rdist, bdist
					);
			case 2:
				return std::make_unique<Melon>(
					gfx, rng, adist, ddist,
					odist, rdist, longdist, latdist
					);
			default:
				assert(false && "bad drawable type in factory");
				return {};
			}
		}
	private:
		Graphics& gfx;
		std::mt19937 rng{ std::random_device{}() };
		std::uniform_real_distribution<float> adist{ 0.0f,PI * 2.0f };
		std::uniform_real_distribution<float> ddist{ 0.0f,PI * 0.5f };
		std::uniform_real_distribution<float> odist{ 0.0f,PI * 0.08f };
		std::uniform_real_distribution<float> rdist{ 6.0f,20.0f };
		std::uniform_real_distribution<float> bdist{ 0.4f,3.0f };
		std::uniform_int_distribution<int> latdist{ 5,20 };
		std::uniform_int_distribution<int> longdist{ 10,40 };
		std::uniform_int_distribution<int> typedist{ 0,2 };
	};

	Factory f(window.GFX());
	drawables.reserve(nDrawables);
	std::generate_n(std::back_inserter(drawables), nDrawables, f);

	window.GFX().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));

}

int App::Go()
{
	while (true)
	{
		if (const auto e = Window::ProcessWindowMessage())
		{
			return *e;
		}
		DoFrame();
	}	
}

App::~App()
{

}

void App::DoFrame()
{
	/*std::ostringstream oss;
	const float time = timer.Peek();
	oss <<std::setprecision(1)<<std::fixed << time <<"s";
	window.SetTitle(oss.str());*/
	/*const float  c = sin(timer.Peek()) / 2.f + 0.5f;
	const float mouseX = (float)window.mouse.GetPosX();
	const float mouseY = (float)window.mouse.GetPosY();
	window.GFX().ClearBuffer(c, c, 0.f);

	window.GFX().DrawTestTriangle(-timer.Peek(),
		0.f,0.f);

	window.GFX().DrawTestTriangle(timer.Peek(),
		mouseX / 400.f - 1.f,
		-1.f * mouseY / 300.f + 1.f);*/


	const auto dt = timer.Mark();
	window.GFX().ClearBuffer(0.07f, 0.0f, 0.12f);
	for (auto& d : drawables)
	{
		d->Update(dt);
		d->Draw(window.GFX());
	}
	window.GFX().EndFrame();
	
}
