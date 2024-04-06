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

# include "CImageEncoder.hpp"
# include <Siv3D/IWriter.hpp>
# include <Siv3D/EngineLog.hpp>
# include <Siv3D/ImageFormat/BMPEncoder.hpp>
# include <Siv3D/ImageFormat/PNGEncoder.hpp>
# include <Siv3D/ImageFormat/JPEGEncoder.hpp>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static Array<std::unique_ptr<IImageEncoder>>::const_iterator FindEncoderByName(const Array<std::unique_ptr<IImageEncoder>>& encoders, const StringView encoderName) noexcept
		{
			return std::find_if(encoders.begin(), encoders.end(), [encoderName](const std::unique_ptr<IImageEncoder>& encoder)
				{
					return (encoder->name() == encoderName);
				});
		}
	}

	CImageEncoder::~CImageEncoder()
	{
		LOG_SCOPED_DEBUG("CImageEncoder::~CImageEncoder()");
	}

	void CImageEncoder::init()
	{
		LOG_SCOPED_DEBUG("CImageEncoder::init()");

		m_encoders.push_back(std::make_unique<BMPEncoder>());
		m_encoders.push_back(std::make_unique<PNGEncoder>());
		m_encoders.push_back(std::make_unique<JPEGEncoder>());
	}

	bool CImageEncoder::add(std::unique_ptr<IImageEncoder>&& encoder)
	{
		const StringView name = encoder->name();

		// 同名のエンコーダがすでに登録されているか
		const bool exists = (FindEncoderByName(m_encoders, name) != m_encoders.end());

		if (exists) // すでに登録されている場合は失敗
		{
			LOG_FAIL(fmt::format("❌ ImageEncoder: {} is already registered", name));

			return false;
		}
		else // 未登録の場合は登録
		{
			m_encoders.push_back(std::move(encoder));

			LOG_INFO(fmt::format("🆕 ImageEncoder: {} has been registered", name));

			return true;
		}
	}

	void CImageEncoder::remove(const StringView encoderName)
	{
		const auto it = FindEncoderByName(m_encoders, encoderName);

		if (it != m_encoders.end())
		{
			m_encoders.erase(it);

			LOG_INFO(fmt::format("🗑️ ImageEncoder: {} has been removed", encoderName));
		}
		else
		{
			LOG_DEBUG(fmt::format("ImageEncoder: {} not found", encoderName));
		}
	}

	const Array<std::unique_ptr<IImageEncoder>>& CImageEncoder::enumEncoder() const noexcept
	{
		return m_encoders;
	}

	String CImageEncoder::getEncoderNameFromExtension(const StringView extension) const
	{
		if (not extension)
		{
			return{};
		}

		const String ext{ extension };

		for (const auto& encoder : m_encoders)
		{
			if (encoder->possibleExtensions().contains(ext))
			{
				return String{ encoder->name() };
			}
		}

		return{};
	}

	bool CImageEncoder::save(const Image& image, const StringView encoderName, const FilePathView path) const
	{
		LOG_SCOPED_DEBUG(fmt::format("CImageEncoder::save({}, {})", encoderName, path));

		const auto it = FindEncoderByName(m_encoders, encoderName);

		if (it == m_encoders.end())
		{
			return{};
		}

		return (*it)->save(image, path);
	}

	bool CImageEncoder::encode(const Image& image, const StringView encoderName, IWriter& writer) const
	{
		LOG_SCOPED_DEBUG(fmt::format("CImageEncoder::encode({})", encoderName));

		const auto it = FindEncoderByName(m_encoders, encoderName);

		if (it == m_encoders.end())
		{
			return false;
		}

		return (*it)->encode(image, writer);
	}

	Blob CImageEncoder::encode(const Image& image, const StringView encoderName) const
	{
		LOG_SCOPED_DEBUG(fmt::format("CImageEncoder::encode({})", encoderName));

		const auto it = FindEncoderByName(m_encoders, encoderName);

		if (it == m_encoders.end())
		{
			return{};
		}

		return (*it)->encode(image);
	}
}

