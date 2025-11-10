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
# include <Siv3D/Common.hpp>

namespace s3d
{
	class Font;
	struct GUIColorStyle;
	struct GUIShapeStyle;
	class Theme;

	class SIV3D_NOVTABLE ISiv3DGUI
	{
	public:

		[[nodiscard]]
		static ISiv3DGUI* Create();

		virtual ~ISiv3DGUI() = default;

		virtual void init() = 0;

		virtual const Font& getDefaultFont() const noexcept = 0;

		virtual const GUIColorStyle& getColorStyle(const Theme& theme) const noexcept = 0;

		virtual const GUIShapeStyle& getShapeStyle(const Theme& theme) const noexcept = 0;
	};
}
