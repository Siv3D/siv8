//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2026 Ryo Suzuki
//	Copyright (c) 2016-2026 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include "CAudioEncoder.hpp"
# include <Siv3D/IWriter.hpp>
# include <Siv3D/EngineLog.hpp>
# include <Siv3D/AudioFormat/WAVEncoder.hpp>
# include <Siv3D/AudioFormat/AACEncoder.hpp>
# include <Siv3D/AudioFormat/MP3Encoder.hpp>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static Array<std::unique_ptr<IAudioEncoder>>::const_iterator FindEncoderByName(const Array<std::unique_ptr<IAudioEncoder>>& encoders, const StringView encoderName) noexcept
		{
			return std::find_if(encoders.begin(), encoders.end(), [encoderName](const std::unique_ptr<IAudioEncoder>& encoder)
				{
					return (encoder->name() == encoderName);
				});
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	CAudioEncoder::~CAudioEncoder()
	{
		LOG_SCOPED_DEBUG("CAudioEncoder::~CAudioEncoder()");
	}

	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

	void CAudioEncoder::init()
	{
		LOG_SCOPED_DEBUG("CAudioEncoder::init()");

		m_encoders.push_back(std::make_unique<WAVEncoder>());

	# if SIV3D_PLATFORM(WINDOWS)
		m_encoders.push_back(std::make_unique<AACEncoder>());
		m_encoders.push_back(std::make_unique<MP3Encoder>());
	# endif
	}

	////////////////////////////////////////////////////////////////
	//
	//	add
	//
	////////////////////////////////////////////////////////////////

	bool CAudioEncoder::add(std::unique_ptr<IAudioEncoder> encoder)
	{
		const StringView name = encoder->name();

		// 同名のエンコーダがすでに登録されているか
		const bool exists = (FindEncoderByName(m_encoders, name) != m_encoders.end());

		if (exists) // すでに登録されている場合は失敗
		{
			LOG_FAIL(fmt::format("❌ AudioEncoder: {} is already registered", name));

			return false;
		}
		else // 未登録の場合は登録
		{
			m_encoders.push_back(std::move(encoder));

			LOG_INFO(fmt::format("🆕 AudioEncoder: {} has been registered", name));

			return true;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	remove
	//
	////////////////////////////////////////////////////////////////

	void CAudioEncoder::remove(const StringView encoderName)
	{
		const auto it = FindEncoderByName(m_encoders, encoderName);

		if (it != m_encoders.end())
		{
			m_encoders.erase(it);

			LOG_INFO(fmt::format("🗑️ AudioEncoder: {} has been removed", encoderName));
		}
		else
		{
			LOG_DEBUG(fmt::format("AudioEncoder: {} not found", encoderName));
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	enumEncoder
	//
	////////////////////////////////////////////////////////////////

	const Array<std::unique_ptr<IAudioEncoder>>& CAudioEncoder::enumEncoder() const noexcept
	{
		return m_encoders;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getEncoderNameFromExtension
	//
	////////////////////////////////////////////////////////////////

	String CAudioEncoder::getEncoderNameFromExtension(const StringView extension) const
	{
		if (not extension)
		{
			return{};
		}

		for (const auto& encoder : m_encoders)
		{
			if (std::ranges::contains(encoder->possibleExtensions(), extension))
			{
				return String{ encoder->name() };
			}
		}

		return{};
	}

	////////////////////////////////////////////////////////////////
	//
	//	save
	//
	////////////////////////////////////////////////////////////////

	bool CAudioEncoder::save(const Wave& wave, const StringView encoderName, const FilePathView path) const
	{
		LOG_SCOPED_DEBUG(fmt::format("CAudioEncoder::save({}, {})", encoderName, path));

		const auto it = FindEncoderByName(m_encoders, encoderName);

		if (it == m_encoders.end())
		{
			return false;
		}

		return (*it)->save(wave, path);
	}

	////////////////////////////////////////////////////////////////
	//
	//	encode
	//
	////////////////////////////////////////////////////////////////

	bool CAudioEncoder::encode(const Wave& wave, const StringView encoderName, IWriter& writer) const
	{
		LOG_SCOPED_DEBUG(fmt::format("CAudioEncoder::encode({})", encoderName));

		const auto it = FindEncoderByName(m_encoders, encoderName);

		if (it == m_encoders.end())
		{
			return false;
		}

		return (*it)->encode(wave, writer);
	}

	Blob CAudioEncoder::encode(const Wave& wave, const StringView encoderName) const
	{
		LOG_SCOPED_DEBUG(fmt::format("CAudioEncoder::encode({})", encoderName));

		const auto it = FindEncoderByName(m_encoders, encoderName);

		if (it == m_encoders.end())
		{
			return{};
		}

		return (*it)->encode(wave);
	}
}
