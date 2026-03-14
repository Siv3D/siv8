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

# include <Siv3D/Dialog.hpp>

namespace s3d
{
	namespace
	{
		static const Array<FileDialogFilter> DefaultOpenImageFileFilters =
		{
			FileDialogFilter::AllImageFiles(),
			FileDialogFilter::PNG(),
			FileDialogFilter::JPEG(),
			FileDialogFilter::BMP(),
			FileDialogFilter::GIF(),
			FileDialogFilter::TGA(),
			FileDialogFilter::SVG(),
			FileDialogFilter::WebP(),
			FileDialogFilter::TIFF(),
			FileDialogFilter::PPM(),
		};

		static const Array<FileDialogFilter> DefaultOpenAudioFileFilters =
		{
			FileDialogFilter::AllAudioFiles(),
			FileDialogFilter::WAV(),
			FileDialogFilter::MP3(),
			FileDialogFilter::AAC(),
			FileDialogFilter::OggVorbis(),
			FileDialogFilter::Opus(),
			FileDialogFilter::MIDI(),
		# if SIV3D_PLATFORM(WINDOWS)
			FileDialogFilter::WMA(),
			FileDialogFilter::FLAC(),
		# elif SIV3D_PLATFORM(MACOS)
			FileDialogFilter::AIFF(),
			FileDialogFilter::FLAC(),
		# elif SIV3D_PLATFORM(LINUX)

		# elif SIV3D_PLATFORM(WEB)

		# endif
		};

		static const Array<FileDialogFilter> DefaultSaveImageFileFilters =
		{
			FileDialogFilter::PNG(),
			FileDialogFilter::JPEG(),
			FileDialogFilter::BMP(),
			FileDialogFilter::GIF(),
			FileDialogFilter::TGA(),
			FileDialogFilter::WebP(),
			FileDialogFilter::TIFF(),
			FileDialogFilter::PPM(),
		};

		static const Array<FileDialogFilter> DefaultSaveAudioFileFilters =
		{
			FileDialogFilter::WAV(),
			//FileFilter::MP3(),
			FileDialogFilter::OggVorbis(),
			//FileFilter::Opus(),
		};
	}

	namespace Dialog
	{
		////////////////////////////////////////////////////////////////
		//
		//	OpenImage
		//
		////////////////////////////////////////////////////////////////

		Image OpenImage(const OpenFileDialogOptions& options)
		{
			return OpenImage(PremultiplyAlpha::Yes, options);
		}

		Image OpenImage(const PremultiplyAlpha premultiplyAlpha, const OpenFileDialogOptions& options)
		{
			OpenFileDialogOptions optionsOverride = options;
			optionsOverride.filters = DefaultOpenImageFileFilters;

			if (const auto path = OpenFile(optionsOverride))
			{
				return Image{ *path, premultiplyAlpha };
			}

			return{};
		}

		////////////////////////////////////////////////////////////////
		//
		//	OpenTexture
		//
		////////////////////////////////////////////////////////////////

		Texture OpenTexture(const OpenFileDialogOptions& options)
		{
			return OpenTexture(TextureDesc::Default2D, options);
		}

		Texture OpenTexture(const TextureDesc textureDesc, const OpenFileDialogOptions& options)
		{
			OpenFileDialogOptions optionsOverride = options;
			optionsOverride.filters = DefaultOpenImageFileFilters;

			if (const auto path = OpenFile(optionsOverride))
			{
				return Texture{ *path, textureDesc };
			}

			return{};
		}

		////////////////////////////////////////////////////////////////
		//
		//	OpenWave
		//
		////////////////////////////////////////////////////////////////

		Wave OpenWave(const OpenFileDialogOptions& options)
		{
			OpenFileDialogOptions optionsOverride = options;
			optionsOverride.filters = DefaultOpenAudioFileFilters;
			
			if (const auto path = OpenFile(optionsOverride))
			{
				return Wave{ *path };
			}
			
			return{};
		}

		////////////////////////////////////////////////////////////////
		//
		//	OpenAudio
		//
		////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////
		//
		//	SaveImage
		//
		////////////////////////////////////////////////////////////////

		Optional<FilePath> SaveImage(const SaveFileDialogOptions& options)
		{
			SaveFileDialogOptions optionsOverride = options;
			optionsOverride.filters = DefaultSaveImageFileFilters;
			return SaveFile(optionsOverride);
		}

		////////////////////////////////////////////////////////////////
		//
		//	SaveWave
		//
		////////////////////////////////////////////////////////////////

		Optional<FilePath> SaveWave(const SaveFileDialogOptions& options)
		{
			SaveFileDialogOptions optionsOverride = options;
			optionsOverride.filters = DefaultSaveAudioFileFilters;
			return SaveFile(optionsOverride);
		}
	}
}
