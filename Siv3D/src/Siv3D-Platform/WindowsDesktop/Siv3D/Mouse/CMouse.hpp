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
# include <Siv3D/Array.hpp>
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

		bool update() override;

		Vec2 wheel() const noexcept override;

		void onMouseButtonUpdated(int32 index, bool pressed) override;

		void onScroll(double x, double y) override;

		Optional<Point> getPrimaryTouchPos() override;

		void onTouchInput(const Array<TOUCHINPUT>& touchInputs);

	private:
		
		HWND m_hWnd = nullptr;
	
		bool m_touchAvailable = false;

		////////////////////////////////////////////////////////////////
		//
		std::mutex m_wheelMutex;

		Vec2 m_wheelInternal{ 0.0, 0.0 };
		//
		////////////////////////////////////////////////////////////////

		Vec2 m_wheel{ 0.0, 0.0 };

		////////////////////////////////////////////////////////////////
		//
		std::mutex m_touchMutex;

		Optional<DWORD> m_currentPrimaryTouchID;

		Optional<Point> m_primaryTouchScreenPos;
		//
		////////////////////////////////////////////////////////////////
	};
}
