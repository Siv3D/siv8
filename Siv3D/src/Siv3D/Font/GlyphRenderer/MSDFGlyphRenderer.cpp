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

# include "MSDFGlyphRenderer.hpp"
# include <ThirdParty/msdfgen/msdfgen.h>
# include <ThirdParty/msdfgen/ext/resolve-shape-geometry.h>

namespace s3d
{
	namespace
	{
		struct FtContext
		{
			msdfgen::Point2 position{};
			msdfgen::Shape* shape = nullptr;
			msdfgen::Contour* contour = nullptr;
		};

		struct GlyphBBox
		{
			double xMin = Math::Inf;
			double yMin = Math::Inf;
			double xMax = -Math::Inf;
			double yMax = -Math::Inf;
		};

		[[nodiscard]]
		inline msdfgen::Point2 ftPoint2(const FT_Vector& vector) noexcept
		{
			return msdfgen::Point2{ (vector.x / 64.0), (vector.y / 64.0) };
		}

		static int ftMoveTo(const FT_Vector* to, void* user)
		{
			FtContext* context = reinterpret_cast<FtContext*>(user);
			context->contour = &context->shape->addContour();
			context->position = ftPoint2(*to);
			return 0;
		}

		static int ftLineTo(const FT_Vector* to, void* user)
		{
			FtContext* context = reinterpret_cast<FtContext*>(user);
			context->contour->addEdge(new msdfgen::LinearSegment(context->position, ftPoint2(*to)));
			context->position = ftPoint2(*to);
			return 0;
		}

		static int ftConicTo(const FT_Vector* control, const FT_Vector* to, void* user)
		{
			FtContext* context = reinterpret_cast<FtContext*>(user);
			context->contour->addEdge(new msdfgen::QuadraticSegment(context->position, ftPoint2(*control), ftPoint2(*to)));
			context->position = ftPoint2(*to);
			return 0;
		}

		static int ftCubicTo(const FT_Vector* control1, const FT_Vector* control2, const FT_Vector* to, void* user)
		{
			FtContext* context = reinterpret_cast<FtContext*>(user);
			context->contour->addEdge(new msdfgen::CubicSegment(context->position, ftPoint2(*control1), ftPoint2(*control2), ftPoint2(*to)));
			context->position = ftPoint2(*to);
			return 0;
		}

		[[nodiscard]]
		static bool GetShape(FT_Face face, msdfgen::Shape& shape)
		{
			const FT_Outline_Funcs func_interface = {
				.move_to	= &ftMoveTo,
				.line_to	= &ftLineTo,
				.conic_to	= &ftConicTo,
				.cubic_to	= &ftCubicTo,
				.shift = 0,
				.delta = 0
			};

			FtContext context;
			context.shape = &shape;

			if (::FT_Outline_Decompose(&face->glyph->outline, &func_interface, &context))
			{
				return false;
			}

			resolveShapeGeometry(shape);

			shape.normalize();

			edgeColoringSimple(shape, 3.0);

			return true;
		}

		[[nodiscard]]
		static GlyphBBox GetBound(const msdfgen::Shape& shape)
		{
			double l = Math::Inf;
			double b = Math::Inf;
			double r = -Math::Inf;
			double t = -Math::Inf;
			shape.bound(l, b, r, t);

			return{
				.xMin = std::round(l),
				.yMin = std::round(b),
				.xMax = std::round(r),
				.yMax = std::round(t)
			};
		}

		[[nodiscard]]
		static Image ToImage(const msdfgen::Bitmap<float, 4>& bitmap)
		{
			const int32 imageWidth	= bitmap.width();
			const int32 imageHeight	= bitmap.height();

			Image image{ Size{ imageWidth, imageHeight } };
			Color* pDst = image.data();

			const float* pLine = bitmap(0, (imageHeight - 1));
			const int32 stride = (4 * imageWidth);

			for (int32 y = (imageHeight - 1); 0 <= y; --y)
			{
				const float* pSrc = pLine;

				for (int32 x = 0; x < imageWidth; ++x)
				{
					pDst->r = static_cast<uint8>(Clamp(int32(*pSrc++ * 0x100), 0, 255));
					pDst->g = static_cast<uint8>(Clamp(int32(*pSrc++ * 0x100), 0, 255));
					pDst->b = static_cast<uint8>(Clamp(int32(*pSrc++ * 0x100), 0, 255));
					pDst->a = static_cast<uint8>(Clamp(int32(*pSrc++ * 0x100), 0, 255));
					++pDst;
				}

				pLine -= stride;
			}

			return image;
		}
	}

	MSDFGlyph RenderMSDFGlyph(const ::FT_Face face, const GlyphIndex glyphIndex, const FontFaceInfo& info, int32 bufferThickness, const ReadingDirection readingDirection)
	{
		{
			uint32 loadFlags = FT_LOAD_NO_HINTING;

			if (not(info.style & FontStyle::Bitmap))
			{
				loadFlags |= FT_LOAD_NO_BITMAP;
			}

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
		}

		bufferThickness = Max(0, bufferThickness);

		msdfgen::Shape shape;
		if (not GetShape(face, shape))
		{
			return{};
		}

		const GlyphBBox bbox = GetBound(shape);
		
		if (const bool isInf = (std::isinf(bbox.xMin) || std::isinf(bbox.xMax) || std::isinf(bbox.yMin) || std::isinf(bbox.yMax)))
		{
			MSDFGlyph glyph;
			glyph.glyphIndex		= glyphIndex;
			glyph.bufferThickness	= static_cast<int16>(bufferThickness);
			glyph.left				= 0.0f;
			glyph.top				= 0.0f;
			glyph.width				= 0.0f;
			glyph.height			= 0.0f;
			glyph.ascender			= info.ascender;
			glyph.descender			= info.descender;

			if (readingDirection == ReadingDirection::TopToBottom)
			{
				glyph.advance = (face->glyph->metrics.vertAdvance / 64.0f);

				if (glyph.advance == 0.0f)
				{
					glyph.advance = glyph.height;
				}
			}
			else
			{
				glyph.advance = (face->glyph->metrics.horiAdvance / 64.0f);
			}

			return glyph;
		}

		const int32 width	= static_cast<int32>(bbox.xMax - bbox.xMin);
		const int32 height	= static_cast<int32>(bbox.yMax - bbox.yMin);
		const Vec2 offset{ (-bbox.xMin + bufferThickness), (-bbox.yMin + bufferThickness) };
		const msdfgen::Projection projection{ 1.0, msdfgen::Vector2{ offset.x, offset.y } };
		
		msdfgen::MSDFGeneratorConfig generatorConfig;
		generatorConfig.overlapSupport = false;
		generatorConfig.errorCorrection.mode = msdfgen::ErrorCorrectionConfig::EDGE_PRIORITY;
		generatorConfig.errorCorrection.distanceCheckMode = msdfgen::ErrorCorrectionConfig::CHECK_DISTANCE_AT_EDGE;

		msdfgen::Bitmap<float, 4> bitmap{ (width + (2 * bufferThickness)), (height + (2 * bufferThickness)) };
		msdfgen::generateMTSDF(bitmap, shape, projection, 4.0, generatorConfig);

		MSDFGlyph glyph;
		glyph.glyphIndex		= glyphIndex;
		glyph.bufferThickness	= static_cast<int16>(bufferThickness);
		glyph.width				= static_cast<float>(width);
		glyph.height			= static_cast<float>(height);
		glyph.ascender			= info.ascender;
		glyph.descender			= info.descender;
		
		if (readingDirection == ReadingDirection::TopToBottom)
		{
			glyph.left		= (face->glyph->metrics.vertBearingX / 64.0f);
			glyph.top		= (face->glyph->metrics.vertBearingY / 64.0f);
			glyph.advance	= (face->glyph->metrics.vertAdvance / 64.0f);

			if ((glyph.left == 0.0f) && (glyph.top == 0.0f) && (glyph.advance == 0.0f))
			{
				glyph.left		= -(glyph.width / 2.0f);
				glyph.advance	= glyph.height;
			}
		}
		else
		{
			glyph.left		= static_cast<float>(bbox.xMin);
			glyph.top		= static_cast<float>(bbox.yMax);
			glyph.advance	= (face->glyph->metrics.horiAdvance / 64.0f);
		}
		
		glyph.image			= ToImage(bitmap);
		return glyph;
	}
}
