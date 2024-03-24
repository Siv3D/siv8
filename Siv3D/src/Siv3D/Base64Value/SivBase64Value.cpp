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
		
		const size_t binarySize = getBinarySize();

		dst.resize(binarySize);

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

	//Result<size_t, size_t> Base64Value::decodeToFile(const FilePathView path) const;

	////////////////////////////////////////////////////////////////
	//
	//	EncodeFromMemoery
	//
	////////////////////////////////////////////////////////////////

	Base64Value Base64Value::EncodeFromMemoery(const void* src, const size_t size)
	{
		Base64Value base64Value;

		base64Value.encodeFromMemory(src, size);

		return base64Value;
	}

	////////////////////////////////////////////////////////////////
	//
	//	EncodeFromBlob
	//
	////////////////////////////////////////////////////////////////

	Base64Value Base64Value::EncodeFromBlob(const Blob& blob)
	{
		return EncodeFromMemoery(blob.data(), blob.size_bytes());
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
}
