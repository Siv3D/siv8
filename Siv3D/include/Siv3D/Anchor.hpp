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

# pragma once
# include "StringView.hpp"

namespace s3d
{
	struct FormatData;

	////////////////////////////////////////////////////////////////
	//
	//	Anchor
	//
	////////////////////////////////////////////////////////////////

	/// @brief アンカーの基準位置
	class Anchor
	{
	private:

		static constexpr uint8 VTop = 0b100'000;
		static constexpr uint8 VMiddle = 0b010'000;
		static constexpr uint8 VBottom = 0b001'000;
		static constexpr uint8 HLeft = 0b000'100;
		static constexpr uint8 HCenter = 0b000'010;
		static constexpr uint8 HRight = 0b000'001;

	public:

		struct Name
		{
			uint8 value;
		};

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		Anchor() = default;

		[[nodiscard]]
		constexpr Anchor(Name name) noexcept;

		[[nodiscard]]
		constexpr Anchor(StringView name) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	isTop, isVMiddle, isBottom
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr bool isTop() const noexcept;

		[[nodiscard]]
		constexpr bool isVMiddle() const noexcept;

		[[nodiscard]]
		constexpr bool isBottom() const noexcept;
	
		////////////////////////////////////////////////////////////////
		//
		//	isLeft, isHCenter, isRight
		//
		////////////////////////////////////////////////////////////////

		[[nodiscard]]
		constexpr bool isLeft() const noexcept;

		[[nodiscard]]
		constexpr bool isHCenter() const noexcept;

		[[nodiscard]]
		constexpr bool isRight() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	TopLeft, TopCenter, TopRight
		//
		////////////////////////////////////////////////////////////////

		/// @brief 左上
		static constexpr Name TopLeft{ VTop | HLeft };

		/// @brief 上中央
		static constexpr Name TopCenter{ VTop | HCenter };

		/// @brief 右上
		static constexpr Name TopRight{ VTop | HRight };
	
		////////////////////////////////////////////////////////////////
		//
		//	MiddleLeft, Center, MiddleRight
		//
		////////////////////////////////////////////////////////////////

		/// @brief 左中央
		static constexpr Name MiddleLeft{ VMiddle | HLeft };

		/// @brief 中央
		static constexpr Name Center{ VMiddle | HCenter };

		/// @brief 右中央
		static constexpr Name MiddleRight{ VMiddle | HRight };
	
		////////////////////////////////////////////////////////////////
		//
		//	BottomLeft, BottomCenter, BottomRight
		//
		////////////////////////////////////////////////////////////////

		/// @brief 左下
		static constexpr Name BottomLeft{ VBottom | HLeft };

		/// @brief 下中央
		static constexpr Name BottomCenter{ VBottom | HCenter };

		/// @brief 右下
		static constexpr Name BottomRight{ VBottom | HRight };

		////////////////////////////////////////////////////////////////
		//
		//	operator ==
		//
		////////////////////////////////////////////////////////////////

		friend constexpr bool operator ==(const Anchor&, const Anchor&) = default;
	
		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////

		friend void Formatter(FormatData& formatData, Anchor value);

	private:

		uint8 m_value = Center.value;
	};
}

# include "detail/Anchor.ipp"
