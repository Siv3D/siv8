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

# include <Siv3D/OSCTypeTag.hpp>
# include <Siv3D/StringView.hpp>
# include <Siv3D/FormatData.hpp>

namespace s3d
{
	namespace
	{
		[[nodiscard]]
		constexpr StringView ToString(const OSCTypeTag value) noexcept
		{
			switch (value)
			{
			case OSCTypeTag::False:			return U"False"_sv;
			case OSCTypeTag::True:			return U"True"_sv;
			case OSCTypeTag::Char:			return U"Char"_sv;
			case OSCTypeTag::Int32:			return U"Int32"_sv;
			case OSCTypeTag::Int64:			return U"Int64"_sv;
			case OSCTypeTag::Float:			return U"Float"_sv;
			case OSCTypeTag::Double:		return U"Double"_sv;
			case OSCTypeTag::MIDIMessage:	return U"MIDIMessage"_sv;
			case OSCTypeTag::TimeTag:		return U"TimeTag"_sv;
			case OSCTypeTag::RGBAColor:		return U"RGBAColor"_sv;
			case OSCTypeTag::String:		return U"String"_sv;
			case OSCTypeTag::Symbol:		return U"Symbol"_sv;
			case OSCTypeTag::Blob:			return U"Blob"_sv;
			case OSCTypeTag::Nil:			return U"Nil"_sv;
			case OSCTypeTag::Infinitum:		return U"Infinitum"_sv;
			case OSCTypeTag::ArrayBegin:	return U"ArrayBegin"_sv;
			case OSCTypeTag::ArrayEnd:		return U"ArrayEnd"_sv;
			default:
				break;
			}
			return U"(Invalid)"_sv;
		}
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const OSCTypeTag value)
	{
		formatData.string.append(ToString(value));
	}
}
