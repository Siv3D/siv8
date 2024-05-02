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
# include <Siv3D/CursorStyle/ICursorStyle.hpp>

namespace s3d
{
	class CCursorStyle final : public ISiv3DCursorStyle
	{
	public:

		CCursorStyle() = default;

		~CCursorStyle() override;

		void init() override;

		bool update() override;

		void setDefaultStyle(CursorStyle style) override;

		bool registerCursor(StringView name, const Image& image, Point hotSpot) override;

		void requestStyle(CursorStyle style) override;

		void requestStyle(StringView name) override;

	private:

	};
}
