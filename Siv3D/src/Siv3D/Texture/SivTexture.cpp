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

# include <Siv3D/Texture.hpp>
# include <Siv3D/TextureRegion.hpp>
# include <Siv3D/FloatRect.hpp>
# include <Siv3D/Texture/ITexture.hpp>
# include <Siv3D/Renderer2D/IRenderer2D.hpp>
# include <Siv3D/Troubleshooting/Troubleshooting.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace
	{
		static void CheckEngine()
		{
			if (Siv3DEngine::isNull())
			{
				Troubleshooting::Show(Troubleshooting::Error::AssetInitializationBeforeEngineStartup, U"Texture");
				std::exit(EXIT_FAILURE);
			}
		}
	}

	template <>
	AssetIDWrapper<AssetHandle<Texture>>::AssetIDWrapper()
	{
		CheckEngine();
	}

	template <>
	AssetIDWrapper<AssetHandle<Texture>>::~AssetIDWrapper()
	{
		if (not Siv3DEngine::isAvailable())
		{
			return;
		}

		SIV3D_ENGINE(Texture)->release(m_id);
	}

	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	Texture::Texture() {}

	Texture::Texture(const Image& image, const TextureDesc desc)
		: AssetHandle{ (CheckEngine(), std::make_shared<AssetIDWrapperType>(SIV3D_ENGINE(Texture)->create(image, desc))) }
	{
		//SIV3D_ENGINE(AssetMonitor)->created();
	}

	Texture::Texture(const Image& image, const Array<Image>& mipmaps, const TextureDesc desc)
		: AssetHandle{ (CheckEngine(), std::make_shared<AssetIDWrapperType>(SIV3D_ENGINE(Texture)->create(image, mipmaps, desc))) }
	{
		//SIV3D_ENGINE(AssetMonitor)->created();
	}

	Texture::Texture(const FilePathView path, const TextureDesc desc)
		: Texture{ (CheckEngine(), Image{ path }), desc } {}

	Texture::Texture(IReader&& reader, const TextureDesc desc)
		: Texture{ (CheckEngine(), Image{ std::move(reader) }), desc } {}

	Texture::Texture(const FilePathView rgb, const FilePathView alpha, const TextureDesc desc)
		: Texture{ (CheckEngine(), Image{ rgb, alpha }), desc } {}

	Texture::Texture(const Color& rgb, const FilePathView alpha, const TextureDesc desc)
		: Texture{ (CheckEngine(), Image{ rgb, alpha }), desc } {}

	Texture::Texture(const Emoji& emoji, const TextureDesc desc)
		: Texture{ (CheckEngine(), Image{ emoji }), desc } {}

	Texture::Texture(const Emoji& emoji, const int32 size, const TextureDesc desc)
		: Texture{ (CheckEngine(), Image{ emoji, size }), desc } {}

	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	Texture::~Texture() {}

	////////////////////////////////////////////////////////////////
	//
	//	width, height
	//
	////////////////////////////////////////////////////////////////

	int32 Texture::width() const
	{
		return SIV3D_ENGINE(Texture)->getSize(m_handle->id()).x;
	}

	int32 Texture::height() const
	{
		return SIV3D_ENGINE(Texture)->getSize(m_handle->id()).y;
	}

	////////////////////////////////////////////////////////////////
	//
	//	size
	//
	////////////////////////////////////////////////////////////////

	Size Texture::size() const
	{
		return SIV3D_ENGINE(Texture)->getSize(m_handle->id());
	}

	////////////////////////////////////////////////////////////////
	//
	//	getDesc
	//
	////////////////////////////////////////////////////////////////

	TextureDesc Texture::getDesc() const
	{
		return SIV3D_ENGINE(Texture)->getDesc(m_handle->id());
	}

	////////////////////////////////////////////////////////////////
	//
	//	getDesc
	//
	////////////////////////////////////////////////////////////////

	bool Texture::hasMipmaps() const
	{
		return (1 < SIV3D_ENGINE(Texture)->getMipLevels(m_handle->id()));
	}

	////////////////////////////////////////////////////////////////
	//
	//	isSRGB
	//
	////////////////////////////////////////////////////////////////

	bool Texture::isSRGB() const
	{
		return SIV3D_ENGINE(Texture)->getDesc(m_handle->id()).sRGB;
	}

	////////////////////////////////////////////////////////////////
	//
	//	isSDF
	//
	////////////////////////////////////////////////////////////////

	bool Texture::isSDF() const
	{
		return SIV3D_ENGINE(Texture)->getDesc(m_handle->id()).isSDF;
	}

	////////////////////////////////////////////////////////////////
	//
	//	mipLevels
	//
	////////////////////////////////////////////////////////////////

	uint32 Texture::mipLevels() const
	{
		return SIV3D_ENGINE(Texture)->getMipLevels(m_handle->id());
	}

	////////////////////////////////////////////////////////////////
	//
	//	getFormat
	//
	////////////////////////////////////////////////////////////////

	TextureFormat Texture::getFormat() const
	{
		return SIV3D_ENGINE(Texture)->getFormat(m_handle->id());
	}

	////////////////////////////////////////////////////////////////
	//
	//	hasDepth
	//
	////////////////////////////////////////////////////////////////

	bool Texture::hasDepth() const
	{
		return SIV3D_ENGINE(Texture)->hasDepth(m_handle->id());
	}

	////////////////////////////////////////////////////////////////
	//
	//	swap
	//
	////////////////////////////////////////////////////////////////

	void Texture::swap(Texture& other) noexcept
	{
		m_handle.swap(other.m_handle);
	}

	////////////////////////////////////////////////////////////////
	//
	//	region
	//
	////////////////////////////////////////////////////////////////

	Rect Texture::region(const int32 x, const int32 y) const
	{
		return{ x, y, size() };
	}

	Rect Texture::region(const Point pos) const
	{
		return region(pos.x, pos.y);
	}

	RectF Texture::region(const double x, const double y) const
	{
		return{ x, y, size() };
	}

	RectF Texture::region(const Vec2 pos) const
	{
		return region(pos.x, pos.y);
	}

	////////////////////////////////////////////////////////////////
	//
	//	regionAt
	//
	////////////////////////////////////////////////////////////////

	RectF Texture::regionAt(const double x, const double y) const
	{
		return{ Arg::center(x, y), size() };
	}

	RectF Texture::regionAt(const Vec2 pos) const
	{
		return regionAt(pos.x, pos.y);
	}

	////////////////////////////////////////////////////////////////
	//
	//	draw
	//
	////////////////////////////////////////////////////////////////

	RectF Texture::draw(const ColorF& diffuse) const
	{
		return draw(0.0, 0.0, diffuse);
	}

	RectF Texture::draw(const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const
	{
		return draw(0.0, 0.0, topLeftColor, topRightColor, bottomRightColor, bottomLeftColor);
	}

	RectF Texture::draw(const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		return draw(0.0, 0.0, *topColor, *topColor, *bottomColor, *bottomColor);
	}

	RectF Texture::draw(const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		return draw(0.0, 0.0, *leftColor, *rightColor, *rightColor, *leftColor);
	}

	RectF Texture::draw(const double x, const double y, const ColorF& diffuse) const
	{
		const Size size = SIV3D_ENGINE(Texture)->getSize(m_handle->id());

		SIV3D_ENGINE(Renderer2D)->addTextureRegion(
			*this,
			FloatRect{ x, y, (x + size.x), (y + size.y) },
			FloatRect{ 0.0f, 0.0f, 1.0f, 1.0f },
			diffuse.toFloat4()
		);

		return{ x, y, size };
	}

	RectF Texture::draw(const double x, const double y, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const
	{
		const Size size = SIV3D_ENGINE(Texture)->getSize(m_handle->id());
		const Float4 colors[4] = { topLeftColor.toFloat4(), topRightColor.toFloat4(), bottomRightColor.toFloat4(), bottomLeftColor.toFloat4() };

		SIV3D_ENGINE(Renderer2D)->addTextureRegion(
			*this,
			FloatRect{ x, y, (x + size.x), (y + size.y) },
			FloatRect{ 0.0f, 0.0f, 1.0f, 1.0f },
			colors
		);

		return{ x, y, size };
	}

	RectF Texture::draw(const double x, const double y, const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		return draw(x, y, *topColor, *topColor, *bottomColor, *bottomColor);
	}

	RectF Texture::draw(const double x, const double y, const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		return draw(x, y, *leftColor, *rightColor, *rightColor, *leftColor);
	}

	RectF Texture::draw(const Vec2& pos, const ColorF& diffuse) const
	{
		return draw(pos.x, pos.y, diffuse);
	}

	RectF Texture::draw(const Vec2& pos, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const
	{
		return draw(pos.x, pos.y, topLeftColor, topRightColor, bottomRightColor, bottomLeftColor);
	}

	RectF Texture::draw(const Vec2& pos, const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		return draw(pos.x, pos.y, *topColor, *topColor, *bottomColor, *bottomColor);
	}

	RectF Texture::draw(const Vec2& pos, const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		return draw(pos.x, pos.y, *leftColor, *rightColor, *rightColor, *leftColor);
	}

	RectF Texture::draw(const Arg::topLeft_<Vec2> topLeft, const ColorF& diffuse) const
	{
		return draw(topLeft->x, topLeft->y, diffuse);
	}

	RectF Texture::draw(const Arg::topLeft_<Vec2> topLeft, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const
	{
		return draw(topLeft->x, topLeft->y, topLeftColor, topRightColor, bottomRightColor, bottomLeftColor);
	}

	RectF Texture::draw(const Arg::topLeft_<Vec2> topLeft, const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		return draw(topLeft->x, topLeft->y, *topColor, *topColor, *bottomColor, *bottomColor);
	}

	RectF Texture::draw(const Arg::topLeft_<Vec2> topLeft, const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		return draw(topLeft->x, topLeft->y, *leftColor, *rightColor, *rightColor, *leftColor);
	}

	RectF Texture::draw(const Arg::topRight_<Vec2> topRight, const ColorF& diffuse) const
	{
		return draw((topRight->x - width()), topRight->y, diffuse);
	}

	RectF Texture::draw(const Arg::topRight_<Vec2> topRight, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const
	{
		return draw((topRight->x - width()), topRight->y, topLeftColor, topRightColor, bottomRightColor, bottomLeftColor);
	}
	
	RectF Texture::draw(const Arg::topRight_<Vec2> topRight, const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		return draw((topRight->x - width()), topRight->y, *topColor, *topColor, *bottomColor, *bottomColor);
	}

	RectF Texture::draw(const Arg::topRight_<Vec2> topRight, const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		return draw((topRight->x - width()), topRight->y, *leftColor, *rightColor, *rightColor, *leftColor);
	}

	RectF Texture::draw(const Arg::bottomLeft_<Vec2> bottomLeft, const ColorF& diffuse) const
	{
		return draw(bottomLeft->x, (bottomLeft->y - height()), diffuse);
	}

	RectF Texture::draw(const Arg::bottomLeft_<Vec2> bottomLeft, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const
	{
		return draw(bottomLeft->x, (bottomLeft->y - height()), topLeftColor, topRightColor, bottomRightColor, bottomLeftColor);
	}

	RectF Texture::draw(const Arg::bottomLeft_<Vec2> bottomLeft, const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		return draw(bottomLeft->x, (bottomLeft->y - height()), *topColor, *topColor, *bottomColor, *bottomColor);
	}

	RectF Texture::draw(const Arg::bottomLeft_<Vec2> bottomLeft, const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		return draw(bottomLeft->x, (bottomLeft->y - height()), *leftColor, *rightColor, *rightColor, *leftColor);
	}

	RectF Texture::draw(const Arg::bottomRight_<Vec2> bottomRight, const ColorF& diffuse) const
	{
		return draw((bottomRight->x - width()), (bottomRight->y - height()), diffuse);
	}

	RectF Texture::draw(const Arg::bottomRight_<Vec2> bottomRight, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const
	{
		return draw((bottomRight->x - width()), (bottomRight->y - height()), topLeftColor, topRightColor, bottomRightColor, bottomLeftColor);
	}

	RectF Texture::draw(const Arg::bottomRight_<Vec2> bottomRight, const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		return draw((bottomRight->x - width()), (bottomRight->y - height()), *topColor, *topColor, *bottomColor, *bottomColor);
	}

	RectF Texture::draw(const Arg::bottomRight_<Vec2> bottomRight, const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		return draw((bottomRight->x - width()), (bottomRight->y - height()), *leftColor, *rightColor, *rightColor, *leftColor);
	}

	RectF Texture::draw(const Arg::topCenter_<Vec2> topCenter, const ColorF& diffuse) const
	{
		return draw((topCenter->x - (width() * 0.5)), topCenter->y, diffuse);
	}

	RectF Texture::draw(const Arg::topCenter_<Vec2> topCenter, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const
	{
		return draw((topCenter->x - (width() * 0.5)), topCenter->y, topLeftColor, topRightColor, bottomRightColor, bottomLeftColor);
	}

	RectF Texture::draw(const Arg::topCenter_<Vec2> topCenter, const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		return draw((topCenter->x - (width() * 0.5)), topCenter->y, *topColor, *topColor, *bottomColor, *bottomColor);
	}

	RectF Texture::draw(const Arg::topCenter_<Vec2> topCenter, const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		return draw((topCenter->x - (width() * 0.5)), topCenter->y, *leftColor, *rightColor, *rightColor, *leftColor);
	}

	RectF Texture::draw(const Arg::bottomCenter_<Vec2> bottomCenter, const ColorF& diffuse) const
	{
		return draw((bottomCenter->x - (width() * 0.5)), (bottomCenter->y - height()), diffuse);
	}

	RectF Texture::draw(const Arg::bottomCenter_<Vec2> bottomCenter, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const
	{
		return draw((bottomCenter->x - (width() * 0.5)), (bottomCenter->y - height()), topLeftColor, topRightColor, bottomRightColor, bottomLeftColor);
	}

	RectF Texture::draw(const Arg::bottomCenter_<Vec2> bottomCenter, const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		return draw((bottomCenter->x - (width() * 0.5)), (bottomCenter->y - height()), *topColor, *topColor, *bottomColor, *bottomColor);
	}

	RectF Texture::draw(const Arg::bottomCenter_<Vec2> bottomCenter, const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		return draw((bottomCenter->x - (width() * 0.5)), (bottomCenter->y - height()), *leftColor, *rightColor, *rightColor, *leftColor);
	}

	RectF Texture::draw(const Arg::leftCenter_<Vec2> leftCenter, const ColorF& diffuse) const
	{
		return draw(leftCenter->x, (leftCenter->y - (height() * 0.5)), diffuse);
	}

	RectF Texture::draw(const Arg::leftCenter_<Vec2> leftCenter, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const
	{
		return draw(leftCenter->x, (leftCenter->y - (height() * 0.5)), topLeftColor, topRightColor, bottomRightColor, bottomLeftColor);
	}

	RectF Texture::draw(const Arg::leftCenter_<Vec2> leftCenter, const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		return draw(leftCenter->x, (leftCenter->y - (height() * 0.5)), *topColor, *topColor, *bottomColor, *bottomColor);
	}

	RectF Texture::draw(const Arg::leftCenter_<Vec2> leftCenter, const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		return draw(leftCenter->x, (leftCenter->y - (height() * 0.5)), *leftColor, *rightColor, *rightColor, *leftColor);
	}

	RectF Texture::draw(const Arg::rightCenter_<Vec2>rightCenter, const ColorF& diffuse) const
	{
		return draw((rightCenter->x - width()), (rightCenter->y - (height() * 0.5)), diffuse);
	}

	RectF Texture::draw(const Arg::rightCenter_<Vec2>rightCenter, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const
	{
		return draw((rightCenter->x - width()), (rightCenter->y - (height() * 0.5)), topLeftColor, topRightColor, bottomRightColor, bottomLeftColor);
	}

	RectF Texture::draw(const Arg::rightCenter_<Vec2>rightCenter, const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		return draw((rightCenter->x - width()), (rightCenter->y - (height() * 0.5)), *topColor, *topColor, *bottomColor, *bottomColor);
	}

	RectF Texture::draw(const Arg::rightCenter_<Vec2>rightCenter, const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		return draw((rightCenter->x - width()), (rightCenter->y - (height() * 0.5)), *leftColor, *rightColor, *rightColor, *leftColor);
	}

	RectF Texture::draw(const Arg::center_<Vec2> center, const ColorF& diffuse) const
	{
		const Vec2 topLeft = center->movedBy(size() * -0.5);
		return draw(topLeft.x, topLeft.y, diffuse);
	}

	RectF Texture::draw(const Arg::center_<Vec2> center, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const
	{
		const Vec2 topLeft = center->movedBy(size() * -0.5);
		return draw(topLeft.x, topLeft.y, topLeftColor, topRightColor, bottomRightColor, bottomLeftColor);
	}

	RectF Texture::draw(const Arg::center_<Vec2> center, const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		const Vec2 topLeft = center->movedBy(size() * -0.5);
		return draw(topLeft.x, topLeft.y, *topColor, *topColor, *bottomColor, *bottomColor);
	}

	RectF Texture::draw(const Arg::center_<Vec2> center, const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		const Vec2 topLeft = center->movedBy(size() * -0.5);
		return draw(topLeft.x, topLeft.y, *leftColor, *rightColor, *rightColor, *leftColor);
	}

	RectF Texture::drawAt(const double x, const double y, const ColorF& diffuse) const
	{
		const Vec2 topLeft = Vec2{ x, y }.movedBy(size() * -0.5);
		return draw(topLeft.x, topLeft.y, diffuse);
	}

	RectF Texture::drawAt(const double x, const double y, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const
	{
		const Vec2 topLeft = Vec2{ x, y }.movedBy(size() * -0.5);
		return draw(topLeft.x, topLeft.y, topLeftColor, topRightColor, bottomRightColor, bottomLeftColor);
	}

	RectF Texture::drawAt(const double x, const double y, const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		const Vec2 topLeft = Vec2{ x, y }.movedBy(size() * -0.5);
		return draw(topLeft.x, topLeft.y, *topColor, *topColor, *bottomColor, *bottomColor);
	}

	RectF Texture::drawAt(const double x, const double y, const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		const Vec2 topLeft = Vec2{ x, y }.movedBy(size() * -0.5);
		return draw(topLeft.x, topLeft.y, *leftColor, *rightColor, *rightColor, *leftColor);
	}

	RectF Texture::drawAt(const Vec2& pos, const ColorF& diffuse) const
	{
		const Vec2 topLeft = pos.movedBy(size() * -0.5);
		return draw(topLeft.x, topLeft.y, diffuse);
	}

	RectF Texture::drawAt(const Vec2& pos, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const
	{
		const Vec2 topLeft = pos.movedBy(size() * -0.5);
		return draw(topLeft.x, topLeft.y, topLeftColor, topRightColor, bottomRightColor, bottomLeftColor);
	}

	RectF Texture::drawAt(const Vec2& pos, const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		const Vec2 topLeft = pos.movedBy(size() * -0.5);
		return draw(topLeft.x, topLeft.y, *topColor, *topColor, *bottomColor, *bottomColor);
	}

	RectF Texture::drawAt(const Vec2& pos, const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		const Vec2 topLeft = pos.movedBy(size() * -0.5);
		return draw(topLeft.x, topLeft.y, *leftColor, *rightColor, *rightColor, *leftColor);
	}






	////////////////////////////////////////////////////////////////
	//
	//	operator ()
	//
	////////////////////////////////////////////////////////////////

	TextureRegion Texture::operator ()(const double x, const double y, const double size) const
	{
		return operator()(x, y, size, size);
	}

	TextureRegion Texture::operator ()(const double x, const double y, const double w, const double h) const
	{
		const Size size = SIV3D_ENGINE(Texture)->getSize(m_handle->id());
		const double invSizeX = (1.0 / size.x);
		const double invSizeY = (1.0 / size.y);
		return{
			*this,
			static_cast<float>(x * invSizeX),
			static_cast<float>(y * invSizeY),
			static_cast<float>((x + w) * invSizeX),
			static_cast<float>((y + h) * invSizeY),
			w,
			h
		};
	}

	TextureRegion Texture::operator ()(const Vec2& xy, const double w, const double h) const
	{
		return operator()(xy.x, xy.y, w, h);
	}

	TextureRegion Texture::operator ()(const double x, const double y, const Vec2& size) const
	{
		return operator()(x, y, size.x, size.y);
	}

	TextureRegion Texture::operator ()(const Vec2& xy, const double size) const
	{
		return operator()(xy.x, xy.y, size, size);
	}

	TextureRegion Texture::operator ()(const Vec2& xy, const Vec2& size) const
	{
		return operator()(xy.x, xy.y, size.x, size.y);
	}

	TextureRegion Texture::operator ()(const RectF& rect) const
	{
		return operator()(rect.pos.x, rect.pos.y, rect.size.x, rect.size.y);
	}
}
