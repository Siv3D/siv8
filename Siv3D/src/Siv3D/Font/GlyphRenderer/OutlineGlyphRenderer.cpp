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

# include "OutlineGlyphRenderer.hpp"

SIV3D_DISABLE_MSVC_WARNINGS_PUSH(5054)
# include "agg/agg_curves.h"
SIV3D_DISABLE_MSVC_WARNINGS_POP()

namespace s3d
{
	namespace
	{
		struct OutlineData
		{
			Array<LineString> rings;

			LineString ring;
		};

		struct GlyphBBox
		{
			double xMin = Math::Inf;
			double yMin = Math::Inf;
			double xMax = -Math::Inf;
			double yMax = -Math::Inf;
		};

		static void FinishRing(LineString& ring)
		{
			const Vec2& first = ring.front();
			const Vec2& last = ring.back();

			if (first != last)
			{
				ring.push_back(first);
			}
		}

		static int MoveTo(const FT_Vector* to, void* ptr)
		{
			OutlineData* user = static_cast<OutlineData*>(ptr);

			if (user->ring)
			{
				FinishRing(user->ring);
				user->rings.push_back(user->ring);
				user->ring.clear();
			}

			user->ring.emplace_back((to->x / 64.0), (to->y / 64.0));
			return 0;
		}

		static int LineTo(const FT_Vector* to, void* ptr)
		{
			OutlineData* user = static_cast<OutlineData*>(ptr);
			user->ring.emplace_back((to->x / 64.0), (to->y / 64.0));
			return 0;
		}

		static int ConicTo(const FT_Vector* control, const FT_Vector* to, void* ptr)
		{
			OutlineData* user = static_cast<OutlineData*>(ptr);

			if (user->ring)
			{
				const Vec2 prev = user->ring.back();
				user->ring.pop_back();

				agg_fontnik::curve3_div curve{ prev.x, prev.y,
					(control->x / 64.0), (control->y / 64.0),
					(to->x / 64.0), (to->y / 64.0) };
				curve.rewind(0);

				double x, y;
				unsigned cmd;

				while (agg_fontnik::path_cmd_stop != (cmd = curve.vertex(&x, &y)))
				{
					user->ring.emplace_back(x, y);
				}
			}

			return 0;
		}

		static int CubicTo(const FT_Vector* c1, const FT_Vector* c2, const FT_Vector* to, void* ptr)
		{
			OutlineData* user = static_cast<OutlineData*>(ptr);

			if (user->ring)
			{
				const Vec2 prev = user->ring.back();
				user->ring.pop_back();

				agg_fontnik::curve4_div curve{ prev.x, prev.y,
					(c1->x / 64.0), (c1->y / 64.0),
					(c2->x / 64.0), (c2->y / 64.0),
					(to->x / 64.0), (to->y / 64.0) };
				curve.rewind(0);

				double x, y;
				unsigned cmd;

				while (agg_fontnik::path_cmd_stop != (cmd = curve.vertex(&x, &y)))
				{
					user->ring.emplace_back(x, y);
				}
			}

			return 0;
		}

		[[nodiscard]]
		static Array<LineString> GetOutlineGlyph(FT_Face face, GlyphBBox& bbox, const s3d::CloseRing closeRing)
		{
			const FT_Outline_Funcs outlineFuncs = {
				.move_to = &MoveTo,
				.line_to = &LineTo,
				.conic_to = &ConicTo,
				.cubic_to = &CubicTo,
				.shift = 0,
				.delta = 0
			};

			OutlineData userData;

			if (const FT_Error error = ::FT_Outline_Decompose(&face->glyph->outline, &outlineFuncs, &userData))
			{
				return{};
			}

			if (userData.ring)
			{
				FinishRing(userData.ring);
				userData.rings.push_back(userData.ring);
			}

			if (not userData.rings)
			{
				return{};
			}

			for (auto& ring : userData.rings)
			{
				ring.unique_consecutive();
			}

			const auto orientation = ::FT_Outline_Get_Orientation(&face->glyph->outline);
			if (orientation == ::FT_ORIENTATION_POSTSCRIPT)
			{
				for (auto& ring : userData.rings)
				{
					ring.reverse();
				}
			}

			if (not closeRing)
			{
				for (auto& ring : userData.rings)
				{
					if (ring.front() == ring.back())
					{
						ring.pop_back();
					}
				}
			}

			GlyphBBox t;
			{
				for (const auto& ring : userData.rings)
				{
					for (const auto& point : ring)
					{
						if (t.xMax < point.x)
						{
							t.xMax = point.x;
						}

						if (point.x < t.xMin)
						{
							t.xMin = point.x;
						}

						if (t.yMax < point.y)
						{
							t.yMax = point.y;
						}

						if (point.y < t.yMin)
						{
							t.yMin = point.y;
						}
					}
				}
			}

			for (auto& ring : userData.rings)
			{
				for (auto& point : ring)
				{
					point.x -= t.xMin;
					point.y = -point.y;
					point.y += t.yMax;
				}
			}

			bbox.xMin = t.xMin;
			bbox.yMin = t.yMin;
			bbox.xMax = t.xMax;
			bbox.yMax = t.yMax;
			//bbox.xMin = std::round(t.xMin);
			//bbox.yMin = std::round(t.yMin);
			//bbox.xMax = std::round(t.xMax);
			//bbox.yMax = std::round(t.yMax);

			return userData.rings;
		}
	}

	OutlineGlyph RenderOutlineGlyph(::FT_Face face, GlyphIndex glyphIndex, CloseRing closeRing, const FontFaceInfo& info, const ReadingDirection readingDirection)
	{
		uint32 loadFlags = ((info.hinting ? FT_LOAD_DEFAULT : FT_LOAD_NO_HINTING) | FT_LOAD_NO_BITMAP);

		if (readingDirection == ReadingDirection::TopToBottom)
		{
			loadFlags |= FT_LOAD_VERTICAL_LAYOUT;
		}

		if (::FT_Load_Glyph(face, glyphIndex, loadFlags))
		{
			return{};
		}

		if (info.style & FontStyle::Bold)
		{
			::FT_GlyphSlot_Embolden(face->glyph);
		}

		if (info.style & FontStyle::Italic)
		{
			::FT_GlyphSlot_Oblique(face->glyph);
		}
		
		GlyphBBox bbox;
		Array<LineString> rings = GetOutlineGlyph(face, bbox, closeRing);

		OutlineGlyph glyph;
		glyph.glyphIndex	= glyphIndex;
		glyph.width			= static_cast<float>(bbox.xMax - bbox.xMin);
		glyph.height		= static_cast<float>(bbox.yMax - bbox.yMin);
		glyph.ascender		= info.ascender;
		glyph.descender		= info.descender;
		
		if (readingDirection == ReadingDirection::TopToBottom)
		{
			glyph.left		= (face->glyph->metrics.vertBearingX / 64.0f);
			glyph.top		= (face->glyph->metrics.vertBearingY / 64.0f);
			glyph.advance	= (face->glyph->metrics.vertAdvance / 64.0f);
		}
		else
		{
			glyph.left		= (face->glyph->metrics.horiBearingX / 64.0f);
			glyph.top		= (face->glyph->metrics.horiBearingY / 64.0f);
			glyph.advance	= (face->glyph->metrics.horiAdvance / 64.0f);
		}
		
		glyph.rings			= std::move(rings);
		return glyph;
	}
}
