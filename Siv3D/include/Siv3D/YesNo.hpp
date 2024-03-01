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
	/// @brief Yes/No パラメータのためのユーティリティクラス | Utility class for Yes/No parameters
	/// @tparam Tag タグ名 | Tag name
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

		/// @brief Yes を示す定数 | Constant that indicates Yes
		static constexpr Helper Yes{ true };

		/// @brief No を示す定数 | Constant that indicates No
		static constexpr Helper No{ false };
	};
}

# include "detail/YesNo.ipp"
