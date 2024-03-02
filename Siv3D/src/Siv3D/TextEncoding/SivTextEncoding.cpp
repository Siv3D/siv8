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

# include <Siv3D/TextEncoding.hpp>
# include <Siv3D/BinaryReader.hpp>
# include <Siv3D/FormatData.hpp>

namespace s3d
{
	namespace detail
	{
		static constexpr std::array TextEncodingStrings =
		{
			U"UTF8_NO_BOM"_sv,
			U"UTF8_WITH_BOM"_sv,
			U"UTF16LE"_sv,
			U"UTF16BE"_sv,
		};
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const TextEncoding value)
	{
		formatData.string.append(detail::TextEncodingStrings[FromEnum(value)]);
	}

	namespace Unicode
	{
		////////////////////////////////////////////////////////////////
		//
		//	GetTextEncoding
		//
		////////////////////////////////////////////////////////////////

		TextEncoding GetTextEncoding(const IReader& reader)
		{
			if (not reader.isOpen())
			{
				return TextEncoding::UTF8_NO_BOM;
			}

			if (reader.size() == 0)
			{
				return TextEncoding::UTF8_NO_BOM;
			}

			uint8 bom[3]{};
			reader.lookahead(bom);

			if ((bom[0] == 0xEF) && (bom[1] == 0xBB) && (bom[2] == 0xBF))
			{
				return TextEncoding::UTF8_WITH_BOM;
			}
			else if ((bom[0] == 0xFF) && (bom[1] == 0xFE))
			{
				return TextEncoding::UTF16LE;
			}
			else if ((bom[0] == 0xFE) && (bom[1] == 0xFF))
			{
				return TextEncoding::UTF16BE;
			}

			return TextEncoding::UTF8_NO_BOM;
		}

		TextEncoding GetTextEncoding(const FilePathView path)
		{
			return GetTextEncoding(BinaryReader{ path });
		}
	}
}
