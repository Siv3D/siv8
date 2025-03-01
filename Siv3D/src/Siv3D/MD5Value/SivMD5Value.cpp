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

# include <Siv3D/MD5Value.hpp>
# include <Siv3D/Byte.hpp>
# include <Siv3D/Blob.hpp>
# include <Siv3D/BinaryReader.hpp>
# include <ThirdParty/RFC1321/RFC1321.hpp>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	to_string
	//
	////////////////////////////////////////////////////////////////

	std::string MD5Value::to_string() const
	{
		std::string result(32, '\0');
		char* pDst = result.data();

		for (size_t i = 0; i < 16; ++i)
		{
			const std::array<char8, 2> hex = ToHex(Byte{ m_values[i] });
			*pDst++ = hex[0];
			*pDst++ = hex[1];
		}

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	str
	//
	////////////////////////////////////////////////////////////////

	String MD5Value::str() const
	{
		String result(32, U'\0');
		char32* pDst = result.data();

		for (size_t i = 0; i < 16; ++i)
		{
			const std::array<char8, 2> hex = ToHex(Byte{ m_values[i] });
			*pDst++ = hex[0];
			*pDst++ = hex[1];
		}

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	FromMemory
	//
	////////////////////////////////////////////////////////////////

	MD5Value MD5Value::FromMemory(const void* const data, const size_t size) noexcept
	{
		MD5_CTX ctx;
		MD5Value result;

		MD5_Init(&ctx);
		MD5_Update(&ctx, data, static_cast<unsigned long>(size));
		MD5_Final(result.m_values.data(), &ctx);

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	FromBlob
	//
	////////////////////////////////////////////////////////////////

	MD5Value MD5Value::FromBlob(const Blob& blob) noexcept
	{
		return FromMemory(blob.data(), blob.size_bytes());
	}

	////////////////////////////////////////////////////////////////
	//
	//	FromUTF8
	//
	////////////////////////////////////////////////////////////////

	MD5Value MD5Value::FromUTF8(const std::string_view view) noexcept
	{
		return FromMemory(view.data(), view.size());
	}

	////////////////////////////////////////////////////////////////
	//
	//	FromUTF32
	//
	////////////////////////////////////////////////////////////////

	MD5Value MD5Value::FromUTF32(const StringView view) noexcept
	{
		return FromMemory(view.data(), view.size_bytes());
	}

	////////////////////////////////////////////////////////////////
	//
	//	FromFile
	//
	////////////////////////////////////////////////////////////////

	MD5Value MD5Value::FromFile(const FilePathView path)
	{
		BinaryReader reader{ path };

		if (not reader.isOpen())
		{
			return MD5Value::EmptyHash();
		}

		MD5_CTX ctx;
		MD5Value result;

		constexpr size_t BufferSize = 4096;
		std::unique_ptr<uint8[]> buffer = std::make_unique_for_overwrite<uint8[]>(BufferSize);

		MD5_Init(&ctx);

		while (isize readSize = reader.read(buffer.get(), BufferSize))
		{
			MD5_Update(&ctx, buffer.get(), static_cast<uint32>(readSize));
		}

		MD5_Final(result.m_values.data(), &ctx);

		return result;
	}

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	void Formatter(FormatData& formatData, const MD5Value& value)
	{
		formatData.string.append(value.str());
	}
}
