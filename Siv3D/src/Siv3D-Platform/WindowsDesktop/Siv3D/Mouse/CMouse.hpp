//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <array>
# include <mutex>
# include <Siv3D/Array.hpp>
# include <Siv3D/Mouse.hpp>
# include <Siv3D/InputState.hpp>
# include <Siv3D/Mouse/IMouse.hpp>
# include <Siv3D/Windows/Windows.hpp>

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

		void onScroll(double x, double y) override;

		Optional<Point> getPrimaryTouchPos() override;

		void onTouchInput(const Array<TOUCHINPUT>& touchInputs);

	private:
		
		HWND m_hWnd = nullptr;
	
		bool m_touchAvailable = false;

		struct MouseButton
		{
			std::array<InputState, Mouse::NumButtons> states;

			Array<Input> allInputs;
		
		} m_mouseButton;

		struct Wheel
		{
			////////////////////////////////////////////////////////////////
			//
			std::mutex mutex;

			Vec2 wheelInternal{ 0.0, 0.0 };
			//
			////////////////////////////////////////////////////////////////

			Vec2 wheel{ 0.0, 0.0 };
		
		} m_wheel;

		struct Touch
		{
			std::mutex mutex;

			Optional<DWORD> currentPrimaryTouchID;

			Optional<Point> primaryTouchScreenPos;

		} m_touch;
	};
}
