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

# include <Siv3D/Exif.hpp>
# include <Siv3D/Array.hpp>
# include <Siv3D/Unicode.hpp>
# include <Siv3D/MemoryMappedFileView.hpp>
# include <ThirdParty/easyexif/exif.h>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		static Optional<Exif> ReadFromMemory(const void* data, const size_t size)
		{
			easyexif::EXIFInfo info;

			if (info.parseFrom(static_cast<const uint8*>(data), static_cast<uint32>(size)) != PARSE_EXIF_SUCCESS)
			{
				return none;
			}

			return Exif{
				.imageDescription	= Unicode::FromUTF8(info.ImageDescription),
				.make				= Unicode::FromUTF8(info.Make),
				.model				= Unicode::FromUTF8(info.Model),
				.orientation		= info.Orientation,
				.bitsPerSample		= info.BitsPerSample,
				.software			= Unicode::FromUTF8(info.Software),
				.dateTime			= Unicode::FromUTF8(info.DateTime),
				.dateTimeOriginal	= Unicode::FromUTF8(info.DateTimeOriginal),
				.dateTimeDigitized	= Unicode::FromUTF8(info.DateTimeDigitized),
				.subSecTimeOriginal	= Unicode::FromUTF8(info.SubSecTimeOriginal),
				.copyright			= Unicode::FromUTF8(info.Copyright),
				.exposureTime		= info.ExposureTime,
				.FNumber			= info.FNumber,
				.exposureProgram	= info.ExposureProgram,
				.ISOSpeedRatings	= info.ISOSpeedRatings,
				.shutterSpeedValue	= info.ShutterSpeedValue,
				.exposureBiasValue	= info.ExposureBiasValue,
				.subjectDistance	= info.SubjectDistance,
				.focalLength		= info.FocalLength,
				.focalLengthIn35mm	= info.FocalLengthIn35mm,
				.flash				= static_cast<bool>(info.Flash),
				.flashReturnedLight	= info.FlashReturnedLight,
				.flashMode			= info.FlashMode,
				.meteringMode		= info.MeteringMode,
				.imageWidth			= info.ImageWidth,
				.imageHeight		= info.ImageHeight,

				.geoLocation =
				{
					.latitude		= info.GeoLocation.Latitude,
					.longitude		= info.GeoLocation.Longitude,
					.altitude		= info.GeoLocation.Altitude,
					.altitudeRef	= info.GeoLocation.AltitudeRef,
					.DOP			= info.GeoLocation.DOP,

					.latComponents =
					{
						.degrees	= info.GeoLocation.LatComponents.degrees,
						.minutes	= info.GeoLocation.LatComponents.minutes,
						.seconds	= info.GeoLocation.LatComponents.seconds,
						.direction	= static_cast<char32>(info.GeoLocation.LatComponents.direction),
					},

					.lonComponents =
					{
						.degrees	= info.GeoLocation.LonComponents.degrees,
						.minutes	= info.GeoLocation.LonComponents.minutes,
						.seconds	= info.GeoLocation.LonComponents.seconds,
						.direction	= static_cast<char32>(info.GeoLocation.LonComponents.direction),
					},
				},

				.lensInfo =
				{
					.FStopMin					= info.LensInfo.FStopMin,
					.FStopMax					= info.LensInfo.FStopMax,
					.focalLengthMin				= info.LensInfo.FocalLengthMin,
					.focalLengthMax				= info.LensInfo.FocalLengthMax,
					.focalPlaneXResolution		= info.LensInfo.FocalPlaneXResolution,
					.focalPlaneYResolution		= info.LensInfo.FocalPlaneYResolution,
					.focalPlaneResolutionUnit	= info.LensInfo.FocalPlaneResolutionUnit,
					.make						= Unicode::FromUTF8(info.LensInfo.Make),
					.model						= Unicode::FromUTF8(info.LensInfo.Model),
				}
			};
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	Read
	//
	////////////////////////////////////////////////////////////////

	Optional<Exif> Exif::Read(const FilePathView path)
	{
		MemoryMappedFileView file{ path };

		if (not file)
		{
			return none;
		}

		Optional<Exif> result;

		if (const auto mapped = file.mapAll())
		{
			result = ReadFromMemory(mapped.data, mapped.size);

			file.unmap();
		}

		return result;
	}

	Optional<Exif> Exif::Read(const IReader& reader)
	{
		Array<uint8> data(static_cast<size_t>(reader.size()));

		reader.lookahead(data.data(), data.size());

		return ReadFromMemory(data.data(), data.size());
	}
}
