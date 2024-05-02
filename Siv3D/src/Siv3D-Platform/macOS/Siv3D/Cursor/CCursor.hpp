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
# include <Siv3D/Cursor/ICursor.hpp>
# include <Siv3D/Cursor/HighTemporalResolutionCursor.hpp>
# include <Siv3D/GLFW/GLFW.hpp>

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
		
	private:
		
		GLFWwindow* m_window = nullptr;

		CursorState m_state;
		
		HighTemporalResolutionCursor m_highTemporalResolutionCursor;
	};
}
