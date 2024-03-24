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

# pragma once

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	(constructor)
	//
	////////////////////////////////////////////////////////////////

	constexpr Base64Value::Base64Value(const char8* base64)
		: m_base64{ base64 } {}

	constexpr Base64Value::Base64Value(const std::string_view base64)
		: m_base64{ base64 } {}

	constexpr Base64Value::Base64Value(const std::string& base64)
		: m_base64{ base64 } {}

	constexpr Base64Value::Base64Value(std::string&& base64)
		: m_base64{ std::move(base64) } {}

	inline Base64Value::Base64Value(StringView base64)
		: m_base64{ Unicode::ToUTF8(base64) } {}

	////////////////////////////////////////////////////////////////
	//
	//	getBase64
	//
	////////////////////////////////////////////////////////////////

	constexpr const std::string& Base64Value::getBase64() const noexcept
	{
		return m_base64;
	}

	////////////////////////////////////////////////////////////////
	//
	//	isEmpty
	//
	////////////////////////////////////////////////////////////////

	constexpr bool Base64Value::isEmpty() const noexcept
	{
		return m_base64.empty();
	}

	////////////////////////////////////////////////////////////////
	//
	//	operator bool
	//
	////////////////////////////////////////////////////////////////

	constexpr Base64Value::operator bool() const noexcept
	{
		return (not m_base64.empty());
	}

	////////////////////////////////////////////////////////////////
	//
	//	getBinarySize
	//
	////////////////////////////////////////////////////////////////

	constexpr size_t Base64Value::getBinarySize() const noexcept
	{
		size_t padding = 0;

		if (m_base64.ends_with("=="))
		{
			padding = 2;
		}
		else if (m_base64.ends_with('='))
		{
			padding = 1;
		}

		return (((m_base64.size() - padding) * 3) / 4);
	}

}
