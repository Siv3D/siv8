//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2024 Ryo Suzuki
//	Copyright (c) 2016-2024 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/ScopedColorAdd2D.hpp>
# include <Siv3D/Graphics2D.hpp>

namespace s3d
{
	ScopedColorAdd2D::ScopedColorAdd2D(const ColorF& rgb)
		: ScopedColorAdd2D{ rgb.toFloat3() } {}

	ScopedColorAdd2D::ScopedColorAdd2D(const Float4& rgb)
		: ScopedColorAdd2D{ rgb.xyz() } {}

	ScopedColorAdd2D::ScopedColorAdd2D(const Float3& rgb)
		: m_oldColor{ Graphics2D::GetColorAdd() }
	{
		Graphics2D::Internal::SetColorAdd(rgb);
	}

	ScopedColorAdd2D::ScopedColorAdd2D(const double rgb)
		: ScopedColorAdd2D{ Float3::All(static_cast<float>(rgb))} {}

	ScopedColorAdd2D::ScopedColorAdd2D(const double r, const double g, const double b)
		: ScopedColorAdd2D{ Float3{ r, g, b } } {}

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
