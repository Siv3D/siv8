//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2022 Ryo Suzuki
//	Copyright (c) 2016-2022 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once

namespace s3d
{
	/// @brief Yes/No パラメータのためのユーティリティクラス
	/// @tparam Tag タグ名
	template <class Tag>
	struct YesNo
	{
	private:

		bool m_yesNo;

		struct Helper
		{
			bool yesNo;
		};

	public:

		[[nodiscard]]
		explicit constexpr YesNo(bool yesNo) noexcept;

		[[nodiscard]]
		constexpr YesNo(Helper helper) noexcept;

		[[nodiscard]]
		explicit constexpr operator bool() const noexcept;

		[[nodiscard]]
		constexpr bool getBool() const noexcept;

		/// @brief Yes を示す定数
		static constexpr Helper Yes{ true };

		/// @brief No を示す定数
		static constexpr Helper No{ false };
	};
}

# include "detail/YesNo.ipp"
