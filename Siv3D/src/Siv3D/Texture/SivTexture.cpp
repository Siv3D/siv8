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

# include <Siv3D/Texture.hpp>
# include <Siv3D/TextureRegion.hpp>
# include <Siv3D/TexturedQuad.hpp>
# include <Siv3D/TexturedRoundRect.hpp>
# include <Siv3D/FloatRect.hpp>
# include <Siv3D/FloatQuad.hpp>
# include <Siv3D/Texture/ITexture.hpp>
# include <Siv3D/Renderer2D/IRenderer2D.hpp>
# include <Siv3D/AssetMonitor/IAssetMonitor.hpp>
# include <Siv3D/Troubleshooting/Troubleshooting.hpp>
# include <Siv3D/Engine/Siv3DEngine.hpp>

# if SIV3D_PLATFORM(WINDOWS)
#	include <Siv3D/Texture/D3D11/CTexture_D3D11.hpp>
# endif

namespace s3d
{
	namespace
	{
		static void CheckEngine(const StringView type = U"Texture")
		{
			if (Siv3DEngine::isNull())
			{
				Troubleshooting::Show(Troubleshooting::Error::AssetInitializationBeforeEngineStartup, type);
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
		SIV3D_ENGINE(AssetMonitor)->reportAssetCreation();
	}

	Texture::Texture(const Image& image, const Array<Image>& mipmaps, const TextureDesc desc)
		: AssetHandle{ (CheckEngine(), std::make_shared<AssetIDWrapperType>(SIV3D_ENGINE(Texture)->create(image, mipmaps, desc))) }
	{
		SIV3D_ENGINE(AssetMonitor)->reportAssetCreation();
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

	Texture::Texture(const Size& size, const void* pData, const size_t size_bytes, const TextureFormat& format, const TextureDesc desc)
		: AssetHandle{ (CheckEngine(), std::make_shared<AssetIDWrapperType>(SIV3D_ENGINE(Texture)->create(size, pData, size_bytes, format, desc))) }
	{
		SIV3D_ENGINE(AssetMonitor)->reportAssetCreation();
	}

	Texture::Texture(Dynamic, const Size& size, const void* pData, const uint32 stride, const TextureFormat& format, const TextureDesc desc)
		: AssetHandle{ (CheckEngine(U"DynamicTexture"), std::make_shared<AssetIDWrapperType>(SIV3D_ENGINE(Texture)->createDynamic(size, pData, stride, format, desc))) }
	{
		SIV3D_ENGINE(AssetMonitor)->reportAssetCreation();
	}

	Texture::Texture(Dynamic, const Size& size, const ColorF& color, const TextureFormat& format, const TextureDesc desc)
		: AssetHandle{ (CheckEngine(U"DynamicTexture"), std::make_shared<AssetIDWrapperType>(SIV3D_ENGINE(Texture)->createDynamic(size, color, format, desc))) }
	{
		SIV3D_ENGINE(AssetMonitor)->reportAssetCreation();
	}

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

		SIV3D_ENGINE(Renderer2D)->addTexturedQuad(
			*this,
			FloatQuad::FromRect(x, y, size.x, size.y),
			FloatRect{ 0.0f, 0.0f, 1.0f, 1.0f },
			diffuse.toFloat4()
		);

		return{ x, y, size };
	}

	RectF Texture::draw(const double x, const double y, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const
	{
		const Size size = SIV3D_ENGINE(Texture)->getSize(m_handle->id());
		const Float4 colors[4] = { topLeftColor.toFloat4(), topRightColor.toFloat4(), bottomRightColor.toFloat4(), bottomLeftColor.toFloat4() };

		SIV3D_ENGINE(Renderer2D)->addTexturedQuad(
			*this,
			FloatQuad::FromRect(x, y, size.x, size.y),
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
	//	drawQuadWarp
	//
	////////////////////////////////////////////////////////////////

	bool Texture::drawQuadWarp(const Quad& quad, const ColorF& diffuse) const
	{
		if (not quad.isConvex())
		{
			return false;
		}

		SIV3D_ENGINE(Renderer2D)->addQuadWarp(
			*this,
			FloatRect{ 0.0f, 0.0f, 1.0f, 1.0f },
			FloatQuad{ quad },
			diffuse.toFloat4()
		);

		return true;
	}

	bool Texture::drawQuadWarp(const Quad& quad, const ColorF& topLeftColor, const ColorF& topRightColor, const ColorF& bottomRightColor, const ColorF& bottomLeftColor) const
	{
		if (not quad.isConvex())
		{
			return false;
		}
		
		const Float4 colors[4] = { topLeftColor.toFloat4(), topRightColor.toFloat4(), bottomRightColor.toFloat4(), bottomLeftColor.toFloat4() };
		
		SIV3D_ENGINE(Renderer2D)->addQuadWarp(
			*this,
			FloatRect{ 0.0f, 0.0f, 1.0f, 1.0f },
			FloatQuad{ quad },
			colors
		);

		return true;
	}

	bool Texture::drawQuadWarp(const Quad& quad, const Arg::top_<ColorF> topColor, const Arg::bottom_<ColorF> bottomColor) const
	{
		return drawQuadWarp(quad, *topColor, *topColor, *bottomColor, *bottomColor);
	}

	bool Texture::drawQuadWarp(const Quad& quad, const Arg::left_<ColorF> leftColor, const Arg::right_<ColorF> rightColor) const
	{
		return drawQuadWarp(quad, *leftColor, *rightColor, *rightColor, *leftColor);
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

	TextureRegion Texture::operator ()(const double x, const double y, const SizeF& size) const
	{
		return operator()(x, y, size.x, size.y);
	}

	TextureRegion Texture::operator ()(const Vec2& xy, const double size) const
	{
		return operator()(xy.x, xy.y, size, size);
	}

	TextureRegion Texture::operator ()(const Vec2& xy, const SizeF& size) const
	{
		return operator()(xy.x, xy.y, size.x, size.y);
	}

	TextureRegion Texture::operator ()(const RectF& rect) const
	{
		return operator()(rect.pos.x, rect.pos.y, rect.size.x, rect.size.y);
	}

	////////////////////////////////////////////////////////////////
	//
	//	uv
	//
	////////////////////////////////////////////////////////////////

	TextureRegion Texture::uv(const double u, const double v, const double w, const double h) const
	{
		const Size size = SIV3D_ENGINE(Texture)->getSize(m_handle->id());

		return{
			*this,
			static_cast<float>(u),
			static_cast<float>(v),
			static_cast<float>(u + w),
			static_cast<float>(v + h),
			(size.x * w),
			(size.y * h)
		};
	}

	TextureRegion Texture::uv(const double u, const double v, const double uvSize) const
	{
		return uv(u, v, uvSize, uvSize);
	}

	TextureRegion Texture::uv(const RectF& rect) const
	{
		return uv(rect.x, rect.y, rect.w, rect.h);
	}

	////////////////////////////////////////////////////////////////
	//
	//	mirrored
	//
	////////////////////////////////////////////////////////////////

	TextureRegion Texture::mirrored() const
	{
		return{ *this, FloatRect{ 1.0f, 0.0f, 0.0f, 1.0f }, size() };
	}

	TextureRegion Texture::mirrored(const bool doMirror) const
	{
		if (doMirror)
		{
			return mirrored();
		}
		else
		{
			return{ *this };
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	flipped
	//
	////////////////////////////////////////////////////////////////

	TextureRegion Texture::flipped() const
	{
		return{ *this, FloatRect{ 0.0f, 1.0f, 1.0f, 0.0f }, size() };
	}

	TextureRegion Texture::flipped(const bool doFlip) const
	{
		if (doFlip)
		{
			return flipped();
		}
		else
		{
			return{ *this };
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	scaled
	//
	////////////////////////////////////////////////////////////////

	TextureRegion Texture::scaled(const double s) const
	{
		return scaled(s, s);
	}

	TextureRegion Texture::scaled(const double xs, const double ys) const
	{
		const Size size = SIV3D_ENGINE(Texture)->getSize(m_handle->id());

		return{ *this, (size.x * xs), (size.y * ys) };
	}

	TextureRegion Texture::scaled(const SizeF s) const
	{
		return scaled(s.x, s.y);
	}

	////////////////////////////////////////////////////////////////
	//
	//	resized
	//
	////////////////////////////////////////////////////////////////

	TextureRegion Texture::resized(const double size) const
	{
		const Size baseSize = SIV3D_ENGINE(Texture)->getSize(m_handle->id());

		return scaled(static_cast<double>(size) / Max(baseSize.x, baseSize.y));
	}

	TextureRegion Texture::resized(const double width, const double height) const
	{
		return{ *this, width, height };
	}

	TextureRegion Texture::resized(const SizeF size) const
	{
		return resized(size.x, size.y);
	}

	////////////////////////////////////////////////////////////////
	//
	//	repeated
	//
	////////////////////////////////////////////////////////////////

	TextureRegion Texture::repeated(const double repeat) const
	{
		return repeated(repeat, repeat);
	}

	TextureRegion Texture::repeated(const double xRepeat, const double yRepeat) const
	{
		const Size baseSize = SIV3D_ENGINE(Texture)->getSize(m_handle->id());

		return{
			*this,
			0.0f,
			0.0f,
			static_cast<float>(xRepeat),
			static_cast<float>(yRepeat),
			(baseSize.x * xRepeat),
			(baseSize.y * yRepeat)
		};
	}

	TextureRegion Texture::repeated(const SizeF repeat) const
	{
		return repeated(repeat.x, repeat.y);
	}

	////////////////////////////////////////////////////////////////
	//
	//	mapped
	//
	////////////////////////////////////////////////////////////////

	TextureRegion Texture::mapped(const double size) const
	{
		return mapped(size, size);
	}

	TextureRegion Texture::mapped(const double width, const double height) const
	{
		const Size baseSize = SIV3D_ENGINE(Texture)->getSize(m_handle->id());

		return{
			*this,
			0.0f,
			0.0f,
			static_cast<float>(width / baseSize.x),
			static_cast<float>(height / baseSize.y),
			width,
			height
		};
	}

	TextureRegion Texture::mapped(const SizeF size) const
	{
		return mapped(size.x, size.y);
	}

	////////////////////////////////////////////////////////////////
	//
	//	fitted
	//
	////////////////////////////////////////////////////////////////

	TextureRegion Texture::fitted(const double size, const AllowUpscale allowUpscale) const
	{
		return fitted(size, size, allowUpscale);
	}

	TextureRegion Texture::fitted(double width, double height, const AllowUpscale allowUpscale) const
	{
		const Size baseSize = SIV3D_ENGINE(Texture)->getSize(m_handle->id());

		if (not allowUpscale)
		{
			width	= Min<double>(width, baseSize.x);
			height	= Min<double>(height, baseSize.y);
		}

		// どれだけの倍率で拡大縮小するか
		double ws = (width / baseSize.x);
		double hs = (height / baseSize.y);
		double targetWidth, targetHeight;

		if (ws < hs)
		{
			targetWidth		= width;
			targetHeight	= (baseSize.y * ws);
		}
		else
		{
			targetWidth		= (baseSize.x * hs);
			targetHeight	= height;
		}

		return{ *this, targetWidth, targetHeight };
	}

	TextureRegion Texture::fitted(const SizeF& size, const AllowUpscale allowUpscale) const
	{
		return fitted(size.x, size.y, allowUpscale);
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotated
	//
	////////////////////////////////////////////////////////////////

	TexturedQuad Texture::rotated(const double angle) const
	{
		const Size size = SIV3D_ENGINE(Texture)->getSize(m_handle->id());

		return{
			*this,
			0.0f, 0.0f, 1.0f, 1.0f,
			Rect{ size }.rotated(angle),
			Float2{ (size.x * 0.5f), (size.y * 0.5f) }
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	rotatedAt
	//
	////////////////////////////////////////////////////////////////

	TexturedQuad Texture::rotatedAt(const double x, const double y, const double angle) const
	{
		const Size baseSize = SIV3D_ENGINE(Texture)->getSize(m_handle->id());

		return{
			*this,
			0.0f, 0.0f, 1.0f, 1.0f,
			Rect{ baseSize }.rotatedAt(x, y, angle),
			Float2{ x, y }
		};
	}

	TexturedQuad Texture::rotatedAt(const Vec2& pos, const double angle) const
	{
		return rotatedAt(pos.x, pos.y, angle);
	}

	////////////////////////////////////////////////////////////////
	//
	//	rounded
	//
	////////////////////////////////////////////////////////////////

	TexturedRoundRect Texture::rounded(const double r) const
	{
		const Size baseSize = SIV3D_ENGINE(Texture)->getSize(m_handle->id());
		const double round = Min(Abs(r), (Min(baseSize.x, baseSize.y) * 0.5));

		return{
			*this,
			0.0f, 0.0f, 1.0f, 1.0f,
			RoundRect{ 0, 0, baseSize, round }
		};
	}

	TexturedRoundRect Texture::rounded(const double x, const double y, const double w, const double h, const double r) const
	{
		const Size baseSize = SIV3D_ENGINE(Texture)->getSize(m_handle->id());
		const double round = Min(Abs(r), (Min(w, h) * 0.5));

		return{
			*this,
			static_cast<float>(x / baseSize.x), static_cast<float>(y / baseSize.y),
			static_cast<float>((x + w) / baseSize.x), static_cast<float>((y + h) / baseSize.y),
			RoundRect{ 0, 0, w, h, round }
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	getD3D11Texture2D
	//
	////////////////////////////////////////////////////////////////

# if SIV3D_PLATFORM(WINDOWS)

	ID3D11Texture2D* Texture::getD3D11Texture2D()
	{
		if (auto p = dynamic_cast<CTexture_D3D11*>(SIV3D_ENGINE(Texture)))
		{
			return p->getTexture(m_handle->id());
		}
		else
		{
			return nullptr;
		}
	}

# endif


	////////////////////////////////////////////////////////////////
	//
	//	CreateR8_Unorm
	//
	////////////////////////////////////////////////////////////////

	Texture Texture::CreateR8_Unorm(const Size& size, const uint8 r)
	{
		const size_t width = ((size.x + 3) & ~3);
		const size_t height = size.y;
		const Grid<uint8> padded(width, height, r);

		return Texture{ size, padded.data(), padded.size_bytes(), TextureFormat::R8_Unorm, TextureDesc::NoMipmap };
	}

	Texture Texture::CreateR8_Unorm(const Size& size, const ColorF& r)
	{
		return CreateR8_Unorm(size, r.toR8_Unorm());
	}

	Texture Texture::CreateR8_Unorm(const Grid<uint8>& image)
	{
		if ((image.width()) % 4 == 0)
		{
			return Texture{ image.size(), image.data(), image.size_bytes(), TextureFormat::R8_Unorm, TextureDesc::NoMipmap };
		}
		else
		{
			const size_t width = ((image.width() + 3) & ~3);
			const size_t height = image.height();
			Grid<uint8> padded(width, height);
			
			for (size_t y = 0; y < height; ++y)
			{
				std::memcpy(padded[y], image[y], image.width());
			}

			return Texture{ image.size(), padded.data(), padded.size_bytes(), TextureFormat::R8_Unorm, TextureDesc::NoMipmap };
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	CreateR8G8_Unorm
	//
	////////////////////////////////////////////////////////////////

	Texture Texture::CreateR8G8_Unorm(const Size& size, const uint8 r, const uint8 g)
	{
		return CreateR8G8_Unorm(size, ((static_cast<uint16>(g) << 8) | r));
	}

	Texture Texture::CreateR8G8_Unorm(const Size& size, const uint16 rg)
	{
		const size_t width = ((size.x + 1) & ~1);
		const size_t height = size.y;
		const Grid<uint16> padded(width, height, rg);

		return Texture{ size, padded.data(), padded.size_bytes(), TextureFormat::R8G8_Unorm, TextureDesc::NoMipmap };
	}

	Texture Texture::CreateR8G8_Unorm(const Size& size, const ColorF& rg)
	{
		return CreateR8G8_Unorm(size, rg.toR8G8_Unorm());
	}

	Texture Texture::CreateR8G8_Unorm(const Grid<uint16>& image)
	{
		if ((image.width()) % 2 == 0)
		{
			return Texture{ image.size(), image.data(), image.size_bytes(), TextureFormat::R8G8_Unorm, TextureDesc::NoMipmap };
		}
		else
		{
			const size_t width = ((image.width() + 1) & ~1);
			const size_t height = image.height();
			Grid<uint16> padded(width, height);
			
			for (size_t y = 0; y < height; ++y)
			{
				std::memcpy(padded[y], image[y], image.width() * sizeof(uint16));
			}
			
			return Texture{ image.size(), padded.data(), padded.size_bytes(), TextureFormat::R8G8_Unorm, TextureDesc::NoMipmap };
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	CreateR16_Float
	//
	////////////////////////////////////////////////////////////////

	Texture Texture::CreateR16_Float(const Size& size, HalfFloat r)
	{
		const size_t width = ((size.x + 1) & ~1);
		const size_t height = size.y;
		const Grid<HalfFloat> padded(width, height, r);

		return Texture{ size, padded.data(), padded.size_bytes(), TextureFormat::R16_Float, TextureDesc::NoMipmap };
	}

	Texture Texture::CreateR16_Float(const Size& size, float r)
	{
		return CreateR16_Float(size, HalfFloat{ r });
	}

	Texture Texture::CreateR16_Float(const Size& size, const ColorF& r)
	{
		return CreateR16_Float(size, r.toR16_Float());
	}

	Texture Texture::CreateR16_Float(const Grid<HalfFloat>& image)
	{
		if ((image.width()) % 2 == 0)
		{
			return Texture{ image.size(), image.data(), image.size_bytes(), TextureFormat::R16_Float, TextureDesc::NoMipmap };
		}
		else
		{
			const size_t width = ((image.width() + 1) & ~1);
			const size_t height = image.height();
			Grid<HalfFloat> padded(width, height);

			for (size_t y = 0; y < height; ++y)
			{
				std::memcpy(padded[y], image[y], image.width() * sizeof(HalfFloat));
			}

			return Texture{ image.size(), padded.data(), padded.size_bytes(), TextureFormat::R16_Float, TextureDesc::NoMipmap };
		}
	}
}
