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

# include <Siv3D/ScopedQuadWarp2D.hpp>
# include <Siv3D/Graphics2D.hpp>

namespace s3d
{
	ScopedQuadWarp2D::ScopedQuadWarp2D(const Mat3x3& transform, const Target target)
		: m_oldTransform{ Graphics2D::GetQuadWarpTransform() }
	{
		Graphics2D::Internal::SetQuadWarpTransform((target == Target::Push) ? (transform * *m_oldTransform) : transform);
	}

	ScopedQuadWarp2D::ScopedQuadWarp2D(ScopedQuadWarp2D&& other) noexcept
		: m_oldTransform{ std::exchange(other.m_oldTransform, none) } {}

	ScopedQuadWarp2D::~ScopedQuadWarp2D()
	{
		if (m_oldTransform)
		{
			Graphics2D::Internal::SetQuadWarpTransform(*m_oldTransform);
		}
	}
}
