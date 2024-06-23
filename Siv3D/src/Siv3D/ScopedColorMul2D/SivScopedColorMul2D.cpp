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

# include <Siv3D/ScopedColorMul2D.hpp>
# include <Siv3D/Graphics2D.hpp>

namespace s3d
{
	ScopedColorMul2D::ScopedColorMul2D(const ColorF& color)
		: ScopedColorMul2D{ color.toFloat4() } {}

	ScopedColorMul2D::ScopedColorMul2D(const Float4& color)
		: m_oldColor{ Graphics2D::GetColorMul() }
	{
		Graphics2D::Internal::SetColorMul(color);
	}

	ScopedColorMul2D::ScopedColorMul2D(const Float3& rgb, const double a)
		: ScopedColorMul2D{ Float4{ rgb, a } } {}

	ScopedColorMul2D::ScopedColorMul2D(const double rgb, const double a)
		: ScopedColorMul2D{ Float4{ rgb, rgb, rgb, a } } {}

	ScopedColorMul2D::ScopedColorMul2D(const double r, const double g, const double b, const double a)
		: ScopedColorMul2D{ Float4{ r, g, b, a } } {}

	ScopedColorMul2D::ScopedColorMul2D(ScopedColorMul2D&& other) noexcept
	{
		m_oldColor = std::exchange(other.m_oldColor, none);
	}

	ScopedColorMul2D::~ScopedColorMul2D()
	{
		if (m_oldColor)
		{
			Graphics2D::Internal::SetColorMul(*m_oldColor);
		}
	}

	void ScopedColorMul2D::clear() noexcept
	{
		m_oldColor.reset();
	}
}
