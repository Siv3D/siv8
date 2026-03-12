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

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	template <ReaderObject Reader>
	Font::Font(const int32 baseSize, Reader&& reader, const FontOptions& options)
		: Font{ FontMethod::Bitmap, baseSize, std::make_unique<Reader>(std::forward<Reader>(reader)), options } {}

	template <ReaderObject Reader>
	Font::Font(const FontMethod fontMethod, const int32 baseSize, Reader&& reader, const FontOptions& options)
		: Font{ fontMethod, baseSize, std::make_unique<Reader>(std::forward<Reader>(reader)), options } {}

	template <ReaderObject Reader>
	Font::Font(const FontMethod fontMethod, const int32 baseSize, const int32 bufferThickness, Reader&& reader, const FontOptions& options)
		: Font{ fontMethod, baseSize, std::make_unique<Reader>(std::forward<Reader>(reader)), options } {}
}
