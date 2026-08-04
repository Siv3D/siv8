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

# include "Siv3DTest.hpp"
# include <array>

namespace
{
	template <class Enum, size_t N>
	void CheckEnumFormatter(const std::array<std::pair<Enum, StringView>, N>& cases)
	{
		for (const auto& [value, expected] : cases)
		{
			CHECK_EQ(Format(value), expected);
		}
	}
}

TEST_CASE("EnumFormatter")
{
	SUBCASE("TexturePixelFormat")
	{
		static constexpr std::array Cases =
		{
			std::pair{ TexturePixelFormat::Unknown, StringView{ U"Unknown" } },
			std::pair{ TexturePixelFormat::R8_Unorm, StringView{ U"R8_Unorm" } },
			std::pair{ TexturePixelFormat::R8G8_Unorm, StringView{ U"R8G8_Unorm" } },
			std::pair{ TexturePixelFormat::R16_Float, StringView{ U"R16_Float" } },
			std::pair{ TexturePixelFormat::R8G8B8A8_Unorm, StringView{ U"R8G8B8A8_Unorm" } },
			std::pair{ TexturePixelFormat::R8G8B8A8_Unorm_SRGB, StringView{ U"R8G8B8A8_Unorm_SRGB" } },
			std::pair{ TexturePixelFormat::R16G16_Unorm, StringView{ U"R16G16_Unorm" } },
			std::pair{ TexturePixelFormat::R16G16_Float, StringView{ U"R16G16_Float" } },
			std::pair{ TexturePixelFormat::R32_Float, StringView{ U"R32_Float" } },
			std::pair{ TexturePixelFormat::R10G10B10A2_Unorm, StringView{ U"R10G10B10A2_Unorm" } },
			std::pair{ TexturePixelFormat::R11G11B10_UFloat, StringView{ U"R11G11B10_UFloat" } },
			std::pair{ TexturePixelFormat::R16G16B16A16_Float, StringView{ U"R16G16B16A16_Float" } },
			std::pair{ TexturePixelFormat::R32G32_Float, StringView{ U"R32G32_Float" } },
			std::pair{ TexturePixelFormat::R32G32B32A32_Float, StringView{ U"R32G32B32A32_Float" } },
			std::pair{ TexturePixelFormat::BC1_RGBA_Unorm, StringView{ U"BC1_RGBA_Unorm" } },
			std::pair{ TexturePixelFormat::BC1_RGBA_Unorm_SRGB, StringView{ U"BC1_RGBA_Unorm_SRGB" } },
			std::pair{ TexturePixelFormat::BC3_RGBA_Unorm, StringView{ U"BC3_RGBA_Unorm" } },
			std::pair{ TexturePixelFormat::BC3_RGBA_Unorm_SRGB, StringView{ U"BC3_RGBA_Unorm_SRGB" } },
			std::pair{ TexturePixelFormat::BC4_R_Unorm, StringView{ U"BC4_R_Unorm" } },
			std::pair{ TexturePixelFormat::BC5_RG_Unorm, StringView{ U"BC5_RG_Unorm" } },
			std::pair{ TexturePixelFormat::BC7_RGBA_Unorm, StringView{ U"BC7_RGBA_Unorm" } },
			std::pair{ TexturePixelFormat::BC7_RGBA_Unorm_SRGB, StringView{ U"BC7_RGBA_Unorm_SRGB" } },
		};

		CheckEnumFormatter(Cases);
	}

	SUBCASE("AnimatedImageDecodeError")
	{
		static constexpr std::array Cases =
		{
			std::pair{ AnimatedImageDecodeError::None, StringView{ U"None" } },
			std::pair{ AnimatedImageDecodeError::InvalidFormat, StringView{ U"InvalidFormat" } },
			std::pair{ AnimatedImageDecodeError::NotAnimated, StringView{ U"NotAnimated" } },
			std::pair{ AnimatedImageDecodeError::InvalidData, StringView{ U"InvalidData" } },
			std::pair{ AnimatedImageDecodeError::TruncatedInput, StringView{ U"TruncatedInput" } },
			std::pair{ AnimatedImageDecodeError::ReadError, StringView{ U"ReadError" } },
			std::pair{ AnimatedImageDecodeError::UnsupportedFeature, StringView{ U"UnsupportedFeature" } },
			std::pair{ AnimatedImageDecodeError::TooManyFrames, StringView{ U"TooManyFrames" } },
			std::pair{ AnimatedImageDecodeError::DecodedBytesLimitExceeded, StringView{ U"DecodedBytesLimitExceeded" } },
			std::pair{ AnimatedImageDecodeError::WorkingMemoryLimitExceeded, StringView{ U"WorkingMemoryLimitExceeded" } },
			std::pair{ AnimatedImageDecodeError::OutOfMemory, StringView{ U"OutOfMemory" } },
		};

		CheckEnumFormatter(Cases);
	}

	SUBCASE("INIParseErrorCode")
	{
		static constexpr std::array Cases =
		{
			std::pair{ INIParseErrorCode::InvalidReader, StringView{ U"InvalidReader" } },
			std::pair{ INIParseErrorCode::InvalidEncoding, StringView{ U"InvalidEncoding" } },
			std::pair{ INIParseErrorCode::BareCR, StringView{ U"BareCR" } },
			std::pair{ INIParseErrorCode::MissingClosingBracket, StringView{ U"MissingClosingBracket" } },
			std::pair{ INIParseErrorCode::EmptySectionName, StringView{ U"EmptySectionName" } },
			std::pair{ INIParseErrorCode::UnexpectedCharacterAfterSection, StringView{ U"UnexpectedCharacterAfterSection" } },
			std::pair{ INIParseErrorCode::MissingAssignment, StringView{ U"MissingAssignment" } },
			std::pair{ INIParseErrorCode::EmptyKey, StringView{ U"EmptyKey" } },
			std::pair{ INIParseErrorCode::DuplicateSection, StringView{ U"DuplicateSection" } },
			std::pair{ INIParseErrorCode::DuplicateKey, StringView{ U"DuplicateKey" } },
		};

		CheckEnumFormatter(Cases);
	}

	SUBCASE("CSVParseErrorCode")
	{
		static constexpr std::array Cases =
		{
			std::pair{ CSVParseErrorCode::InvalidReader, StringView{ U"InvalidReader" } },
			std::pair{ CSVParseErrorCode::InvalidEncoding, StringView{ U"InvalidEncoding" } },
			std::pair{ CSVParseErrorCode::UnexpectedQuoteInUnquotedField, StringView{ U"UnexpectedQuoteInUnquotedField" } },
			std::pair{ CSVParseErrorCode::UnexpectedCharacterAfterClosingQuote, StringView{ U"UnexpectedCharacterAfterClosingQuote" } },
			std::pair{ CSVParseErrorCode::UnterminatedQuotedField, StringView{ U"UnterminatedQuotedField" } },
			std::pair{ CSVParseErrorCode::BareCR, StringView{ U"BareCR" } },
			std::pair{ CSVParseErrorCode::BareLF, StringView{ U"BareLF" } },
			std::pair{ CSVParseErrorCode::InconsistentColumns, StringView{ U"InconsistentColumns" } },
		};

		CheckEnumFormatter(Cases);
	}

	SUBCASE("AnimatedImageReadStatus")
	{
		static constexpr std::array Cases =
		{
			std::pair{ AnimatedImageReadStatus::Frame, StringView{ U"Frame" } },
			std::pair{ AnimatedImageReadStatus::EndOfStream, StringView{ U"EndOfStream" } },
			std::pair{ AnimatedImageReadStatus::Error, StringView{ U"Error" } },
		};

		CheckEnumFormatter(Cases);
	}

	SUBCASE("MessageBoxResult")
	{
		static constexpr std::array Cases =
		{
			std::pair{ MessageBoxResult::Ok, StringView{ U"Ok" } },
			std::pair{ MessageBoxResult::Cancel, StringView{ U"Cancel" } },
			std::pair{ MessageBoxResult::Yes, StringView{ U"Yes" } },
			std::pair{ MessageBoxResult::No, StringView{ U"No" } },
		};

		CheckEnumFormatter(Cases);
	}

	SUBCASE("ChildProcessPipe")
	{
		static constexpr std::array Cases =
		{
			std::pair{ ChildProcessPipe::None, StringView{ U"None" } },
			std::pair{ ChildProcessPipe::StdIn, StringView{ U"StdIn" } },
			std::pair{ ChildProcessPipe::StdOut, StringView{ U"StdOut" } },
			std::pair{ ChildProcessPipe::StdInOut, StringView{ U"StdInOut" } },
		};

		CheckEnumFormatter(Cases);
	}

	SUBCASE("FileChangeAction")
	{
		static constexpr std::array Cases =
		{
			std::pair{ FileChangeAction::Unknown, StringView{ U"Unknown" } },
			std::pair{ FileChangeAction::Added, StringView{ U"Added" } },
			std::pair{ FileChangeAction::Removed, StringView{ U"Removed" } },
			std::pair{ FileChangeAction::Modified, StringView{ U"Modified" } },
		};

		CheckEnumFormatter(Cases);
	}

	SUBCASE("PolygonFailureType")
	{
		static constexpr std::array Cases =
		{
			std::pair{ PolygonFailureType::Ok, StringView{ U"Ok" } },
			std::pair{ PolygonFailureType::FewPoints, StringView{ U"FewPoints" } },
			std::pair{ PolygonFailureType::WrongTopologicalDimension, StringView{ U"WrongTopologicalDimension" } },
			std::pair{ PolygonFailureType::Spikes, StringView{ U"Spikes" } },
			std::pair{ PolygonFailureType::DuplicatePoints, StringView{ U"DuplicatePoints" } },
			std::pair{ PolygonFailureType::NotClosed, StringView{ U"NotClosed" } },
			std::pair{ PolygonFailureType::SelfIntersections, StringView{ U"SelfIntersections" } },
			std::pair{ PolygonFailureType::WrongOrientation, StringView{ U"WrongOrientation" } },
			std::pair{ PolygonFailureType::InteriorRingsOutside, StringView{ U"InteriorRingsOutside" } },
			std::pair{ PolygonFailureType::NestedInteriorRings, StringView{ U"NestedInteriorRings" } },
			std::pair{ PolygonFailureType::DisconnectedInterior, StringView{ U"DisconnectedInterior" } },
			std::pair{ PolygonFailureType::IntersectingInteriors, StringView{ U"IntersectingInteriors" } },
			std::pair{ PolygonFailureType::WrongCornerOrder, StringView{ U"WrongCornerOrder" } },
			std::pair{ PolygonFailureType::InvalidCoordinate, StringView{ U"InvalidCoordinate" } },
			std::pair{ PolygonFailureType::Unknown, StringView{ U"Unknown" } },
		};

		CheckEnumFormatter(Cases);
	}

	SUBCASE("NotificationActionCategory")
	{
		static constexpr std::array Cases =
		{
			std::pair{ NotificationActionCategory::Default, StringView{ U"Default" } },
			std::pair{ NotificationActionCategory::Open, StringView{ U"Open" } },
			std::pair{ NotificationActionCategory::OpenDismiss, StringView{ U"OpenDismiss" } },
			std::pair{ NotificationActionCategory::Ok, StringView{ U"Ok" } },
			std::pair{ NotificationActionCategory::OkCancel, StringView{ U"OkCancel" } },
			std::pair{ NotificationActionCategory::YesNo, StringView{ U"YesNo" } },
			std::pair{ NotificationActionCategory::YesNoCancel, StringView{ U"YesNoCancel" } },
			std::pair{ NotificationActionCategory::AcceptDecline, StringView{ U"AcceptDecline" } },
			std::pair{ NotificationActionCategory::AcceptDeclineLater, StringView{ U"AcceptDeclineLater" } },
			std::pair{ NotificationActionCategory::RetryCancel, StringView{ U"RetryCancel" } },
			std::pair{ NotificationActionCategory::ViewDismiss, StringView{ U"ViewDismiss" } },
		};

		CheckEnumFormatter(Cases);
	}

	SUBCASE("NotificationResponseType")
	{
		static constexpr std::array Cases =
		{
			std::pair{ NotificationResponseType::Unspecified, StringView{ U"Unspecified" } },
			std::pair{ NotificationResponseType::DefaultActivated, StringView{ U"DefaultActivated" } },
			std::pair{ NotificationResponseType::ActionActivated, StringView{ U"ActionActivated" } },
			std::pair{ NotificationResponseType::Dismissed, StringView{ U"Dismissed" } },
			std::pair{ NotificationResponseType::Failed, StringView{ U"Failed" } },
		};

		CheckEnumFormatter(Cases);
	}

	SUBCASE("AudioFormat")
	{
		static constexpr std::array Cases =
		{
			std::pair{ AudioFormat::Unknown, StringView{ U"Unknown" } },
			std::pair{ AudioFormat::WAV, StringView{ U"WAV" } },
			std::pair{ AudioFormat::MP3, StringView{ U"MP3" } },
			std::pair{ AudioFormat::AAC, StringView{ U"AAC" } },
			std::pair{ AudioFormat::OggVorbis, StringView{ U"OggVorbis" } },
			std::pair{ AudioFormat::Opus, StringView{ U"Opus" } },
			std::pair{ AudioFormat::WMA, StringView{ U"WMA" } },
			std::pair{ AudioFormat::FLAC, StringView{ U"FLAC" } },
			std::pair{ AudioFormat::MIDI, StringView{ U"MIDI" } },
		};

		CheckEnumFormatter(Cases);
	}

	SUBCASE("ShaderStage")
	{
		static constexpr std::array Cases =
		{
			std::pair{ ShaderStage::Vertex, StringView{ U"Vertex" } },
			std::pair{ ShaderStage::Pixel, StringView{ U"Pixel" } },
		};

		CheckEnumFormatter(Cases);
	}

	SUBCASE("LogType")
	{
		static constexpr std::array Cases =
		{
			std::pair{ LogType::Error, StringView{ U"Error" } },
			std::pair{ LogType::Fail, StringView{ U"Fail" } },
			std::pair{ LogType::Warning, StringView{ U"Warning" } },
			std::pair{ LogType::App, StringView{ U"App" } },
			std::pair{ LogType::Info, StringView{ U"Info" } },
			std::pair{ LogType::Debug, StringView{ U"Debug" } },
			std::pair{ LogType::Trace, StringView{ U"Trace" } },
		};

		CheckEnumFormatter(Cases);
	}

	SUBCASE("LetterCase")
	{
		static constexpr std::array Cases =
		{
			std::pair{ LetterCase::Lower, StringView{ U"Lower" } },
			std::pair{ LetterCase::Upper, StringView{ U"Upper" } },
		};

		CheckEnumFormatter(Cases);
	}

}
