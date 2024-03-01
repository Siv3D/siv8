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

	template <class Tag>
	constexpr YesNo<Tag>::YesNo(const bool yesNo) noexcept
		: m_yesNo{ yesNo } {}

	template <class Tag>
	constexpr YesNo<Tag>::YesNo(const Helper helper) noexcept
		: m_yesNo{ helper.yesNo } {}
		
	////////////////////////////////////////////////////////////////
	//
	//	operator bool
	//
	////////////////////////////////////////////////////////////////

	template <class Tag>
	constexpr YesNo<Tag>::operator bool() const noexcept
	{
		return m_yesNo;
	}
		
	////////////////////////////////////////////////////////////////
	//
	//	getBool
	//
	////////////////////////////////////////////////////////////////

	template <class Tag>
	constexpr bool YesNo<Tag>::getBool() const noexcept
	{
		return m_yesNo;
	}
}
