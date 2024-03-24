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
# include <ThirdParty/simdutf/simdutf.h>

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	decodeToBinary
	//
	////////////////////////////////////////////////////////////////

	Result<void, size_t> Base64Value::decodeToMemory(void* dst) const
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

		return{};
	}

	////////////////////////////////////////////////////////////////
	//
	//	decodeToBlob
	//
	////////////////////////////////////////////////////////////////

	Result<void, size_t> Base64Value::decodeToBlob(Blob& dst) const
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

		return{};
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

	//bool decodeToFile(FilePathView path) const;

	////////////////////////////////////////////////////////////////
	//
	//	EncodeFromMemoery
	//
	////////////////////////////////////////////////////////////////

	Base64Value Base64Value::EncodeFromMemoery(const void* src, const size_t size)
	{
		const size_t base64Size = simdutf::base64_length_from_binary(size);

		Base64Value base64Value;
		base64Value.m_base64.resize(base64Size);
		simdutf::binary_to_base64(static_cast<const char*>(src), size, base64Value.m_base64.data());

		return base64Value;
	}

	////////////////////////////////////////////////////////////////
	//
	//	EncodeFromBlob
	//
	////////////////////////////////////////////////////////////////

	//Base64Value EncodeFromBlob(const Blob& blob);

	////////////////////////////////////////////////////////////////
	//
	//	FromFile
	//
	////////////////////////////////////////////////////////////////

	//Base64Value EncodeFromFile(FilePathView path);

}
