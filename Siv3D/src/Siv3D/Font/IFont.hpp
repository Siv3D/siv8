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
# include <Siv3D/FontFileInfo.hpp>

namespace s3d
{
	class SIV3D_NOVTABLE ISiv3DFont
	{
	public:

		[[nodiscard]]
		static ISiv3DFont* Create();

		virtual ~ISiv3DFont() = default;

		virtual void init() = 0;

		[[nodiscard]]
		virtual FontFileInfo getFontFileInfo(FilePathView path) const = 0;
	};
}
