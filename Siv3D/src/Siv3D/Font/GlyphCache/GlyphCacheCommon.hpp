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
# include <Siv3D/PointVector.hpp>
# include <Siv3D/GlyphIndex.hpp>
# include <Siv3D/ReadingDirection.hpp>

namespace s3d
{
	struct FontFaceInfo;

	[[nodiscard]]
	double GetTabAdvance(double spaceWidth, double scale, double xBegin, double currentX, int32 tabSize);

	[[nodiscard]]
	bool ConsumeControlCharacterHorizontal(char32 ch, Vec2& penPos, int32& lineCount, const Vec2& basePos, double scale, double lineHeightScale, const FontFaceInfo& info);

	[[nodiscard]]
	bool ConsumeControlCharacterVertical(char32 ch, Vec2& penPos, int32& lineCount, const Vec2& basePos, double scale, double lineHeightScale, const FontFaceInfo& info);
}
