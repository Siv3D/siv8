//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <mutex>
# include <Siv3D/Mouse/IMouse.hpp>
# include <Siv3D/GLFW/GLFW.hpp>

namespace s3d
{
	class CMouse final : public ISiv3DMouse
	{
	public:

		CMouse() = default;

		~CMouse() override;

		void init() override;

		void update() override;

		InputState& getInputState(uint32 index) noexcept override;

		const Array<Input>& getAllInput() const noexcept override;

		Vec2 wheel() const noexcept override;

		void onMouseButtonUpdated(int32 index, bool pressed) override;

		void onScroll(double x, double y) override;

		Optional<Point> getPrimaryTouchPos() override;

	private:
		
		GLFWwindow* m_window = nullptr;
		
		////////////////////////////////////////////////////////////////
		//
		std::mutex m_wheelMutex;

		Vec2 m_wheelInternal{ 0.0, 0.0 };
		//
		////////////////////////////////////////////////////////////////

		Vec2 m_wheel{ 0.0, 0.0 };
		
		static void OnMouseButtonUpdated(GLFWwindow*, int button, int action, int mods);

		static void OnScroll(GLFWwindow*, double xOffset, double yOffset);
	};
}
