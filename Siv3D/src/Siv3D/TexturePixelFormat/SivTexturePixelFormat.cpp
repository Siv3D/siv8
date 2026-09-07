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

# include <array>
# include <Siv3D/TexturePixelFormat.hpp>
# include <Siv3D/StringView.hpp>
# include <Siv3D/FormatData.hpp>
# include <Siv3D/Utility.hpp>

namespace s3d
{
	namespace
	{
		static constexpr std::array TexturePixelFormatStrings =
		{
			U"Unknown"_sv,
			U"R8_Unorm"_sv,
			U"R8G8_Unorm"_sv,
			U"R16_Float"_sv,
			U"R8G8B8A8_Unorm"_sv,
			U"R8G8B8A8_Unorm_SRGB"_sv,
			U"R16G16_Unorm"_sv,
			U"R16G16_Float"_sv,
			U"R32_Float"_sv,
			U"R10G10B10A2_Unorm"_sv,
			U"R11G11B10_UFloat"_sv,
			U"R16G16B16A16_Float"_sv,
			U"R32G32_Float"_sv,
			U"R32G32B32A32_Float"_sv,
			U"BC1_RGBA_Unorm"_sv,
			U"BC1_RGBA_Unorm_SRGB"_sv,
			U"BC3_RGBA_Unorm"_sv,
			U"BC3_RGBA_Unorm_SRGB"_sv,
			U"BC4_R_Unorm"_sv,
			U"BC5_RG_Unorm"_sv,
			U"BC7_RGBA_Unorm"_sv,
			U"BC7_RGBA_Unorm_SRGB"_sv,
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const TexturePixelFormat value)
	{
		formatData.string.append(TexturePixelFormatStrings[FromEnum(value)]);
	}
}

