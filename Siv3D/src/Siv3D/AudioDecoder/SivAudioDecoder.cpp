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

# include <Siv3D/AudioDecoder.hpp>
# include <Siv3D/BinaryFileReader.hpp>
# include "IAudioDecoder.hpp"
# include <Siv3D/Engine/Siv3DEngine.hpp>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static String GetDecoderName(const AudioFormat audioFormat)
		{
			if (audioFormat == AudioFormat::Unspecified)
			{
				return{};
			}

			return Format(audioFormat);
		}
	}

	namespace AudioDecoder
	{
		////////////////////////////////////////////////////////////////
		//
		//	GetAudioFormat
		//
		////////////////////////////////////////////////////////////////

		AudioFormat GetAudioFormat(const FilePathView path)
		{
			return SIV3D_ENGINE(AudioDecoder)->getAudioFormat(BinaryFileReader{ path }, path, {});
		}

		AudioFormat GetAudioFormat(const IReader& reader)
		{
			return SIV3D_ENGINE(AudioDecoder)->getAudioFormat(reader, {}, {});
		}

		////////////////////////////////////////////////////////////////
		//
		//	Decode
		//
		////////////////////////////////////////////////////////////////

		Wave Decode(const FilePathView path, const AudioFormat imageFormat)
		{
			return Decode(path, GetDecoderName(imageFormat));
		}

		Wave Decode(const FilePathView path, const StringView decoderName)
		{
			std::unique_ptr<BinaryFileReader> reader = std::make_unique<BinaryFileReader>(path);

			if (not reader->isOpen())
			{
				return{};
			}

			return SIV3D_ENGINE(AudioDecoder)->decode(std::move(reader), path, decoderName);
		}

		Wave Decode(std::unique_ptr<IReader> reader, const AudioFormat imageFormat)
		{
			return Decode(std::move(reader), GetDecoderName(imageFormat));
		}

		Wave Decode(std::unique_ptr<IReader> reader, const StringView decoderName)
		{
			return SIV3D_ENGINE(AudioDecoder)->decode(std::move(reader), {}, decoderName);
		}

		////////////////////////////////////////////////////////////////
		//
		//	Add
		//
		////////////////////////////////////////////////////////////////

		bool Add(std::unique_ptr<IAudioDecoder> decoder)
		{
			return SIV3D_ENGINE(AudioDecoder)->add(std::move(decoder));
		}
		
		////////////////////////////////////////////////////////////////
		//
		//	Remove
		//
		////////////////////////////////////////////////////////////////

		void Remove(const StringView decoderName)
		{
			return SIV3D_ENGINE(AudioDecoder)->remove(decoderName);
		}
		
		////////////////////////////////////////////////////////////////
		//
		//	IsRegistered
		//
		////////////////////////////////////////////////////////////////

		bool IsRegistered(const StringView decoderName) noexcept
		{
			for (const auto& decoder : SIV3D_ENGINE(AudioDecoder)->enumDecoder())
			{
				if (decoder->name() == decoderName)
				{
					return true;
				}
			}

			return false;
		}
		
		////////////////////////////////////////////////////////////////
		//
		//	Enum
		//
		////////////////////////////////////////////////////////////////

		const Array<std::unique_ptr<IAudioDecoder>>& Enum() noexcept
		{
			return SIV3D_ENGINE(AudioDecoder)->enumDecoder();
		}
	}
}
