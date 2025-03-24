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

# include <Siv3D/Base64Value.hpp>
# include <Siv3D/Blob.hpp>
# include <Siv3D/MemoryMappedFileView.hpp>
# include <ThirdParty/simdutf/simdutf.h>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	encodeFromMemory
	//
	////////////////////////////////////////////////////////////////

	void Base64Value::encodeFromMemory(const void* src, const size_t size)
	{
		if ((not src) || (size == 0))
		{
			m_base64.clear();
			return;
		}

		const size_t base64Length = simdutf::base64_length_from_binary(size);

		m_base64.resize(base64Length);
		
		simdutf::binary_to_base64(static_cast<const char*>(src), size, m_base64.data());
	}

	void Base64Value::encodeFromMemory(const std::span<const Byte> src)
	{
		encodeFromMemory(src.data(), src.size_bytes());
	}

	////////////////////////////////////////////////////////////////
	//
	//	encodeFromBlob
	//
	////////////////////////////////////////////////////////////////

	void Base64Value::encodeFromBlob(const Blob& blob)
	{
		encodeFromMemory(blob.data(), blob.size_bytes());
	}

	////////////////////////////////////////////////////////////////
	//
	//	encodeFromFile
	//
	////////////////////////////////////////////////////////////////

	void Base64Value::encodeFromFile(const FilePathView path)
	{
		MemoryMappedFileView file{ path };

		if (not file)
		{
			m_base64.clear();
			return;
		}

		if (auto mapped = file.mapAll())
		{
			encodeFromMemory(mapped.data, mapped.size);
		}
		else
		{
			m_base64.clear();
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	encodeFromUTF8
	//
	////////////////////////////////////////////////////////////////

	void Base64Value::encodeFromUTF8(const std::string_view s)
	{
		encodeFromMemory(s.data(), s.size());
	}

	////////////////////////////////////////////////////////////////
	//
	//	encodeFromString
	//
	////////////////////////////////////////////////////////////////

	void Base64Value::encodeFromString(const StringView s)
	{
		encodeFromUTF8(Unicode::ToUTF8(s));
	}

	////////////////////////////////////////////////////////////////
	//
	//	decodeToBinary
	//
	////////////////////////////////////////////////////////////////

	Result<size_t, size_t> Base64Value::decodeToMemory(void* dst) const
	{
		if ((not m_base64.empty()) && (not dst))
		{
			return Err{ 0 };
		}

		const auto result = simdutf::base64_to_binary(m_base64.data(), m_base64.size(), static_cast<char*>(dst));

		if (result.error != simdutf::SUCCESS)
		{
			return Err{ result.count };
		}

		return result.count;
	}

	////////////////////////////////////////////////////////////////
	//
	//	decodeToBlob
	//
	////////////////////////////////////////////////////////////////

	Result<size_t, size_t> Base64Value::decodeToBlob(Blob& dst) const
	{
		if (m_base64.empty())
		{
			dst.clear();
			return{};
		}
		
		const size_t maxBinarySize = getMaxBinarySize();

		dst.resize(maxBinarySize);

		const auto result = simdutf::base64_to_binary(m_base64.data(), m_base64.size(), static_cast<char*>(static_cast<void*>((dst.data()))));

		if (result.error != simdutf::SUCCESS)
		{
			dst.clear();
			return Err{ result.count };
		}

		dst.resize(result.count);

		return result.count;
	}

	Blob Base64Value::decodeToBlob() const
	{
		Blob blob;

		if (decodeToBlob(blob))
		{
			return blob;
		}
		else
		{
			return{};
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	decodeToFile
	//
	////////////////////////////////////////////////////////////////

	Result<size_t, size_t> Base64Value::decodeToFile(const FilePathView path) const
	{
		Blob blob;

		if (const auto result = decodeToBlob(blob))
		{
			if (blob.save(path))
			{
				return result;
			}
			else
			{
				return Err{ 0 };
			}
		}
		else
		{
			return Err{ result.error() };
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	decodeToUTF8
	//
	////////////////////////////////////////////////////////////////

	Result<size_t, size_t> Base64Value::decodeToUTF8(std::string& dst) const
	{
		if (m_base64.empty())
		{
			dst.clear();
			return{};
		}

		const size_t maxBinarySize = getMaxBinarySize();

		dst.resize(maxBinarySize);

		const auto result = simdutf::base64_to_binary(m_base64.data(), m_base64.size(), dst.data());

		if (result.error != simdutf::SUCCESS)
		{
			dst.clear();
			return Err{ result.count };
		}

		dst.resize(result.count);

		return result.count;
	}

	std::string Base64Value::decodeToUTF8() const
	{
		std::string s;

		if (decodeToUTF8(s))
		{
			return s;
		}
		else
		{
			return{};
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	decodeToString
	//
	////////////////////////////////////////////////////////////////

	Result<size_t, size_t> Base64Value::decodeToString(String& dst) const
	{
		std::string s;

		const auto result = decodeToUTF8(s);

		if (result)
		{
			dst = Unicode::FromUTF8(s);
		}
		else
		{
			dst.clear();
			return Err{ result.error() };
		}

		return dst.size();
	}

	String Base64Value::decodeToString() const
	{
		String s;

		if (decodeToString(s))
		{
			return s;
		}
		else
		{
			return{};
		}
	}

	////////////////////////////////////////////////////////////////
	//
	//	EncodeFromMemory
	//
	////////////////////////////////////////////////////////////////

	Base64Value Base64Value::EncodeFromMemory(const void* src, const size_t size)
	{
		Base64Value base64Value;

		base64Value.encodeFromMemory(src, size);

		return base64Value;
	}

	Base64Value Base64Value::EncodeFromMemory(const std::span<const Byte> src)
	{
		return EncodeFromMemory(src.data(), src.size_bytes());
	}

	////////////////////////////////////////////////////////////////
	//
	//	EncodeFromBlob
	//
	////////////////////////////////////////////////////////////////

	Base64Value Base64Value::EncodeFromBlob(const Blob& blob)
	{
		return EncodeFromMemory(blob.data(), blob.size_bytes());
	}

	////////////////////////////////////////////////////////////////
	//
	//	FromFile
	//
	////////////////////////////////////////////////////////////////

	Base64Value Base64Value::EncodeFromFile(const FilePathView path)
	{
		Base64Value base64Value;

		base64Value.encodeFromFile(path);

		return base64Value;
	}

	////////////////////////////////////////////////////////////////
	//
	//	EncodeFromUTF8
	//
	////////////////////////////////////////////////////////////////

	Base64Value Base64Value::EncodeFromUTF8(const std::string_view s)
	{
		Base64Value base64Value;

		base64Value.encodeFromUTF8(s);

		return base64Value;
	}

	////////////////////////////////////////////////////////////////
	//
	//	EncodeFromString
	//
	////////////////////////////////////////////////////////////////

	Base64Value Base64Value::EncodeFromString(const StringView s)
	{
		Base64Value base64Value;

		base64Value.encodeFromString(s);

		return base64Value;
	}
}
