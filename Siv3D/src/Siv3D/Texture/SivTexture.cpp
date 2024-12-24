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
}
