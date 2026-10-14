//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# include "IRenderer2D.hpp"
# include <Siv3D/LineCap.hpp>
# include <Siv3D/Geometry2D/BezierTessellation.hpp>

namespace s3d
{
	namespace
	{
		void Tessellate(Array<Vec2>& destination, const detail::BezierTessellation::Curve& source,
			ISiv3DRenderer2D& renderer, const double targetError, const int32 maxSegments)
		{
			assert(0 < targetError);
			assert(1 <= maxSegments && maxSegments <= 4096);
			const Mat3x2 affine = renderer.getLocalTransform() * renderer.getCameraTransform();
			const auto metric = detail::BezierTessellation::TransformMetric(source, affine, renderer.getSceneScale());
			detail::BezierTessellation::Workspace workspace;
			detail::BezierTessellation::Flatten(destination, workspace, source, metric, targetError, static_cast<uint32>(maxSegments));
		}
	}

	void ISiv3DRenderer2D::addBezier(const Bezier2& curve, const LineCap startCap, const LineCap endCap,
		const float thickness, const Float4& color, const double targetError, const int32 maxSegments)
	{
		Tessellate(m_bezierPoints, curve, *this, targetError, maxSegments);
		addLineString(startCap, endCap, m_bezierPoints, none, Abs(thickness), false, CloseRing::No, color);
	}

	void ISiv3DRenderer2D::addBezier(const Bezier3& curve, const LineCap startCap, const LineCap endCap,
		const float thickness, const Float4& color, const double targetError, const int32 maxSegments)
	{
		Tessellate(m_bezierPoints, curve, *this, targetError, maxSegments);
		addLineString(startCap, endCap, m_bezierPoints, none, Abs(thickness), false, CloseRing::No, color);
	}
}
