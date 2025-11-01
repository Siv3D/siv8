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

# pragma once

namespace s3d
{
	constexpr Anchor::Anchor(const Name name) noexcept
		: m_value{ name.value } {}

	constexpr Anchor::Anchor(const StringView name) noexcept
	{
		if (name == U"TopLeft")
		{
			m_value = TopLeft.value;
		}
		else if (name == U"TopCenter")
		{
			m_value = TopCenter.value;
		}
		else if (name == U"TopRight")
		{
			m_value = TopRight.value;
		}
		else if (name == U"MiddleLeft")
		{
			m_value = MiddleLeft.value;
		}
		else if (name == U"Center")
		{
			m_value = Center.value;
		}
		else if (name == U"MiddleRight")
		{
			m_value = MiddleRight.value;
		}
		else if (name == U"BottomLeft")
		{
			m_value = BottomLeft.value;
		}
		else if (name == U"BottomCenter")
		{
			m_value = BottomCenter.value;
		}
		else if (name == U"BottomRight")
		{
			m_value = BottomRight.value;
		}
		else
		{
			m_value = Center.value;
		}
	}

	constexpr bool Anchor::isTop() const noexcept
	{
		return ((m_value & VTop) != 0);
	}

	constexpr bool Anchor::isVMiddle() const noexcept
	{
		return ((m_value & VMiddle) != 0);
	}

	constexpr bool Anchor::isBottom() const noexcept
	{
		return ((m_value & VBottom) != 0);
	}

	constexpr bool Anchor::isLeft() const noexcept
	{
		return ((m_value & HLeft) != 0);
	}

	constexpr bool Anchor::isHCenter() const noexcept
	{
		return ((m_value & HCenter) != 0);
	}

	constexpr bool Anchor::isRight() const noexcept
	{
		return ((m_value & HRight) != 0);
	}
}
