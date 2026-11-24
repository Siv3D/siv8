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

# include <Siv3D/ScopedColorMul2D.hpp>
# include <Siv3D/Graphics2D.hpp>

namespace s3d
{
	ScopedColorMul2D::ScopedColorMul2D(const ColorF& color, const Target target)
		: ScopedColorMul2D{ color.toFloat4(), target } {}

	ScopedColorMul2D::ScopedColorMul2D(const Float4& color, const Target target)
		: m_oldColor{ Graphics2D::GetColorMul() }
	{
		Graphics2D::Internal::SetColorMul((target == Target::Push) ? (*m_oldColor * color) : color);
	}

	ScopedColorMul2D::ScopedColorMul2D(const Float3& rgb, const double a, const Target target)
		: ScopedColorMul2D{ Float4{ rgb, a }, target } {}

	ScopedColorMul2D::ScopedColorMul2D(const double rgb, const double a, const Target target)
		: ScopedColorMul2D{ Float4{ rgb, rgb, rgb, a }, target } {}

	ScopedColorMul2D::ScopedColorMul2D(const double r, const double g, const double b, const double a, const Target target)
		: ScopedColorMul2D{ Float4{ r, g, b, a }, target } {}

	ScopedColorMul2D::ScopedColorMul2D(const Float3& rgb, const Target target)
		: ScopedColorMul2D{ rgb, 1.0, target } {}

	ScopedColorMul2D::ScopedColorMul2D(const double rgb, const Target target)
		: ScopedColorMul2D{ rgb, 1.0, target } {}

	ScopedColorMul2D::ScopedColorMul2D(const double r, const double g, const double b, const Target target)
		: ScopedColorMul2D{ r, g, b, 1.0, target } {}

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
