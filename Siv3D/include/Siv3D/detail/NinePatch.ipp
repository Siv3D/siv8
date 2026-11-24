//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# pragma once

namespace s3d
{
	constexpr NinePatch::Tiling NinePatch::Tiling::All() noexcept
	{
		return{ true, true, true };
	}

	inline bool NinePatch::isEmpty() const noexcept
	{
		return m_source.texture.isEmpty();
	}

	inline NinePatch::operator bool() const noexcept
	{
		return (not isEmpty());
	}

	inline const TextureRegion& NinePatch::source() const noexcept
	{
		return m_source;
	}

	inline NinePatch::Slice NinePatch::slice() const noexcept
	{
		return m_slice;
	}

	inline double NinePatch::borderScale() const noexcept
	{
		return m_borderScale;
	}

	inline NinePatch& NinePatch::setTiling(const Tiling tiling) noexcept
	{
		m_tiling = tiling;
		return *this;
	}

	inline NinePatch::Tiling NinePatch::tiling() const noexcept
	{
		return m_tiling;
	}

	inline SizeF NinePatch::minimumSize() const noexcept
	{
		return{ ((m_slice.left + m_slice.right) * m_borderScale),
			((m_slice.top + m_slice.bottom) * m_borderScale) };
	}
}
