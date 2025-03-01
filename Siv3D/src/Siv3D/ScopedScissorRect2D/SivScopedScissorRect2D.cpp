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

# include <Siv3D/ScopedScissorRect2D.hpp>
# include <Siv3D/Graphics2D.hpp>

namespace s3d
{
	ScopedScissorRect2D::ScopedScissorRect2D(None_t)
		: m_oldScissorRect{ Graphics2D::GetScissorRect() }
	{
		Graphics2D::Internal::SetScissorRect(none);
	}

	ScopedScissorRect2D::ScopedScissorRect2D(const Optional<Rect>& viewport)
		: m_oldScissorRect{ Graphics2D::GetScissorRect() }
	{
		Graphics2D::Internal::SetScissorRect(viewport);
	}

	ScopedScissorRect2D::ScopedScissorRect2D(const int32 w, const int32 h)
		: ScopedScissorRect2D{ Rect{ w, h } } {}

	ScopedScissorRect2D::ScopedScissorRect2D(const Size size)
		: ScopedScissorRect2D{ Rect{ size } } {}

	ScopedScissorRect2D::ScopedScissorRect2D(const int32 x, const int32 y, const int32 w, const int32 h)
		: ScopedScissorRect2D{ Rect{ x, y, w, h } } {}

	ScopedScissorRect2D::ScopedScissorRect2D(const Point pos, const int32 w, const int32 h)
		: ScopedScissorRect2D{ Rect{ pos, w, h } } {}

	ScopedScissorRect2D::ScopedScissorRect2D(const int32 x, const int32 y, const Size size)
		: ScopedScissorRect2D{ Rect{ x, y, size } } {}

	ScopedScissorRect2D::ScopedScissorRect2D(const Point pos, const Size size)
		: ScopedScissorRect2D{ Rect{ pos, size } } {}

	ScopedScissorRect2D::ScopedScissorRect2D(ScopedScissorRect2D&& other) noexcept
	{
		m_oldScissorRect = std::exchange(other.m_oldScissorRect, none);
	}

	ScopedScissorRect2D::~ScopedScissorRect2D()
	{
		if (m_oldScissorRect)
		{
			Graphics2D::Internal::SetScissorRect(*m_oldScissorRect);
		}
	}

	void ScopedScissorRect2D::clear() noexcept
	{
		m_oldScissorRect.reset();
	}
}
