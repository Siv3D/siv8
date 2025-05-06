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
# include "IFont.hpp"
# include <Siv3D/Font.hpp>
# include <Siv3D/AssetHandleManager/AssetHandleManager.hpp>
# include "FontData.hpp"

namespace s3d
{
	class CFont final : public ISiv3DFont
	{
	public:

		CFont();

		~CFont();

		void init() override;

		[[nodiscard]]
		FontFileInfo getFontFileInfo(FilePathView path) const override;

	private:

		FT_Library m_freeType = nullptr;

		AssetHandleManager<Font::IDType, FontData> m_fonts{ "Font" };
	};
}
