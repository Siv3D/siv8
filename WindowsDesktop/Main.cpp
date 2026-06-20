# include <Siv3D.hpp> // Siv3D v0.8.0
# include "../Test/Siv3DTest.hpp"
//SIV3D_SET(EngineOption::D3D11Driver::WARP);

void Main()
{
	//RunTest();
	Window::Resize(1280, 720);
	Scene::SetBackground(ColorF{ 0.6, 0.8, 0.7 });
	//Window::SetStyle(WindowStyle::Sizable);
	//Scene::SetResizeMode(ResizeMode::Keep);


	while (System::Update())
	{
		Scene::Rect().draw(Pattern::Checker{ ColorF{ 0.12 }, ColorF{ 0.15 }, 40 });

		ScopedRenderStates2D rt{ KeySpace.pressed() ? RasterizerState::WireframeCullBack : RasterizerState::SolidCullBack };

		Circle{ 200, 140, 80 }.drawDashedFrame(8, { .dashCount = 8 });
		Circle{ 200, 360, 80 }.drawDashedFrame(8, { .dashCount = 16 });
		Circle{ 200, 580, 80 }.drawDashedFrame(8, { .dashCount = 32 });

		Circle{ 400, 140, 80 }.drawDashedFrame(4, { .dashCount = 8, .dashRatio = 0.25 }, ColorF{ 0.2, 1.0, 0.5 });
		Circle{ 400, 360, 80 }.drawDashedFrame(4, { .dashCount = 16, .dashRatio = 0.25 }, ColorF{ 0.2, 1.0, 0.5 });
		Circle{ 400, 580, 80 }.drawDashedFrame(4, { .dashCount = 32, .dashRatio = 0.25 }, ColorF{ 0.2, 1.0, 0.5 });

		Circle{ 600, 140, 80 }.drawDashedFrame(8, { .dashCount = 8, .dashRatio = 0.75 }, ColorF{ 0.2, 0.2, 0.8 });
		Circle{ 600, 360, 80 }.drawDashedFrame(8, { .dashCount = 16, .dashRatio = 0.75 }, ColorF{ 0.2, 0.2, 0.8 });
		Circle{ 600, 580, 80 }.drawDashedFrame(8, { .dashCount = 32, .dashRatio = 0.75 }, ColorF{ 0.2, 0.2, 0.8 });

		Circle{ 800, 140, 80 }.drawDashedFrame(8, { .dashCount = 2, .dashRatio = 0.5, .startAngle = 0_deg });
		Circle{ 800, 360, 80 }.drawDashedFrame(8, { .dashCount = 2, .dashRatio = 0.5, .startAngle = 30_deg });
		Circle{ 800, 580, 80 }.drawDashedFrame(8, { .dashCount = 2, .dashRatio = 0.5, .startAngle = 60_deg });

		Circle{ 1000, 140, 80 }.drawDashedFrame(32, 8, { .dashCount = 8, .dashRatio = 0.2 }); 
		Circle{ 1000, 360, 80 }.drawDashedFrame(40, 8, { .dashCount = 12, .dashRatio = 0.25 }, ColorF{ 1.0, 0.0, 0.0 }, ColorF{ 1.0, 1.0, 0.0 });
		Circle{ 1000, 580, 80 }.drawDashedFrame(24, 8, { .dashCount = 16, .dashRatio = 0.1 }, ColorF{ 1.0, 1.0, 0.0, 0.0 }, ColorF{ 1.0, 1.0, 0.0 });
	}
}
