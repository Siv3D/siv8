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
# include <Siv3D/FontMethod.hpp>
# include <Siv3D/MemoryMappedFileView.hpp>
# include "IFont.hpp"
# include "FontFace.hpp"

namespace s3d
{
	class FontData
	{
	public:

		struct Null {};

		FontData() = default;

		explicit FontData(Null);

		FontData(::FT_Library library, FilePathView path, size_t faceIndex, StringView styleName, FontMethod fontMethod, int32 baseSize, FontStyle style);

		[[nodiscard]]
		bool isInitialized() const noexcept;

		[[nodiscard]]
		String toString() const;

		[[nodiscard]]
		const FontFaceProperties& getProperties() const noexcept;

	private:

		MemoryMappedFileView m_mappedFileView;

		std::unique_ptr<FontFace> m_face;

		uint16 m_faceIndex = 0;

		FontMethod m_fontMethod = FontMethod::Bitmap;

		bool m_initialized = false;
	};
}
