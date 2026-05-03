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

# include <Siv3D/AudioEncoder.hpp>
# include <Siv3D/Wave.hpp>
# include "IAudioEncoder.hpp"
# include <Siv3D/Engine/Siv3DEngine.hpp>
# include <Siv3D/FileSystem.hpp>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static String GetEncoderName(const AudioFormat imageFormat)
		{
			if (imageFormat == AudioFormat::Unspecified)
			{
				return{};
			}

			return Format(imageFormat);
		}
	}

	namespace AudioEncoder
	{		
		////////////////////////////////////////////////////////////////
		//
		//	Save
		//
		////////////////////////////////////////////////////////////////

		bool Save(const Wave& wave, AudioFormat format, const FilePathView path)
		{
			if (not wave)
			{
				return false;
			}

			String encoderName;

			if (format == AudioFormat::Unspecified)
			{
				encoderName = SIV3D_ENGINE(AudioEncoder)->getEncoderNameFromExtension(FileSystem::Extension(path));
			}
			else
			{
				encoderName = Format(format);
			}

			return Save(wave, encoderName, path);
		}

		bool Save(const Wave& wave, const StringView encoderName, const FilePathView path)
		{
			return SIV3D_ENGINE(AudioEncoder)->save(wave, encoderName, path);
		}
		
		////////////////////////////////////////////////////////////////
		//
		//	Encode
		//
		////////////////////////////////////////////////////////////////

		bool Encode(const Wave& wave, const AudioFormat format, IWriter& writer)
		{
			return Encode(wave, GetEncoderName(format), writer);
		}

		bool Encode(const Wave& wave, const StringView encoderName, IWriter& writer)
		{
			return SIV3D_ENGINE(AudioEncoder)->encode(wave, encoderName, writer);
		}

		Blob Encode(const Wave& wave, const AudioFormat format)
		{
			return Encode(wave, GetEncoderName(format));
		}

		Blob Encode(const Wave& wave, const StringView encoderName)
		{
			return SIV3D_ENGINE(AudioEncoder)->encode(wave, encoderName);
		}
		
		////////////////////////////////////////////////////////////////
		//
		//	Add
		//
		////////////////////////////////////////////////////////////////

		bool Add(std::unique_ptr<IAudioEncoder> Encoder)
		{
			return SIV3D_ENGINE(AudioEncoder)->add(std::move(Encoder));
		}
		
		////////////////////////////////////////////////////////////////
		//
		//	Remove
		//
		////////////////////////////////////////////////////////////////

		void Remove(const StringView encoderName)
		{
			return SIV3D_ENGINE(AudioEncoder)->remove(encoderName);
		}
		
		////////////////////////////////////////////////////////////////
		//
		//	IsRegistered
		//
		////////////////////////////////////////////////////////////////

		bool IsRegistered(const StringView encoderName) noexcept
		{
			for (const auto& encoder : SIV3D_ENGINE(AudioEncoder)->enumEncoder())
			{
				if (encoder->name() == encoderName)
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

		const Array<std::unique_ptr<IAudioEncoder>>& Enum() noexcept
		{
			return SIV3D_ENGINE(AudioEncoder)->enumEncoder();
		}
	}
}
