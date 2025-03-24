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
# include "Common.hpp"
# include "TexturePixelFormat.hpp"
# include "StringView.hpp"

namespace s3d
{
	/// @brief テクスチャフォーマット
	class TextureFormat
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		TextureFormat() = default;

		[[nodiscard]]
		constexpr TextureFormat(TexturePixelFormat value) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	value
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr TexturePixelFormat value() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	name
		//
		////////////////////////////////////////////////////////////////

		/// @brief フォーマットの名前を返します（例: R8G8B8A8_Unorm）
		/// @return フォーマットの名前
		[[nodiscard]]
		StringView name() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	DXGIFormat
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		int32 DXGIFormat() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	MTLPixelFormat
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		uint32 MTLPixelFormat() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	GLInternalFormat
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		int32 GLInternalFormat() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	GLFormat
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		int32 GLFormat() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	GLType
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		int32 GLType() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	WGPUFormat
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		int32 WGPUFormat() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	pixelSize
		//
		////////////////////////////////////////////////////////////////

		/// @brief 1 ピクセル当たりのサイズ（バイト）を返します。
		/// @return 1 ピクセル当たりのサイズ（バイト）
		[[nodiscard]]
		uint32 pixelSize() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	num_channels
		//
		////////////////////////////////////////////////////////////////

		/// @brief チャンネル数を返します。
		/// @return チャンネル数
		[[nodiscard]]
		uint32 num_channels() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isSRGB
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		bool isSRGB() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isBCn
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		bool isBCn() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	rowPitch
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		uint32 rowPitch(uint32 width) const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	blockSize
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		uint32 blockSize() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator ==
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		friend constexpr bool operator ==(TextureFormat lhs, TextureFormat rhs) noexcept
		{
			return (lhs.value() == rhs.value());
		}

		////////////////////////////////////////////////////////////////
		//
		//	operator <=>
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		friend constexpr auto operator <=>(TextureFormat lhs, TextureFormat rhs) noexcept
		{
			return (lhs.value() <=> rhs.value());
		}

		////////////////////////////////////////////////////////////////
		//
		//	(constant)
		//
		////////////////////////////////////////////////////////////////

		static constexpr TexturePixelFormat Unknown				= TexturePixelFormat::Unknown;

		static constexpr TexturePixelFormat R8_Unorm			= TexturePixelFormat::R8_Unorm;

		static constexpr TexturePixelFormat R8G8_Unorm			= TexturePixelFormat::R8G8_Unorm;

		static constexpr TexturePixelFormat R16_Float			= TexturePixelFormat::R16_Float;

		static constexpr TexturePixelFormat R8G8B8A8_Unorm		= TexturePixelFormat::R8G8B8A8_Unorm;

		static constexpr TexturePixelFormat R8G8B8A8_Unorm_SRGB = TexturePixelFormat::R8G8B8A8_Unorm_SRGB;

		static constexpr TexturePixelFormat R16G16_Unorm		= TexturePixelFormat::R16G16_Unorm;

		static constexpr TexturePixelFormat R16G16_Float		= TexturePixelFormat::R16G16_Float;

		static constexpr TexturePixelFormat R32_Float			= TexturePixelFormat::R32_Float;

		static constexpr TexturePixelFormat R10G10B10A2_Unorm	= TexturePixelFormat::R10G10B10A2_Unorm;

		static constexpr TexturePixelFormat R11G11B10_UFloat	= TexturePixelFormat::R11G11B10_UFloat;

		static constexpr TexturePixelFormat R16G16B16A16_Float	= TexturePixelFormat::R16G16B16A16_Float;

		static constexpr TexturePixelFormat R32G32_Float		= TexturePixelFormat::R32G32_Float;

		static constexpr TexturePixelFormat R32G32B32A32_Float	= TexturePixelFormat::R32G32B32A32_Float;

		static constexpr TexturePixelFormat BC1_RGBA_Unorm		= TexturePixelFormat::BC1_RGBA_Unorm;

		static constexpr TexturePixelFormat BC3_RGBA_Unorm		= TexturePixelFormat::BC3_RGBA_Unorm;

		static constexpr TexturePixelFormat BC4_R_Unorm			= TexturePixelFormat::BC4_R_Unorm;

		static constexpr TexturePixelFormat BC5_RG_Unorm		= TexturePixelFormat::BC5_RG_Unorm;

		static constexpr TexturePixelFormat BC7_RGBA_Unorm		= TexturePixelFormat::BC7_RGBA_Unorm;

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		friend void Formatter(FormatData& formatData, const TextureFormat& value);

	private:

		TexturePixelFormat m_value = TexturePixelFormat::Unknown;
	};
}

# include "detail/TextureFormat.ipp"
