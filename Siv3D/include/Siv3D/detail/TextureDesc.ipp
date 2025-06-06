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

# pragma once

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	constexpr TextureDesc::TextureDesc(const bool _hasMipmap, const bool _sRGB, bool const _isSDF) noexcept
		: hasMipmap{ _hasMipmap }
		, sRGB{ _sRGB }
		, isSDF{ _isSDF } {}

	constexpr TextureDesc::TextureDesc(const TextureDescBuilder& builder) noexcept
		: hasMipmap{ builder.hasMipmap() }
		, sRGB{ builder.sRGB() }
		, isSDF{ builder.isSDF() } {}
}
