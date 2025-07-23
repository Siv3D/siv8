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
# include <Siv3D/PixelShader.hpp>
# include <Siv3D/FontMethod.hpp>
# include <Siv3D/TextStyle.hpp>

namespace s3d
{
	struct FontShader
	{
		PixelShader bitmapFont;

		PixelShader msdfFont;

		[[nodiscard]]
		const PixelShader& getFontShader(const FontMethod method, const TextStyle::Type type) noexcept
		{
			if (method == FontMethod::Bitmap)
			{
				return bitmapFont;
			}
			else
			{
				return msdfFont;
			}

			//if (isColorGlyph)
			//{
			//	return shaders[1];
			//}
			//else if (method == FontMethod::Bitmap)
			//{
			//	return shaders[0];
			//}

			//return shaders[2 + (FromEnum(method) - 1) * 4 + FromEnum(type)];
		}
	};
}
