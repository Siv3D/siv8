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

# include <Siv3D/ScopedColorAdd2D.hpp>
# include <Siv3D/Graphics2D.hpp>

namespace s3d
{
	ScopedColorAdd2D::ScopedColorAdd2D(const ColorF& rgb, const Target target)
		: ScopedColorAdd2D{ rgb.toFloat3(), target } {}

	ScopedColorAdd2D::ScopedColorAdd2D(const Float4& rgb, const Target target)
		: ScopedColorAdd2D{ rgb.xyz(), target } {}

	ScopedColorAdd2D::ScopedColorAdd2D(const Float3& rgb, const Target target)
		: m_oldColor{ Graphics2D::GetColorAdd() }
	{
		Graphics2D::Internal::SetColorAdd((target == Target::Push) ? (*m_oldColor + rgb) : rgb);
	}

	ScopedColorAdd2D::ScopedColorAdd2D(const double rgb, const Target target)
		: ScopedColorAdd2D{ Float3::All(static_cast<float>(rgb)), target } {}

	ScopedColorAdd2D::ScopedColorAdd2D(const double r, const double g, const double b, const Target target)
		: ScopedColorAdd2D{ Float3{ r, g, b }, target } {}

	ScopedColorAdd2D::ScopedColorAdd2D(ScopedColorAdd2D&& other) noexcept
	{
		m_oldColor = std::exchange(other.m_oldColor, none);
	}

	ScopedColorAdd2D::~ScopedColorAdd2D()
	{
		if (m_oldColor)
		{
			Graphics2D::Internal::SetColorAdd(*m_oldColor);
		}
	}

	void ScopedColorAdd2D::clear() noexcept
	{
		m_oldColor.reset();
	}
}
