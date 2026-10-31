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

namespace s3d
{
	inline void MSRenderTexture::swap(MSRenderTexture& other) noexcept
	{
		m_handle.swap(other.m_handle);
	}
}
