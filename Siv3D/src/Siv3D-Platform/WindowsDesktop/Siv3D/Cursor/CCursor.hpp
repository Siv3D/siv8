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
# include <Siv3D/Cursor/ICursor.hpp>
# include <Siv3D/Cursor/HighTemporalResolutionCursor.hpp>
# include <Siv3D/Windows/MinWindows.hpp>

namespace s3d
{
	class CCursor final : public ISiv3DCursor
	{
	public:

		CCursor() = default;

		~CCursor() override;

		void init() override;

		bool update() override;

		void updateHighTemporalResolutionCursorPos(Point rawClientPos) override;

		const CursorState& getState() const override;

		Array<std::pair<int64, Point>> getHighTemporalResolutionCursorPos() const override;

		void setPos(Point pos) override;



		bool isClippedToWindow() const noexcept override;

		void clipToWindow(bool clip) override;

	private:

		HWND m_hWnd = nullptr;

		CursorState m_state;

		HighTemporalResolutionCursor m_highTemporalResolutionCursor;

		bool m_clippedToWindow = false;
	};
}
