//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

namespace s3d
{
	inline ColorRenderTarget::ColorRenderTarget(const RenderTexture& texture)
		: m_texture{ texture } {}

	inline ColorRenderTarget::ColorRenderTarget(const MSRenderTexture& texture)
		: m_texture{ texture } {}

	inline bool ColorRenderTarget::isEmpty() const noexcept
	{
		return m_texture.isEmpty();
	}

	inline ColorRenderTarget::operator bool() const noexcept
	{
		return static_cast<bool>(m_texture);
	}

	inline void ColorRenderTarget::release()
	{
		m_texture.release();
	}

	inline Texture::IDType ColorRenderTarget::id() const noexcept
	{
		return m_texture.id();
	}

	inline Size ColorRenderTarget::size() const
	{
		return m_texture.size();
	}

	inline TextureFormat ColorRenderTarget::format() const
	{
		return m_texture.format();
	}

	inline Texture ColorRenderTarget::getTexture() const
	{
		return m_texture;
	}

	inline void ColorRenderTarget::swap(ColorRenderTarget& other) noexcept
	{
		m_texture.swap(other.m_texture);
	}
}
