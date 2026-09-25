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

# include <Siv3D/GlyphContext.hpp>
# include <Siv3D/TextEffect/QuadWarp.hpp>
# include <Siv3D/ScopedQuadWarp2D.hpp>
# include <Siv3D/Graphics2D.hpp>

namespace s3d
{
	namespace TextEffect
	{
		void QuadWarp::draw(const TextureRegion& textureRegion, const GlyphContext& glyphContext) const
		{
			const ColorF color = (glyphContext.isColorGlyph ? ColorF{ 1.0, m_color.a } : m_color);

			const Quad source = RectF{ glyphContext.pos, textureRegion.size }.asQuad();

			Quad quad = source;

			quad.p0.y -= (textureRegion.size.x * m_leftOffset);
			quad.p1.y -= (textureRegion.size.x * m_rightOffset);
			
			const Mat3x2 affine = (Graphics2D::GetLocalTransform() * Graphics2D::GetCameraTransform());
			const auto transform = Mat3x3::TryHomography(
				Quad{ affine.transformPoint(source.p0), affine.transformPoint(source.p1), affine.transformPoint(source.p2), affine.transformPoint(source.p3) },
				Quad{ affine.transformPoint(quad.p0), affine.transformPoint(quad.p1), affine.transformPoint(quad.p2), affine.transformPoint(quad.p3) });
			if (not transform)
			{
				return;
			}
			const ScopedQuadWarp2D warp{ *transform };
			textureRegion.draw(glyphContext.pos, color);
		}
	}
}
