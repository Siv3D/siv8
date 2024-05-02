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
# include <Siv3D/Common.hpp>

namespace s3d
{
	enum class CursorStyle : uint8;
	class StringView;
	class Image;
	struct Point;

	class SIV3D_NOVTABLE ISiv3DCursorStyle
	{
	public:

		[[nodiscard]]
		static ISiv3DCursorStyle* Create();

		virtual ~ISiv3DCursorStyle() = default;

		virtual void init() = 0;

		virtual bool update() = 0;

		virtual void setDefaultStyle(CursorStyle style) = 0;

		virtual bool registerCursor(StringView name, const Image& image, Point hotSpot) = 0;

		virtual void requestStyle(CursorStyle style) = 0;

		virtual void requestStyle(StringView name) = 0;
	};
}
