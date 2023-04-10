//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2023 Ryo Suzuki
//	Copyright (c) 2016-2023 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once

namespace s3d
{
	template <class Tag>
	constexpr YesNo<Tag>::YesNo(const bool yesNo) noexcept
		: m_yesNo{ yesNo } {}

	template <class Tag>
	constexpr YesNo<Tag>::YesNo(const Helper helper) noexcept
		: m_yesNo{ helper.yesNo } {}

	template <class Tag>
	constexpr YesNo<Tag>::operator bool() const noexcept
	{
		return m_yesNo;
	}

	template <class Tag>
	constexpr bool YesNo<Tag>::getBool() const noexcept
	{
		return m_yesNo;
	}
}
