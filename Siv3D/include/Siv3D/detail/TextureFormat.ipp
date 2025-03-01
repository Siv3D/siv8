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
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	constexpr TextureFormat::TextureFormat(const TexturePixelFormat value) noexcept
		: m_value{ value } {}

	////////////////////////////////////////////////////////////////
	//
	//	value
	//
	////////////////////////////////////////////////////////////////

	constexpr TexturePixelFormat TextureFormat::value() const noexcept
	{
		return m_value;
	}
}
