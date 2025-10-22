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

		[[nodiscard]]
		Anchor() = default;

		[[nodiscard]]
		constexpr Anchor(Name name) noexcept;

		[[nodiscard]]
		constexpr Anchor(StringView name) noexcept;

		[[nodiscard]]
		constexpr bool isTop() const noexcept;

		[[nodiscard]]
		constexpr bool isMiddle() const noexcept;

		[[nodiscard]]
		constexpr bool isBottom() const noexcept;

		[[nodiscard]]
		constexpr bool isLeft() const noexcept;

		[[nodiscard]]
		constexpr bool isCenter() const noexcept;

		[[nodiscard]]
		constexpr bool isRight() const noexcept;

		/// @brief 左上
		static constexpr Name TopLeft{ VTop | HLeft };

		/// @brief 上中央
		static constexpr Name TopCenter{ VTop | HCenter };

		/// @brief 右上
		static constexpr Name TopRight{ VTop | HRight };

		/// @brief 左中央
		static constexpr Name MiddleLeft{ VMiddle | HLeft };

		/// @brief 中央
		static constexpr Name Center{ VMiddle | HCenter };

		/// @brief 右中央
		static constexpr Name MiddleRight{ VMiddle | HRight };

		/// @brief 左下
		static constexpr Name BottomLeft{ VBottom | HLeft };

		/// @brief 下中央
		static constexpr Name BottomCenter{ VBottom | HCenter };

		/// @brief 右下
		static constexpr Name BottomRight{ VBottom | HRight };

		friend constexpr bool operator ==(const Anchor&, const Anchor&) = default;

		friend void Formatter(FormatData& formatData, Anchor value);

	private:

		uint8 m_value = Center.value;
	};
}

# include "detail/Anchor.ipp"
