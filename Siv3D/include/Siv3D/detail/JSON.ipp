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
	JSON::JSON(const Concept::SignedIntegral auto value)
		: m_json(value) {}

	JSON::JSON(const Concept::UnsignedIntegral auto value)
		: m_json(value) {}

	JSON::JSON(const Concept::FloatingPoint auto value)
		: m_json(value) {}

	template <class Iterator>
	JSON::JSON(Iterator first, Iterator last)
		: m_json(first, last) {}

	template <JSONCompatibleType Type>
	JSON::JSON(const Array<Type>& arr)
		: m_json(arr.map([](auto&& v) { return nlohmann::json(v); })) {}

	template <JSONCompatibleType Type>
	JSON::JSON(const std::initializer_list<Type>& list)
		: m_json(list) {}
}

