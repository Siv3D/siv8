//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# include "NinePatchGeometry.hpp"
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/Renderer2D/IRenderer2D.hpp>

namespace s3d
{
	NinePatch::NinePatch(const Texture& texture, const double slice)
		: NinePatch{ TextureRegion{ texture }, slice } {}

	NinePatch::NinePatch(const Texture& texture, const Slice slice)
		: NinePatch{ TextureRegion{ texture }, slice } {}

	NinePatch::NinePatch(const TextureRegion& source, const double slice)
		: NinePatch{ source, Slice{ slice, slice, slice, slice } } {}

	NinePatch::NinePatch(const TextureRegion& source, const Slice slice)
	{
		const double width = Abs(source.size.x);
		const double height = Abs(source.size.y);
		if (source.texture.isEmpty()
			|| (slice.top < 0) || (slice.right < 0) || (slice.bottom < 0) || (slice.left < 0)
			|| (width <= (slice.left + slice.right)) || (height <= (slice.top + slice.bottom)))
		{
			return;
		}

		m_source = source;
		m_slice = slice;
		const auto uv = source.uvRect;
		const float left = ((source.size.x < 0) ? uv.right : uv.left);
		const float right = ((source.size.x < 0) ? uv.left : uv.right);
		const float top = ((source.size.y < 0) ? uv.bottom : uv.top);
		const float bottom = ((source.size.y < 0) ? uv.top : uv.bottom);
		m_u = { left, static_cast<float>(left + (right - left) * (slice.left / width)),
			static_cast<float>(right - (right - left) * (slice.right / width)), right };
		m_v = { top, static_cast<float>(top + (bottom - top) * (slice.top / height)),
			static_cast<float>(bottom - (bottom - top) * (slice.bottom / height)), bottom };
	}

	NinePatch& NinePatch::setBorderScale(const double scale) noexcept
	{
		m_borderScale = Max(scale, 0.0);
		return *this;
	}

	RectF NinePatch::innerRect(const RectF& rect) const noexcept
	{
		if (isEmpty() || (rect.w <= 0) || (rect.h <= 0))
		{
			return{ rect.pos, 0, 0 };
		}
		return detail::MakeNinePatchLayout(rect, m_slice, m_borderScale).innerRect();
	}

	RectF NinePatch::draw(const RectF& rect, const ColorF& color) const
	{
		return drawImpl(rect, color, true);
	}

	RectF NinePatch::drawFrame(const RectF& rect, const ColorF& color) const
	{
		return drawImpl(rect, color, false);
	}

	RectF NinePatch::drawImpl(const RectF& rect, const ColorF& color, const bool drawCenter) const
	{
		if (isEmpty() || (rect.w <= 0) || (rect.h <= 0))
		{
			return{ rect.pos, 0, 0 };
		}
		if ((m_tiling.topBottom || m_tiling.leftRight || (drawCenter && m_tiling.center)) && (m_borderScale > 0))
		{
			return drawTiled(rect, color, drawCenter);
		}

		const auto layout = detail::MakeNinePatchLayout(rect, m_slice, m_borderScale);
		std::array<Vertex2D, 16> vertices;
		std::array<TriangleIndex, 18> triangles;
		const uint32 count = detail::BuildNinePatchMesh(layout, m_u, m_v, color.toFloat4(), drawCenter, vertices, triangles);
		if (count == 0)
		{
			return{ rect.pos, 0, 0 };
		}

		SIV3D_ENGINE(Renderer2D)->addMesh2D(m_source.texture, vertices, std::span{ triangles.data(), count }, none);
		return rect;
	}

	RectF NinePatch::drawTiled(const RectF& rect, const ColorF& color, const bool drawCenter) const
	{
		const auto layout = detail::MakeNinePatchLayout(rect, m_slice, m_borderScale);
		const SizeF sourceCenter{ Abs(m_source.size.x) - (m_slice.left + m_slice.right),
			Abs(m_source.size.y) - (m_slice.top + m_slice.bottom) };
		const auto count = detail::BuildNinePatchTiles(layout, sourceCenter, m_u, m_v, m_tiling, color.toFloat4(), drawCenter,
			[&](const std::span<const Vertex2D> vertices, const std::span<const TriangleIndex> triangles)
			{
				SIV3D_ENGINE(Renderer2D)->addMesh2D(m_source.texture, vertices, triangles, none);
			});
		return (count ? rect : RectF{ rect.pos, 0, 0 });
	}
}
