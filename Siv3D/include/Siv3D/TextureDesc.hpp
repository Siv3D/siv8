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
# include "TextureDescBuilder.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	TextureDesc
	//
	////////////////////////////////////////////////////////////////

	struct TextureDesc
	{
		bool hasMipmap = true;

		bool sRGB = true;

		bool isSDF = false;

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		TextureDesc() = default;

		[[nodiscard]]
		constexpr TextureDesc(bool _hasMipmap, bool _sRGB, bool _isSDF) noexcept;

		[[nodiscard]]
		constexpr TextureDesc(const TextureDescBuilder& builder) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	(constant)
		//
		////////////////////////////////////////////////////////////////

		static constexpr TextureDescBuilder NoMipmap{ false, false, false };

		static constexpr TextureDescBuilder NoMipmapSRGB{ false, true, false };

		static constexpr TextureDescBuilder Mipmap{ true, false, false };

		static constexpr TextureDescBuilder MipmapSRGB{ true, true, false };

		static constexpr TextureDescBuilder SDF{ false, false, true };

		static constexpr TextureDescBuilder Default2D = Mipmap;

		static constexpr TextureDescBuilder Default3D = MipmapSRGB;
	};
}

# include "detail/TextureDesc.ipp"
