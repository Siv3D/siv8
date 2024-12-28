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

# include <Siv3D/TextureRegion.hpp>
# include <Siv3D/FloatQuad.hpp>
# include <Siv3D/Renderer2D/IRenderer2D.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	TextureRegion::TextureRegion(const Texture& _texture)
		: texture{ _texture }
		, uvRect{ 0.0f, 0.0f, 1.0f, 1.0f }
		, size{ _texture.size() } {}

	TextureRegion::TextureRegion(const Texture& _texture, const double width, const double height)
		: texture{ _texture }
		, uvRect{ 0.0f, 0.0f, 1.0f, 1.0f }
		, size{ width, height } {}

	TextureRegion::TextureRegion(const Texture& _texture, const SizeF& _size)
		: texture{ _texture }
		, uvRect{ 0.0f, 0.0f, 1.0f, 1.0f }
		, size{ _size } {}

	TextureRegion::TextureRegion(
		const Texture& _texture,
		const float l, const float t, const float r, const float b, const double width, const double height)
		: texture{ _texture }
		, uvRect{ l, t, r, b }
		, size{ width, height } {}

	TextureRegion::TextureRegion(
		const Texture& _texture,
		const float l, const float t, const float r, const float b, const SizeF& _size)
		: texture{ _texture }
		, uvRect{ l, t, r, b }
		, size{ _size } {}

	TextureRegion::TextureRegion(const Texture& _texture, const FloatRect& _uvRect, const double width, const double height)
		: texture{ _texture }
		, uvRect{ _uvRect }
		, size{ width, height } {}

	TextureRegion::TextureRegion(const Texture& _texture, const FloatRect& _uvRect, const SizeF& _size)
		: texture{ _texture }
		, uvRect{ _uvRect }
		, size{ _size } {}

	////////////////////////////////////////////////////////////////
	//
	//	swap
	//
	////////////////////////////////////////////////////////////////

	void TextureRegion::swap(TextureRegion& other) noexcept
	{
		texture.swap(other.texture);
		std::ranges::swap(uvRect, other.uvRect);
		std::ranges::swap(size, other.size);
	}

	////////////////////////////////////////////////////////////////
	//
	//	region
	//
	////////////////////////////////////////////////////////////////

	RectF TextureRegion::region(const double x, const double y) const noexcept
	{
		return{ x, y, size };
	}

	RectF TextureRegion::region(const Vec2 pos) const noexcept
	{
		return{ pos, size };
	}

	RectF TextureRegion::regionAt(const double x, const double y) const noexcept
	{
		return{ (x - size.x * 0.5), (y - size.y * 0.5), size };
	}

	RectF TextureRegion::regionAt(const Vec2 pos) const noexcept
	{
		return regionAt(pos.x, pos.y);
	}

	////////////////////////////////////////////////////////////////
	//
	//	stretched
	//
	////////////////////////////////////////////////////////////////

	TextureRegion TextureRegion::stretched(const double x, const double y) const
	{
		TextureRegion result{ *this };
		result.size.x += static_cast<float>(x * 2);
		result.size.y += static_cast<float>(y * 2);

		const Size baseTextureSize = texture.size();
		const float uStretch = static_cast<float>(x / baseTextureSize.x);
		const float vStretch = static_cast<float>(y / baseTextureSize.y);
		result.uvRect.left		-= uStretch;
		result.uvRect.right		+= uStretch;
		result.uvRect.top		-= vStretch;
		result.uvRect.bottom	+= vStretch;

		return result;
	}

	TextureRegion TextureRegion::stretched(const Vec2 _size) const
	{
		return stretched(_size.x, _size.y);
	}

	TextureRegion TextureRegion::stretched(const double top, const double right, const double bottom, const double left) const
	{
		TextureRegion result{ *this };
		result.size.x += static_cast<float>(left + right);
		result.size.y += static_cast<float>(top + bottom);

		const Size baseTextureSize = texture.size();
		const double du = (1.0 / baseTextureSize.x);
		const double dv = (1.0 / baseTextureSize.y);
		result.uvRect.left		-= static_cast<float>(du * left);
		result.uvRect.right		+= static_cast<float>(du * right);
		result.uvRect.top		-= static_cast<float>(dv * top);
		result.uvRect.bottom	+= static_cast<float>(dv * bottom);

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	draw
	//
	////////////////////////////////////////////////////////////////

	RectF TextureRegion::draw(const ColorF& diffuse) const
	{
		return draw(0.0, 0.0, diffuse);
	}

	RectF TextureRegion::draw(const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const
	{
		return draw(0.0, 0.0, topLeftColor, topRightColor, bottomRightColor, bottomLeftColor);
	}

	RectF TextureRegion::draw(const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		return draw(0.0, 0.0, *topColor, *topColor, *bottomColor, *bottomColor);
	}

	RectF TextureRegion::draw(const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		return draw(0.0, 0.0, *leftColor, *rightColor, *rightColor, *leftColor);
	}

	RectF TextureRegion::draw(const double x, const double y, const ColorF& diffuse) const
	{
		SIV3D_ENGINE(Renderer2D)->addTextureRegion(
			texture,
			FloatRect{ x, y, (x + size.x), (y + size.y) },
			uvRect,
			diffuse.toFloat4()
		);

		return{ x, y, size };
	}

	RectF TextureRegion::draw(const double x, const double y, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const
	{
		const Float4 colors[4] = { topLeftColor.toFloat4(), topRightColor.toFloat4(), bottomRightColor.toFloat4(), bottomLeftColor.toFloat4() };

		SIV3D_ENGINE(Renderer2D)->addTextureRegion(
			texture,
			FloatRect{ x, y, (x + size.x), (y + size.y) },
			uvRect,
			colors
		);

		return{ x, y, size };
	}

	RectF TextureRegion::draw(const double x, const double y, const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		return draw(x, y, *topColor, *topColor, *bottomColor, *bottomColor);
	}

	RectF TextureRegion::draw(const double x, const double y, const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		return draw(x, y, *leftColor, *rightColor, *rightColor, *leftColor);
	}

	RectF TextureRegion::draw(const Vec2& pos, const ColorF& diffuse) const
	{
		return draw(pos.x, pos.y, diffuse);
	}

	RectF TextureRegion::draw(const Vec2& pos, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const
	{
		return draw(pos.x, pos.y, topLeftColor, topRightColor, bottomRightColor, bottomLeftColor);
	}

	RectF TextureRegion::draw(const Vec2& pos, const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		return draw(pos.x, pos.y, *topColor, *topColor, *bottomColor, *bottomColor);
	}

	RectF TextureRegion::draw(const Vec2& pos, const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		return draw(pos.x, pos.y, *leftColor, *rightColor, *rightColor, *leftColor);
	}

	RectF TextureRegion::draw(const Arg::topLeft_<Vec2> topLeft, const ColorF& diffuse) const
	{
		return draw(topLeft->x, topLeft->y, diffuse);
	}

	RectF TextureRegion::draw(const Arg::topLeft_<Vec2> topLeft, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const
	{
		return draw(topLeft->x, topLeft->y, topLeftColor, topRightColor, bottomRightColor, bottomLeftColor);
	}

	RectF TextureRegion::draw(const Arg::topLeft_<Vec2> topLeft, const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		return draw(topLeft->x, topLeft->y, *topColor, *topColor, *bottomColor, *bottomColor);
	}

	RectF TextureRegion::draw(const Arg::topLeft_<Vec2> topLeft, const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		return draw(topLeft->x, topLeft->y, *leftColor, *rightColor, *rightColor, *leftColor);
	}

	RectF TextureRegion::draw(const Arg::topRight_<Vec2> topRight, const ColorF& diffuse) const
	{
		return draw((topRight->x - size.x), topRight->y, diffuse);
	}

	RectF TextureRegion::draw(const Arg::topRight_<Vec2> topRight, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const
	{
		return draw((topRight->x - size.x), topRight->y, topLeftColor, topRightColor, bottomRightColor, bottomLeftColor);
	}

	RectF TextureRegion::draw(const Arg::topRight_<Vec2> topRight, const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		return draw((topRight->x - size.x), topRight->y, *topColor, *topColor, *bottomColor, *bottomColor);
	}

	RectF TextureRegion::draw(const Arg::topRight_<Vec2> topRight, const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		return draw((topRight->x - size.x), topRight->y, *leftColor, *rightColor, *rightColor, *leftColor);
	}

	RectF TextureRegion::draw(const Arg::bottomRight_<Vec2> bottomRight, const ColorF& diffuse) const
	{
		return draw((bottomRight->x - size.x), (bottomRight->y - size.y), diffuse);
	}

	RectF TextureRegion::draw(const Arg::bottomRight_<Vec2> bottomRight, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const
	{
		return draw((bottomRight->x - size.x), (bottomRight->y - size.y), topLeftColor, topRightColor, bottomRightColor, bottomLeftColor);
	}

	RectF TextureRegion::draw(const Arg::bottomRight_<Vec2> bottomRight, const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		return draw((bottomRight->x - size.x), (bottomRight->y - size.y), *topColor, *topColor, *bottomColor, *bottomColor);
	}

	RectF TextureRegion::draw(const Arg::bottomRight_<Vec2> bottomRight, const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		return draw((bottomRight->x - size.x), (bottomRight->y - size.y), *leftColor, *rightColor, *rightColor, *leftColor);
	}

	RectF TextureRegion::draw(const Arg::bottomLeft_<Vec2> bottomLeft, const ColorF& diffuse) const
	{
		return draw(bottomLeft->x, (bottomLeft->y - size.y), diffuse);
	}

	RectF TextureRegion::draw(const Arg::bottomLeft_<Vec2> bottomLeft, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const
	{
		return draw(bottomLeft->x, (bottomLeft->y - size.y), topLeftColor, topRightColor, bottomRightColor, bottomLeftColor);
	}

	RectF TextureRegion::draw(const Arg::bottomLeft_<Vec2> bottomLeft, const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		return draw(bottomLeft->x, (bottomLeft->y - size.y), *topColor, *topColor, *bottomColor, *bottomColor);
	}

	RectF TextureRegion::draw(const Arg::bottomLeft_<Vec2> bottomLeft, const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		return draw(bottomLeft->x, (bottomLeft->y - size.y), *leftColor, *rightColor, *rightColor, *leftColor);
	}

	RectF TextureRegion::draw(const Arg::topCenter_<Vec2> topCenter, const ColorF& diffuse) const
	{
		return draw((topCenter->x - (size.x * 0.5)), topCenter->y, diffuse);
	}

	RectF TextureRegion::draw(const Arg::topCenter_<Vec2> topCenter, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const
	{
		return draw((topCenter->x - (size.x * 0.5)), topCenter->y, topLeftColor, topRightColor, bottomRightColor, bottomLeftColor);
	}

	RectF TextureRegion::draw(const Arg::topCenter_<Vec2> topCenter, const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		return draw((topCenter->x - (size.x * 0.5)), topCenter->y, *topColor, *topColor, *bottomColor, *bottomColor);
	}

	RectF TextureRegion::draw(const Arg::topCenter_<Vec2> topCenter, const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		return draw((topCenter->x - (size.x * 0.5)), topCenter->y, *leftColor, *rightColor, *rightColor, *leftColor);
	}

	RectF TextureRegion::draw(const Arg::bottomCenter_<Vec2> bottomCenter, const ColorF& diffuse) const
	{
		return draw((bottomCenter->x - (size.x * 0.5)), (bottomCenter->y - size.y), diffuse);
	}

	RectF TextureRegion::draw(const Arg::bottomCenter_<Vec2> bottomCenter, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const
	{
		return draw((bottomCenter->x - (size.x * 0.5)), (bottomCenter->y - size.y), topLeftColor, topRightColor, bottomRightColor, bottomLeftColor);
	}

	RectF TextureRegion::draw(const Arg::bottomCenter_<Vec2> bottomCenter, const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		return draw((bottomCenter->x - (size.x * 0.5)), (bottomCenter->y - size.y), *topColor, *topColor, *bottomColor, *bottomColor);
	}

	RectF TextureRegion::draw(const Arg::bottomCenter_<Vec2> bottomCenter, const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		return draw((bottomCenter->x - (size.x * 0.5)), (bottomCenter->y - size.y), *leftColor, *rightColor, *rightColor, *leftColor);
	}

	RectF TextureRegion::draw(const Arg::leftCenter_<Vec2> leftCenter, const ColorF& diffuse) const
	{
		return draw(leftCenter->x, (leftCenter->y - (size.y * 0.5)), diffuse);
	}

	RectF TextureRegion::draw(const Arg::leftCenter_<Vec2> leftCenter, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const
	{
		return draw(leftCenter->x, (leftCenter->y - (size.y * 0.5)), topLeftColor, topRightColor, bottomRightColor, bottomLeftColor);
	}

	RectF TextureRegion::draw(const Arg::leftCenter_<Vec2> leftCenter, const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		return draw(leftCenter->x, (leftCenter->y - (size.y * 0.5)), *topColor, *topColor, *bottomColor, *bottomColor);
	}

	RectF TextureRegion::draw(const Arg::leftCenter_<Vec2> leftCenter, const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		return draw(leftCenter->x, (leftCenter->y - (size.y * 0.5)), *leftColor, *rightColor, *rightColor, *leftColor);
	}

	RectF TextureRegion::draw(const Arg::rightCenter_<Vec2> rightCenter, const ColorF& diffuse) const
	{
		return draw((rightCenter->x - size.x), (rightCenter->y - (size.y * 0.5)), diffuse);
	}

	RectF TextureRegion::draw(const Arg::rightCenter_<Vec2> rightCenter, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const
	{
		return draw((rightCenter->x - size.x), (rightCenter->y - (size.y * 0.5)), topLeftColor, topRightColor, bottomRightColor, bottomLeftColor);
	}

	RectF TextureRegion::draw(const Arg::rightCenter_<Vec2> rightCenter, const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		return draw((rightCenter->x - size.x), (rightCenter->y - (size.y * 0.5)), *topColor, *topColor, *bottomColor, *bottomColor);
	}

	RectF TextureRegion::draw(const Arg::rightCenter_<Vec2> rightCenter, const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		return draw((rightCenter->x - size.x), (rightCenter->y - (size.y * 0.5)), *leftColor, *rightColor, *rightColor, *leftColor);
	}

	RectF TextureRegion::draw(const Arg::center_<Vec2> center, const ColorF& diffuse) const
	{
		return draw((center->x - (size.x * 0.5)), (center->y - (size.y * 0.5)), diffuse);
	}

	RectF TextureRegion::draw(const Arg::center_<Vec2> center, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const
	{
		return draw((center->x - (size.x * 0.5)), (center->y - (size.y * 0.5)), topLeftColor, topRightColor, bottomRightColor, bottomLeftColor);
	}

	RectF TextureRegion::draw(const Arg::center_<Vec2> center, const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		return draw((center->x - (size.x * 0.5)), (center->y - (size.y * 0.5)), *topColor, *topColor, *bottomColor, *bottomColor);
	}

	RectF TextureRegion::draw(const Arg::center_<Vec2> center, const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		return draw((center->x - (size.x * 0.5)), (center->y - (size.y * 0.5)), *leftColor, *rightColor, *rightColor, *leftColor);
	}

	////////////////////////////////////////////////////////////////
	//
	//	drawAt
	//
	////////////////////////////////////////////////////////////////

	RectF TextureRegion::drawAt(const double x, const double y, const ColorF& diffuse) const
	{
		return draw((x - size.x * 0.5), (y - size.y * 0.5), diffuse);
	}

	RectF TextureRegion::drawAt(const double x, const double y, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const
	{
		return draw((x - size.x * 0.5), (y - size.y * 0.5), topLeftColor, topRightColor, bottomRightColor, bottomLeftColor);
	}

	RectF TextureRegion::drawAt(const double x, const double y, const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		return draw((x - size.x * 0.5), (y - size.y * 0.5), *topColor, *topColor, *bottomColor, *bottomColor);
	}

	RectF TextureRegion::drawAt(const double x, const double y, const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		return draw((x - size.x * 0.5), (y - size.y * 0.5), *leftColor, *rightColor, *rightColor, *leftColor);
	}

	RectF TextureRegion::drawAt(const Vec2& pos, const ColorF& diffuse) const
	{
		return draw((pos.x - size.x * 0.5), (pos.y - size.y * 0.5), diffuse);
	}

	RectF TextureRegion::drawAt(const Vec2& pos, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const
	{
		return draw((pos.x - size.x * 0.5), (pos.y - size.y * 0.5), topLeftColor, topRightColor, bottomRightColor, bottomLeftColor);
	}

	RectF TextureRegion::drawAt(const Vec2& pos, const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		return draw((pos.x - size.x * 0.5), (pos.y - size.y * 0.5), *topColor, *topColor, *bottomColor, *bottomColor);
	}

	RectF TextureRegion::drawAt(const Vec2& pos, const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		return draw((pos.x - size.x * 0.5), (pos.y - size.y * 0.5), *leftColor, *rightColor, *rightColor, *leftColor);
	}



	////////////////////////////////////////////////////////////////
	//
	//	drawQuadWarp
	//
	////////////////////////////////////////////////////////////////

	bool TextureRegion::drawQuadWarp(const Quad& quad, const ColorF& diffuse) const
	{
		if (not quad.isConvex())
		{
			return false;
		}

		SIV3D_ENGINE(Renderer2D)->addQuadWarp(
			texture,
			uvRect,
			FloatQuad{ quad },
			diffuse.toFloat4()
		);

		return true;
	}

	bool TextureRegion::drawQuadWarp(const Quad& quad, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const
	{
		if (not quad.isConvex())
		{
			return false;
		}
		
		const Float4 colors[4] = { topLeftColor.toFloat4(), topRightColor.toFloat4(), bottomRightColor.toFloat4(), bottomLeftColor.toFloat4() };
		
		SIV3D_ENGINE(Renderer2D)->addQuadWarp(
			texture,
			uvRect,
			FloatQuad{ quad },
			colors
		);
		
		return true;
	}

	bool TextureRegion::drawQuadWarp(const Quad& quad, const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		return drawQuadWarp(quad, *topColor, *topColor, *bottomColor, *bottomColor);
	}

	bool TextureRegion::drawQuadWarp(const Quad& quad, const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		return drawQuadWarp(quad, *leftColor, *rightColor, *rightColor, *leftColor);
	}

}
