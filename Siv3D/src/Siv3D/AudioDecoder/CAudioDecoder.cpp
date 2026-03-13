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

# include "CAudioDecoder.hpp"
# include <Siv3D/IReader.hpp>
# include <Siv3D/FileSystem.hpp>
# include <Siv3D/EngineLog.hpp>
//# include <Siv3D/AudioFormat/WAVDecoder.hpp>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static Array<std::unique_ptr<IAudioDecoder>>::const_iterator FindDecoderByName(const Array<std::unique_ptr<IAudioDecoder>>& decoders, const StringView decoderName) noexcept
		{
			return std::find_if(decoders.begin(), decoders.end(), [decoderName](const std::unique_ptr<IAudioDecoder>& decoder)
				{
					return (decoder->name() == decoderName);
				});
		}

		[[nodiscard]]
		static Array<std::unique_ptr<IAudioDecoder>>::const_iterator FindDecoderWithReader(const Array<std::unique_ptr<IAudioDecoder>>& decoders, const IReader& reader, const FilePathView pathHint)
		{
			if (not reader)
			{
				return decoders.end();
			}

			if (not reader.supportsLookahead())
			{
				return decoders.end();
			}

			uint8 header[IAudioDecoder::RequiredHeaderBytes]{};

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
					if (std::ranges::contains((*it)->possibleExtensions(), extension))
					{
						return it;
					}
				}
			}

			return decoders.end();
		}

		[[nodiscard]]
		static Array<std::unique_ptr<IAudioDecoder>>::const_iterator FindDecoder(const Array<std::unique_ptr<IAudioDecoder>>& decoders, const StringView decoderName, const IReader& reader, const FilePathView pathHint)
		{
			if (decoderName)
			{
				if (const auto it = FindDecoderByName(decoders, decoderName); it != decoders.end())
				{
					return it;
				}
			}

			return FindDecoderWithReader(decoders, reader, pathHint);
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	(destructor)
	//
	////////////////////////////////////////////////////////////////

	CAudioDecoder::~CAudioDecoder()
	{
		LOG_SCOPED_DEBUG("CAudioDecoder::~CAudioDecoder()");
	}

	////////////////////////////////////////////////////////////////
	//
	//	init
	//
	////////////////////////////////////////////////////////////////

	void CAudioDecoder::init()
	{
		LOG_SCOPED_DEBUG("CAudioDecoder::init()");

		//m_decoders.push_back(std::make_unique<WAVDecoder>());
	}

	////////////////////////////////////////////////////////////////
	//
	//	add
	//
	////////////////////////////////////////////////////////////////

	bool CAudioDecoder::add(std::unique_ptr<IAudioDecoder> decoder)
	{
		const StringView name = decoder->name();

		// 同名のデコーダがすでに登録されているか
		const bool exists = (FindDecoderByName(m_decoders, name) != m_decoders.end());

		if (exists) // すでに登録されている場合は失敗
		{
			LOG_FAIL(fmt::format("❌ AudioDecoder: {} is already registered", name));
			
			return false;
		}
		else // 未登録の場合は登録
		{
			m_decoders.push_back(std::move(decoder));

			LOG_INFO(fmt::format("🆕 AudioDecoder: {} has been registered", name));

			return true;
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	remove
	//
	////////////////////////////////////////////////////////////////

	void CAudioDecoder::remove(const StringView decoderName)
	{
		const auto it = FindDecoderByName(m_decoders, decoderName);

		if (it != m_decoders.end())
		{
			m_decoders.erase(it);

			LOG_INFO(fmt::format("🗑️ AudioDecoder: {} has been removed", decoderName));
		}
		else
		{
			LOG_DEBUG(fmt::format("AudioDecoder: {} not found", decoderName));
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	enumDecoder
	//
	////////////////////////////////////////////////////////////////

	const Array<std::unique_ptr<IAudioDecoder>>& CAudioDecoder::enumDecoder() const noexcept
	{
		return m_decoders;
	}

	////////////////////////////////////////////////////////////////
	//
	//	getAudioFormat
	//
	////////////////////////////////////////////////////////////////

	AudioFormat CAudioDecoder::getAudioFormat(const IReader& reader, const FilePathView pathHint, const StringView decoderName)
	{
		LOG_SCOPED_DEBUG("CAudioDecoder::getAudioFormat()");

		const auto it = FindDecoder(m_decoders, decoderName, reader, pathHint);

		if (it == m_decoders.end())
		{
			return{};
		}

		LOG_TRACE(fmt::format("Audio decoder name: {}", (*it)->name()));

		return (*it)->audioFormat();
	}

	////////////////////////////////////////////////////////////////
	//
	//	decode
	//
	////////////////////////////////////////////////////////////////

	Wave CAudioDecoder::decode(std::unique_ptr<IReader> reader, const FilePathView pathHint, const StringView decoderName)
	{
		LOG_SCOPED_DEBUG("CAudioDecoder::decode()");

		const auto it = FindDecoder(m_decoders, decoderName, *reader, pathHint);

		if (it == m_decoders.end())
		{
			return{};
		}

		LOG_TRACE(fmt::format("Audio decoder name: {}", (*it)->name()));

		return (*it)->decode(std::move(reader), pathHint);
	}
}
