//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <Siv3D/Common.hpp>
# include <Siv3D/Image.hpp>
# include <Siv3D/GlyphIndex.hpp>

namespace s3d
{
	class SIV3D_NOVTABLE ISiv3DIcon
	{
	public:

		[[nodiscard]]
		static ISiv3DIcon* Create();

		virtual ~ISiv3DIcon() = default;

		virtual void init() = 0;

		virtual bool hasGlyph(char32 icon) const = 0;

		virtual GlyphIndex getGlyphIndex(char32 icon) const = 0;

		virtual Image render(char32 icon, int32 size) = 0;

		virtual Image renderMSDF(char32 icon, int32 size, int32 bufferSize) = 0;
	};
}
