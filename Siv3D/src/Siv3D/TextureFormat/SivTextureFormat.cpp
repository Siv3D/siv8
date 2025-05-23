﻿//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2025 Ryo Suzuki
//	Copyright (c) 2016-2025 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <array>
# include <Siv3D/TextureFormat.hpp>
# include <Siv3D/FormatData.hpp>

enum DXGI_FORMAT
{
	DXGI_FORMAT_UNKNOWN = 0,
	DXGI_FORMAT_R32G32B32A32_TYPELESS = 1,
	DXGI_FORMAT_R32G32B32A32_FLOAT = 2,
	DXGI_FORMAT_R32G32B32A32_UINT = 3,
	DXGI_FORMAT_R32G32B32A32_SINT = 4,
	DXGI_FORMAT_R32G32B32_TYPELESS = 5,
	DXGI_FORMAT_R32G32B32_FLOAT = 6,
	DXGI_FORMAT_R32G32B32_UINT = 7,
	DXGI_FORMAT_R32G32B32_SINT = 8,
	DXGI_FORMAT_R16G16B16A16_TYPELESS = 9,
	DXGI_FORMAT_R16G16B16A16_FLOAT = 10,
	DXGI_FORMAT_R16G16B16A16_UNORM = 11,
	DXGI_FORMAT_R16G16B16A16_UINT = 12,
	DXGI_FORMAT_R16G16B16A16_SNORM = 13,
	DXGI_FORMAT_R16G16B16A16_SINT = 14,
	DXGI_FORMAT_R32G32_TYPELESS = 15,
	DXGI_FORMAT_R32G32_FLOAT = 16,
	DXGI_FORMAT_R32G32_UINT = 17,
	DXGI_FORMAT_R32G32_SINT = 18,
	DXGI_FORMAT_R32G8X24_TYPELESS = 19,
	DXGI_FORMAT_D32_FLOAT_S8X24_UINT = 20,
	DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS = 21,
	DXGI_FORMAT_X32_TYPELESS_G8X24_UINT = 22,
	DXGI_FORMAT_R10G10B10A2_TYPELESS = 23,
	DXGI_FORMAT_R10G10B10A2_UNORM = 24,
	DXGI_FORMAT_R10G10B10A2_UINT = 25,
	DXGI_FORMAT_R11G11B10_FLOAT = 26,
	DXGI_FORMAT_R8G8B8A8_TYPELESS = 27,
	DXGI_FORMAT_R8G8B8A8_UNORM = 28,
	DXGI_FORMAT_R8G8B8A8_UNORM_SRGB = 29,
	DXGI_FORMAT_R8G8B8A8_UINT = 30,
	DXGI_FORMAT_R8G8B8A8_SNORM = 31,
	DXGI_FORMAT_R8G8B8A8_SINT = 32,
	DXGI_FORMAT_R16G16_TYPELESS = 33,
	DXGI_FORMAT_R16G16_FLOAT = 34,
	DXGI_FORMAT_R16G16_UNORM = 35,
	DXGI_FORMAT_R16G16_UINT = 36,
	DXGI_FORMAT_R16G16_SNORM = 37,
	DXGI_FORMAT_R16G16_SINT = 38,
	DXGI_FORMAT_R32_TYPELESS = 39,
	DXGI_FORMAT_D32_FLOAT = 40,
	DXGI_FORMAT_R32_FLOAT = 41,
	DXGI_FORMAT_R32_UINT = 42,
	DXGI_FORMAT_R32_SINT = 43,
	DXGI_FORMAT_R24G8_TYPELESS = 44,
	DXGI_FORMAT_D24_UNORM_S8_UINT = 45,
	DXGI_FORMAT_R24_UNORM_X8_TYPELESS = 46,
	DXGI_FORMAT_X24_TYPELESS_G8_UINT = 47,
	DXGI_FORMAT_R8G8_TYPELESS = 48,
	DXGI_FORMAT_R8G8_UNORM = 49,
	DXGI_FORMAT_R8G8_UINT = 50,
	DXGI_FORMAT_R8G8_SNORM = 51,
	DXGI_FORMAT_R8G8_SINT = 52,
	DXGI_FORMAT_R16_TYPELESS = 53,
	DXGI_FORMAT_R16_FLOAT = 54,
	DXGI_FORMAT_D16_UNORM = 55,
	DXGI_FORMAT_R16_UNORM = 56,
	DXGI_FORMAT_R16_UINT = 57,
	DXGI_FORMAT_R16_SNORM = 58,
	DXGI_FORMAT_R16_SINT = 59,
	DXGI_FORMAT_R8_TYPELESS = 60,
	DXGI_FORMAT_R8_UNORM = 61,
	DXGI_FORMAT_R8_UINT = 62,
	DXGI_FORMAT_R8_SNORM = 63,
	DXGI_FORMAT_R8_SINT = 64,
	DXGI_FORMAT_A8_UNORM = 65,
	DXGI_FORMAT_R1_UNORM = 66,
	DXGI_FORMAT_R9G9B9E5_SHAREDEXP = 67,
	DXGI_FORMAT_R8G8_B8G8_UNORM = 68,
	DXGI_FORMAT_G8R8_G8B8_UNORM = 69,
	DXGI_FORMAT_BC1_TYPELESS = 70,
	DXGI_FORMAT_BC1_UNORM = 71,
	DXGI_FORMAT_BC1_UNORM_SRGB = 72,
	DXGI_FORMAT_BC2_TYPELESS = 73,
	DXGI_FORMAT_BC2_UNORM = 74,
	DXGI_FORMAT_BC2_UNORM_SRGB = 75,
	DXGI_FORMAT_BC3_TYPELESS = 76,
	DXGI_FORMAT_BC3_UNORM = 77,
	DXGI_FORMAT_BC3_UNORM_SRGB = 78,
	DXGI_FORMAT_BC4_TYPELESS = 79,
	DXGI_FORMAT_BC4_UNORM = 80,
	DXGI_FORMAT_BC4_SNORM = 81,
	DXGI_FORMAT_BC5_TYPELESS = 82,
	DXGI_FORMAT_BC5_UNORM = 83,
	DXGI_FORMAT_BC5_SNORM = 84,
	DXGI_FORMAT_B5G6R5_UNORM = 85,
	DXGI_FORMAT_B5G5R5A1_UNORM = 86,
	DXGI_FORMAT_B8G8R8A8_UNORM = 87,
	DXGI_FORMAT_B8G8R8X8_UNORM = 88,
	DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM = 89,
	DXGI_FORMAT_B8G8R8A8_TYPELESS = 90,
	DXGI_FORMAT_B8G8R8A8_UNORM_SRGB = 91,
	DXGI_FORMAT_B8G8R8X8_TYPELESS = 92,
	DXGI_FORMAT_B8G8R8X8_UNORM_SRGB = 93,
	DXGI_FORMAT_BC6H_TYPELESS = 94,
	DXGI_FORMAT_BC6H_UF16 = 95,
	DXGI_FORMAT_BC6H_SF16 = 96,
	DXGI_FORMAT_BC7_TYPELESS = 97,
	DXGI_FORMAT_BC7_UNORM = 98,
	DXGI_FORMAT_BC7_UNORM_SRGB = 99,
	DXGI_FORMAT_AYUV = 100,
	DXGI_FORMAT_Y410 = 101,
	DXGI_FORMAT_Y416 = 102,
	DXGI_FORMAT_NV12 = 103,
	DXGI_FORMAT_P010 = 104,
	DXGI_FORMAT_P016 = 105,
	DXGI_FORMAT_420_OPAQUE = 106,
	DXGI_FORMAT_YUY2 = 107,
	DXGI_FORMAT_Y210 = 108,
	DXGI_FORMAT_Y216 = 109,
	DXGI_FORMAT_NV11 = 110,
	DXGI_FORMAT_AI44 = 111,
	DXGI_FORMAT_IA44 = 112,
	DXGI_FORMAT_P8 = 113,
	DXGI_FORMAT_A8P8 = 114,
	DXGI_FORMAT_B4G4R4A4_UNORM = 115,

	DXGI_FORMAT_P208 = 130,
	DXGI_FORMAT_V208 = 131,
	DXGI_FORMAT_V408 = 132,

	DXGI_FORMAT_SAMPLER_FEEDBACK_MIN_MIP_OPAQUE = 189,
	DXGI_FORMAT_SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE = 190,

	DXGI_FORMAT_FORCE_UINT = 0xffffffff
};

// https://github.com/bkaradzic/metal-cpp/blob/metal-cpp_macOS15.2_iOS18.2/Metal/MTLPixelFormat.hpp
enum MTLPixelFormat
{
	A8Unorm = 1,
	R8Unorm = 10,
	RG8Unorm = 30,
	R16Float = 25,
	RGBA8Unorm = 70,
	RGBA8Unorm_sRGB = 71,
	RG16Unorm = 60,
	RG16Float = 65,
	R32Float = 55,
	RGB10A2Unorm = 90,
	RG11B10Float = 92,
	RGBA16Float = 115,
	RG32Float = 105,
	RGBA32Float	= 125,
	BC1_RGBAUnorm = 130,
	BC1_RGBAUnorm_sRGB = 131,
	BC3_RGBAUnorm = 134,
	BC3_RGBAUnorm_sRGB = 135,
	BC4_RUnorm = 140,
	BC5_RGUnorm = 142,
	BC7_RGBAUnorm = 152,
	BC7_RGBAUnorm_sRGB = 153,
};

enum GL_CONSTANTS
{
	GL_ALPHA = 0x1906,
	GL_RGBA = 0x1908,
	GL_UNSIGNED_BYTE = 0x1401,
	GL_UNSIGNED_SHORT = 0x1403,
	GL_R8 = 0x8229,
	GL_SRGB8_ALPHA8 = 0x8C43,
	GL_RG16F = 0x822F,
	GL_RG = 0x8227,
	GL_HALF_FLOAT = 0x140B,
	GL_R32F = 0x822E,
	GL_RED = 0x1903,
	GL_FLOAT = 0x1406,
	GL_RGB10_A2 = 0x8059,
	GL_UNSIGNED_INT_10_10_10_2 = 0x8036,
	GL_R11F_G11F_B10F = 0x8C3A,
	GL_RGB = 0x1907,
	GL_RGBA8 = 0x8058,
	GL_RGBA16F = 0x881A,
	GL_RG32F = 0x8230,
	GL_RGBA32F = 0x8814,
	GL_RG16UI = 0x823A,

	GL_COMPRESSED_RGBA_S3TC_DXT1_EXT = 0x83F1,
	GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT = 0x8C4D,
	GL_COMPRESSED_RGBA_S3TC_DXT5_EXT = 0x83F3,
	GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT = 0x8C4F,
	GL_COMPRESSED_RED_RGTC1 = 0x8DBB,
	GL_COMPRESSED_RG_RGTC2 = 0x8DBD,
	GL_COMPRESSED_RGBA_BPTC_UNORM = 0x8E8C,
	GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM = 0x8E8D,
};

enum WGPU_CONSTANTS
{
	TextureFormat_R8Unorm = 0x01,
	TextureFormat_RG8Unorm = 0x08,
	TextureFormat_R16Float = 0x7,
	TextureFormat_R32Float = 0x0C,
	TextureFormat_RG16Uint = 0x0F,
	TextureFormat_RG16Float = 0x11,
	TextureFormat_RGBA8Unorm = 0x12,
	TextureFormat_RGBA8UnormSrgb = 0x13,
	TextureFormat_RGB10A2Unorm = 0x19,
	TextureFormat_RG11B10Ufloat = 0x1A,
	TextureFormat_RG32Float = 0x1C,
	TextureFormat_RGBA16Float = 0x21,
	TextureFormat_RGBA32Float = 0x22,
	TextureFormat_BC1RGBAUnorm = 0x2C,
	TextureFormat_BC1RGBAUnormSrgb = 0x2D,
	TextureFormat_BC3RGBAUnorm = 0x30,
	TextureFormat_BC3RGBAUnormSrgb = 0x31,
	TextureFormat_BC4RUnorm = 0x32,
	TextureFormat_BC5RGUnorm = 0x34,
	TextureFormat_BC7RGBAUnorm = 0x38,
	TextureFormat_BC7RGBAUnormSrgb = 0x39,
};

namespace s3d
{
	namespace
	{
		struct TextureFormatData
		{
			StringView name;

			DXGI_FORMAT DXGIFormat{};

			MTLPixelFormat MTLPixelFormat{};

			GL_CONSTANTS GLInternalFormat{};

			GL_CONSTANTS GLFormat{};

			GL_CONSTANTS GLType{};

			WGPU_CONSTANTS WGPUFormat{};

			// 1 ピクセル当たりのサイズ
			uint8 pixelSize = 0;

			// チャンネル数
			uint8 num_channels = 0;

			bool isSRGB = false;
		};

		static constexpr std::array<TextureFormatData, 22> TextureFormatPropertyTable =
		{ {
			{ U"Unknown", DXGI_FORMAT_UNKNOWN, MTLPixelFormat(0), GL_CONSTANTS(0), GL_CONSTANTS(0), GL_CONSTANTS(0), WGPU_CONSTANTS(0), 0, 0 },
			{ U"R8_Unorm", DXGI_FORMAT_R8_UNORM, MTLPixelFormat::R8Unorm, GL_RED, GL_RED, GL_UNSIGNED_BYTE, TextureFormat_R8Unorm, 1, 1 },
			{ U"RG8_Unorm", DXGI_FORMAT_R8G8_UNORM, MTLPixelFormat::RG8Unorm, GL_RG, GL_RG, GL_UNSIGNED_BYTE, TextureFormat_RG8Unorm, 2, 2 },
			{ U"R16_Float", DXGI_FORMAT_R16_FLOAT, MTLPixelFormat::R16Float, GL_RG16F, GL_RG, GL_HALF_FLOAT, TextureFormat_R16Float, 2, 1 },
			{ U"R8G8B8A8_Unorm", DXGI_FORMAT_R8G8B8A8_UNORM, MTLPixelFormat::RGBA8Unorm, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, TextureFormat_RGBA8Unorm, 4, 4 },
			{ U"R8G8B8A8_Unorm_SRGB", DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, MTLPixelFormat::RGBA8Unorm_sRGB, GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_BYTE, TextureFormat_RGBA8UnormSrgb, 4, 4, true },
			{ U"R16G16_Unorm", DXGI_FORMAT_R16G16_UNORM, MTLPixelFormat::RG16Unorm, GL_RG16UI, GL_RG, GL_UNSIGNED_SHORT, TextureFormat_RG16Uint, 4, 2  },
			{ U"R16G16_Float", DXGI_FORMAT_R16G16_FLOAT, MTLPixelFormat::RG16Float, GL_RG16F, GL_RG, GL_HALF_FLOAT, TextureFormat_RG16Float, 4, 2 },
			{ U"R32_Float", DXGI_FORMAT_R32_FLOAT, MTLPixelFormat::R32Float, GL_R32F, GL_RED, GL_FLOAT, TextureFormat_R32Float, 4, 1 },
			{ U"R10G10B10A2_Unorm", DXGI_FORMAT_R10G10B10A2_UNORM, MTLPixelFormat::RGB10A2Unorm, GL_RGB10_A2, GL_RGBA, GL_UNSIGNED_INT_10_10_10_2, TextureFormat_RGB10A2Unorm, 4, 4 },
			{ U"R11G11B10_UFloat", DXGI_FORMAT_R11G11B10_FLOAT, MTLPixelFormat::RG11B10Float, GL_R11F_G11F_B10F, GL_RGB, GL_FLOAT, TextureFormat_RG11B10Ufloat, 4, 3 },
			{ U"R16G16B16A16_Float", DXGI_FORMAT_R16G16B16A16_FLOAT, MTLPixelFormat::RGBA16Float, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, TextureFormat_RGBA16Float, 8, 4 },
			{ U"R32G32_Float", DXGI_FORMAT_R32G32_FLOAT, MTLPixelFormat::RG32Float, GL_RG32F, GL_RG, GL_FLOAT, TextureFormat_RG32Float, 8, 2 },
			{ U"R32G32B32A32_Float", DXGI_FORMAT_R32G32B32A32_FLOAT, MTLPixelFormat::RGBA32Float, GL_RGBA32F, GL_RGBA, GL_FLOAT, TextureFormat_RGBA32Float, 16, 4 },
			{ U"BC1_RGBA_Unorm", DXGI_FORMAT_BC1_UNORM, MTLPixelFormat::BC1_RGBAUnorm, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, GL_UNSIGNED_BYTE, TextureFormat_BC1RGBAUnorm, 0, 4 },
			{ U"BC1_RGBA_Unorm_SRGB", DXGI_FORMAT_BC1_UNORM_SRGB, MTLPixelFormat::BC1_RGBAUnorm_sRGB, GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT, GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT, GL_UNSIGNED_BYTE, TextureFormat_BC1RGBAUnormSrgb, 0, 4 },		
			{ U"BC3_RGBA_Unorm", DXGI_FORMAT_BC3_UNORM, MTLPixelFormat::BC3_RGBAUnorm, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, GL_UNSIGNED_BYTE, TextureFormat_BC3RGBAUnorm, 0, 4 },
			{ U"BC3_RGBA_Unorm_SRGB", DXGI_FORMAT_BC3_UNORM_SRGB, MTLPixelFormat::BC3_RGBAUnorm_sRGB, GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT, GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT, GL_UNSIGNED_BYTE, TextureFormat_BC3RGBAUnormSrgb, 0, 4 },
			{ U"BC4_R_Unorm", DXGI_FORMAT_BC4_UNORM, MTLPixelFormat::BC4_RUnorm, GL_COMPRESSED_RED_RGTC1, GL_COMPRESSED_RED_RGTC1, GL_UNSIGNED_BYTE , TextureFormat_BC4RUnorm, 0, 1 },
			{ U"BC5_RG_Unorm", DXGI_FORMAT_BC5_UNORM, MTLPixelFormat::BC5_RGUnorm, GL_COMPRESSED_RG_RGTC2, GL_COMPRESSED_RG_RGTC2, GL_UNSIGNED_BYTE , TextureFormat_BC5RGUnorm, 0, 2 },
			{ U"BC7_RGBA_Unorm", DXGI_FORMAT_BC7_UNORM, MTLPixelFormat::BC7_RGBAUnorm, GL_COMPRESSED_RGBA_BPTC_UNORM, GL_COMPRESSED_RGBA_BPTC_UNORM, GL_UNSIGNED_BYTE, TextureFormat_BC7RGBAUnorm, 0, 4 },
			{ U"BC7_RGBA_Unorm_SRGB", DXGI_FORMAT_BC7_UNORM_SRGB, MTLPixelFormat::BC7_RGBAUnorm_sRGB, GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM, GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM, GL_UNSIGNED_BYTE, TextureFormat_BC7RGBAUnormSrgb, 0, 4 },
		} };
	}

	////////////////////////////////////////////////////////////////
	//
	//	name
	//
	////////////////////////////////////////////////////////////////

	StringView TextureFormat::name() const noexcept
	{
		return TextureFormatPropertyTable[FromEnum(m_value)].name;
	}

	////////////////////////////////////////////////////////////////
	//
	//	DXGIFormat
	//
	////////////////////////////////////////////////////////////////

	int32 TextureFormat::DXGIFormat() const noexcept
	{
		return TextureFormatPropertyTable[FromEnum(m_value)].DXGIFormat;
	}

	////////////////////////////////////////////////////////////////
	//
	//	MTLPixelFormat
	//
	////////////////////////////////////////////////////////////////

	uint32 TextureFormat::MTLPixelFormat() const noexcept
	{
		return TextureFormatPropertyTable[FromEnum(m_value)].MTLPixelFormat;
	}

	////////////////////////////////////////////////////////////////
	//
	//	GLInternalFormat
	//
	////////////////////////////////////////////////////////////////

	int32 TextureFormat::GLInternalFormat() const noexcept
	{
		return TextureFormatPropertyTable[FromEnum(m_value)].GLInternalFormat;
	}

	////////////////////////////////////////////////////////////////
	//
	//	GLFormat
	//
	////////////////////////////////////////////////////////////////

	int32 TextureFormat::GLFormat() const noexcept
	{
		return TextureFormatPropertyTable[FromEnum(m_value)].GLFormat;
	}

	////////////////////////////////////////////////////////////////
	//
	//	GLType
	//
	////////////////////////////////////////////////////////////////

	int32 TextureFormat::GLType() const noexcept
	{
		return TextureFormatPropertyTable[FromEnum(m_value)].GLType;
	}

	////////////////////////////////////////////////////////////////
	//
	//	WGPUFormat
	//
	////////////////////////////////////////////////////////////////

	int32 TextureFormat::WGPUFormat() const noexcept
	{
		return TextureFormatPropertyTable[FromEnum(m_value)].WGPUFormat;
	}

	////////////////////////////////////////////////////////////////
	//
	//	pixelSize
	//
	////////////////////////////////////////////////////////////////

	uint32 TextureFormat::pixelSize() const noexcept
	{
		return TextureFormatPropertyTable[FromEnum(m_value)].pixelSize;
	}

	////////////////////////////////////////////////////////////////
	//
	//	num_channels
	//
	////////////////////////////////////////////////////////////////

	uint32 TextureFormat::num_channels() const noexcept
	{
		return TextureFormatPropertyTable[FromEnum(m_value)].num_channels;
	}

	////////////////////////////////////////////////////////////////
	//
	//	isSRGB
	//
	////////////////////////////////////////////////////////////////

	bool TextureFormat::isSRGB() const noexcept
	{
		return TextureFormatPropertyTable[FromEnum(m_value)].isSRGB;
	}

	////////////////////////////////////////////////////////////////
	//
	//	isBCn
	//
	////////////////////////////////////////////////////////////////

	bool TextureFormat::isBCn() const noexcept
	{
		return InRange(m_value, TexturePixelFormat::BC1_RGBA_Unorm, TexturePixelFormat::BC7_RGBA_Unorm_SRGB);
	}

	////////////////////////////////////////////////////////////////
	//
	//	bytesPerRow
	//
	////////////////////////////////////////////////////////////////

	uint32 TextureFormat::bytesPerRow(const uint32 width) const noexcept
	{
		if (isBCn())
		{
			const uint32 xBlocks = ((width + 3) / 4);
			return (xBlocks * blockSize());
		}
		else
		{
			return ((width * pixelSize() + 3) & ~3);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	blockSize
	//
	////////////////////////////////////////////////////////////////

	uint32 TextureFormat::blockSize() const noexcept
	{
		if ((m_value == TexturePixelFormat::BC1_RGBA_Unorm)
		 || (m_value == TexturePixelFormat::BC1_RGBA_Unorm_SRGB)
		 || (m_value == TexturePixelFormat::BC4_R_Unorm))
		{
			return 8;
		}
		else if ((m_value == TexturePixelFormat::BC3_RGBA_Unorm)
			  || (m_value == TexturePixelFormat::BC3_RGBA_Unorm_SRGB)
			  || (m_value == TexturePixelFormat::BC5_RG_Unorm)
			  || (m_value == TexturePixelFormat::BC7_RGBA_Unorm)
			  || (m_value == TexturePixelFormat::BC7_RGBA_Unorm_SRGB))
		{
			return 16;
		}
		else
		{
			return pixelSize();
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const TextureFormat& value)
	{
		formatData.string.append(value.name());
	}
}
