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

# include "CImageDecoder.hpp"
# include <Siv3D/IReader.hpp>
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/EngineLog.hpp>
# include <Siv3D/ImageFormat/BMPDecoder.hpp>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static Array<std::unique_ptr<IImageDecoder>>::const_iterator FindDecoderByName(const Array<std::unique_ptr<IImageDecoder>>& decoders, const StringView name) noexcept
		{
			return std::find_if(decoders.begin(), decoders.end(), [name](const std::unique_ptr<IImageDecoder>& decoder)
				{
					return (decoder->name() == name);
				});
		}

		[[nodiscard]]
		static Array<std::unique_ptr<IImageDecoder>>::const_iterator FindDecoder_impl(const Array<std::unique_ptr<IImageDecoder>>& decoders, const ImageFormat imageFormat) noexcept
		{
			if (imageFormat == ImageFormat::Unknown)
			{
				return decoders.end();
			}

			return std::find_if(decoders.begin(), decoders.end(), [imageFormat](const std::unique_ptr<IImageDecoder>& decoder)
				{
					return (decoder->imageFormat() == imageFormat);
				});
		}

		[[nodiscard]]
		static Array<std::unique_ptr<IImageDecoder>>::const_iterator FindDecoder_impl(const Array<std::unique_ptr<IImageDecoder>>& decoders, const IReader& reader, const FilePathView pathHint)
		{
			if (not reader)
			{
				return decoders.end();
			}

			if (not reader.supportsLookahead())
			{
				return decoders.end();
			}

			uint8 header[16]{};

			if (not reader.lookahead(header))
			{
				return decoders.end();
			}

			for (auto it = decoders.begin(); it != decoders.end(); ++it)
			{
				if ((*it)->isHeader(header))
				{
					return it;
				}
			}

			if (pathHint)
			{
				const String extension = FileSystem::Extension(pathHint);

				for (auto it = decoders.begin(); it != decoders.end(); ++it)
				{
					if ((*it)->possibleExtensions().contains(extension))
					{
						return it;
					}
				}
			}

			return decoders.end();
		}

		[[nodiscard]]
		static Array<std::unique_ptr<IImageDecoder>>::const_iterator FindDecoder(const Array<std::unique_ptr<IImageDecoder>>& decoders, const ImageFormat imageFormat, const IReader& reader, const FilePathView pathHint)
		{
			if (const auto it = FindDecoder_impl(decoders, imageFormat); it != decoders.end())
			{
				return it;
			}

			return FindDecoder_impl(decoders, reader, pathHint);
		}
	}

	CImageDecoder::~CImageDecoder()
	{
		LOG_SCOPED_DEBUG("CImageDecoder::~CImageDecoder()");
	}

	void CImageDecoder::init()
	{
		LOG_SCOPED_DEBUG("CImageDecoder::init()");

		m_decoders.push_back(std::make_unique<BMPDecoder>());
	}

	bool CImageDecoder::add(std::unique_ptr<IImageDecoder>&& decoder)
	{
		const StringView name = decoder->name();

		// 同名のデコーダがすでに登録されているか
		const bool exists = (FindDecoderByName(m_decoders, name) != m_decoders.end());

		if (exists) // すでに登録されている場合は失敗
		{
			LOG_FAIL(fmt::format("❌ ImageDecoder: {} is already registered", name));
			
			return false;
		}
		else // 未登録の場合は登録
		{
			m_decoders.push_back(std::move(decoder));

			LOG_INFO(fmt::format("🆕 ImageDecoder: {} has been registered", name));

			return true;
		}
	}

	void CImageDecoder::remove(const StringView name)
	{
		const auto it = FindDecoderByName(m_decoders, name);

		if (it != m_decoders.end())
		{
			m_decoders.erase(it);

			LOG_INFO(fmt::format("🗑️ ImageDecoder: {} has been removed", name));
		}
		else
		{
			LOG_DEBUG(fmt::format("ImageDecoder: {} not found", name));
		}
	}

	const Array<std::unique_ptr<IImageDecoder>>& CImageDecoder::enumDecoder() const noexcept
	{
		return m_decoders;
	}

	Optional<ImageInfo> CImageDecoder::getImageInfo(IReader& reader, const FilePathView pathHint, const ImageFormat imageFormat)
	{
		LOG_SCOPED_DEBUG("CImageDecoder::getImageInfo()");

		const auto it = FindDecoder(m_decoders, imageFormat, reader, pathHint);

		if (it == m_decoders.end())
		{
			return{};
		}

		LOG_TRACE(fmt::format("Image decoder name: {}", (*it)->name()));

		return (*it)->getImageInfo(reader, pathHint);
	}

	Image CImageDecoder::decode(IReader& reader, const FilePathView pathHint, const ImageFormat imageFormat)
	{
		LOG_SCOPED_DEBUG("CImageDecoder::decode()");

		const auto it = FindDecoder(m_decoders, imageFormat, reader, pathHint);

		if (it == m_decoders.end())
		{
			return{};
		}

		LOG_TRACE(fmt::format("Image decoder name: {}", (*it)->name()));

		return (*it)->decode(reader, pathHint);
	}

	Grid<uint16> CImageDecoder::decodeGray16(IReader& reader, const FilePathView pathHint, const ImageFormat imageFormat)
	{
		LOG_SCOPED_DEBUG("CImageDecoder::decodeGray16()");

		const auto it = FindDecoder(m_decoders, imageFormat, reader, pathHint);

		if (it == m_decoders.end())
		{
			return{};
		}

		LOG_TRACE(fmt::format("Image decoder name: {}", (*it)->name()));

		return (*it)->decodeGray16(reader, pathHint);
	}
}
