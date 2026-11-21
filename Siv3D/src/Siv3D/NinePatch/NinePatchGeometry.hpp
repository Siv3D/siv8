//-----------------------------------------------
// This file is part of the Siv3D Engine.
// Copyright (c) 2008-2026 Ryo Suzuki
// Copyright (c) 2016-2026 OpenSiv3D Project
// Licensed under the MIT License.
//-----------------------------------------------

# pragma once
# include <cmath>
# include <span>
# include <Siv3D/NinePatch.hpp>
# include <Siv3D/TriangleIndex.hpp>
# include <Siv3D/Utility.hpp>

namespace s3d::detail
{
	struct NinePatchLayout
	{
		std::array<double, 4> x, y;
		double scale;

		RectF innerRect() const noexcept
		{
			return{ x[1], y[1], (x[2] - x[1]), (y[2] - y[1]) };
		}
	};

	// Positive destination and nonnegative slice/scale. Shared by drawing and layout queries.
	inline NinePatchLayout MakeNinePatchLayout(const RectF& rect, const NinePatch::Slice slice, double scale) noexcept
	{
		const double horizontal = (slice.left + slice.right);
		const double vertical = (slice.top + slice.bottom);
		if (rect.w < horizontal * scale)
		{
			scale = (rect.w / horizontal);
		}
		if (rect.h < vertical * scale)
		{
			scale = (rect.h / vertical);
		}

		const double left = (rect.x + slice.left * scale);
		const double top = (rect.y + slice.top * scale);
		return{
			{ rect.x, left, Max(left, (rect.x + rect.w - slice.right * scale)), (rect.x + rect.w) },
			{ rect.y, top, Max(top, (rect.y + rect.h - slice.bottom * scale)), (rect.y + rect.h) },
			scale
		};
	}

	// The arrays are ordinary CPU storage; upload remains the renderer's responsibility.
	inline uint32 BuildNinePatchMesh(const NinePatchLayout& layout,
		const std::array<float, 4>& u, const std::array<float, 4>& v,
		const Float4 color, const bool drawCenter,
		std::array<Vertex2D, 16>& vertices, std::array<TriangleIndex, 18>& triangles) noexcept
	{
		for (size_t y = 0; y < 4; ++y)
		{
			for (size_t x = 0; x < 4; ++x)
			{
				vertices[y * 4 + x].set(static_cast<float>(layout.x[x]), static_cast<float>(layout.y[y]), u[x], v[y], color);
			}
		}

		uint32 count = 0;
		for (uint16 y = 0; y < 3; ++y)
		{
			for (uint16 x = 0; x < 3; ++x)
			{
				const uint16 i = (y * 4 + x);
				if (((not drawCenter) && (x == 1) && (y == 1))
					|| (vertices[i].pos.x >= vertices[i + 1].pos.x)
					|| (vertices[i].pos.y >= vertices[i + 4].pos.y))
				{
					continue;
				}
				triangles[count++] = { i, uint16(i + 1), uint16(i + 4) };
				triangles[count++] = { uint16(i + 4), uint16(i + 1), uint16(i + 5) };
			}
		}
		return count;
	}

	inline constexpr uint32 NinePatchMaxRepeat = 128;
	inline constexpr uint32 NinePatchTileBatchSize = 128;

	inline constexpr auto NinePatchTileIndices = []
	{
		std::array<TriangleIndex, NinePatchTileBatchSize * 2> indices{};
		for (uint16 i = 0; i < NinePatchTileBatchSize; ++i)
		{
			const uint16 v = (i * 4);
			indices[i * 2] = { v, uint16(v + 1), uint16(v + 2) };
			indices[i * 2 + 1] = { uint16(v + 2), uint16(v + 1), uint16(v + 3) };
		}
		return indices;
	}();

	// Emit fixed-size CPU batches through the caller's ordinary textured-mesh path.
	// Input: positive layout.scale and sourceCenter, with at least one tiling mode enabled.
	template <class Submit>
	uint32 BuildNinePatchTiles(const NinePatchLayout& layout, const SizeF sourceCenter,
		const std::array<float, 4>& u, const std::array<float, 4>& v,
		const NinePatch::Tiling tiling, const Float4 color, const bool drawCenter, Submit&& submit)
	{
		const double width = (layout.x[2] - layout.x[1]);
		const double height = (layout.y[2] - layout.y[1]);
		const bool center = (drawCenter && tiling.center && (width > 0) && (height > 0));
		const bool repeatX = (center || (tiling.topBottom && ((layout.y[1] > layout.y[0]) || (layout.y[3] > layout.y[2]))));
		const bool repeatY = (center || (tiling.leftRight && ((layout.x[1] > layout.x[0]) || (layout.x[3] > layout.x[2]))));
		double scale = layout.scale;
		if (repeatX) scale = Max(scale, width / (sourceCenter.x * NinePatchMaxRepeat));
		if (repeatY) scale = Max(scale, height / (sourceCenter.y * NinePatchMaxRepeat));
		const SizeF period = (sourceCenter * scale);

		std::array<Vertex2D, NinePatchTileBatchSize * 4> vertices;
		uint32 batchSize = 0, total = 0;
		const auto flush = [&]
		{
			submit(std::span{ vertices.data(), batchSize * 4 }, std::span{ NinePatchTileIndices.data(), batchSize * 2 });
			total += batchSize;
			batchSize = 0;
		};

		for (size_t row = 0; row < 3; ++row)
		for (size_t col = 0; col < 3; ++col)
		{
			const double x0 = layout.x[col], x1 = layout.x[col + 1];
			const double y0 = layout.y[row], y1 = layout.y[row + 1];
			if (((not drawCenter) && (col == 1) && (row == 1)) || (x1 <= x0) || (y1 <= y0)) continue;
			const bool tileX = ((col == 1) && ((row == 1) ? tiling.center : tiling.topBottom));
			const bool tileY = ((row == 1) && ((col == 1) ? tiling.center : tiling.leftRight));
			const uint32 nx = (tileX ? Min(uint32(std::ceil((x1 - x0) / period.x)), NinePatchMaxRepeat) : 1);
			const uint32 ny = (tileY ? Min(uint32(std::ceil((y1 - y0) / period.y)), NinePatchMaxRepeat) : 1);
			double top = y0;
			for (uint32 iy = 0; iy < ny; ++iy)
			{
				const bool lastY = (iy + 1 == ny);
				const double bottom = (lastY ? y1 : (y0 + (iy + 1) * period.y));
				const float vt = v[row];
				const float vb = ((tileY && lastY) ? static_cast<float>(vt + (v[row + 1] - vt) * Min((bottom - top) / period.y, 1.0)) : v[row + 1]);
				double left = x0;
				for (uint32 ix = 0; ix < nx; ++ix)
				{
					const bool lastX = (ix + 1 == nx);
					const double right = (lastX ? x1 : (x0 + (ix + 1) * period.x));
					const float ul = u[col];
					const float ur = ((tileX && lastX) ? static_cast<float>(ul + (u[col + 1] - ul) * Min((right - left) / period.x, 1.0)) : u[col + 1]);
					const float l = static_cast<float>(left), r = static_cast<float>(right);
					const float t = static_cast<float>(top), b = static_cast<float>(bottom);
					if ((l < r) && (t < b))
					{
						const uint32 base = (batchSize * 4);
						vertices[base].set(l, t, ul, vt, color);
						vertices[base + 1].set(r, t, ur, vt, color);
						vertices[base + 2].set(l, b, ul, vb, color);
						vertices[base + 3].set(r, b, ur, vb, color);
						if (++batchSize == NinePatchTileBatchSize) flush();
					}
					left = right;
				}
				top = bottom;
			}
		}
		if (batchSize) flush();
		return total;
	}
}
