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

namespace s3d
{
	namespace Unicode
	{
		////////////////////////////////////////////////////////////////
		//
		//	GetBOMSize
		//
		////////////////////////////////////////////////////////////////

		constexpr int32 GetBOMSize(const TextEncoding encoding) noexcept
		{
			switch (encoding)
			{
			case TextEncoding::UTF8_WITH_BOM:
				return 3;
			case TextEncoding::UTF16LE:
			case TextEncoding::UTF16BE:
				return 2;
			default:
				return 0;
			}
		}
	}
}
