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
	//	YesNo
	//
	////////////////////////////////////////////////////////////////

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

			////////////////////////////////////////////////////////////////
			//
			//	operator ==
			//
			////////////////////////////////////////////////////////////////

			/// @brief 2 つの YesNo 値が等しいかを返します。
			/// @param lhs 一方の YesNo 値
			/// @param rhs もう一方の YesNo 値
			/// @return 2 つの YesNo 値が等しい場合 true, それ以外の場合は false
			[[nodiscard]]
			friend constexpr bool operator ==(const Helper lhs, const Helper rhs) noexcept
			{
				return (lhs.yesNo == rhs.yesNo);
			}

			/// @brief 2 つの YesNo 値が等しいかを返します。
			/// @param lhs 一方の YesNo 値
			/// @param rhs もう一方の YesNo 値
			/// @return 2 つの YesNo 値が等しい場合 true, それ以外の場合は false
			[[nodiscard]]
			friend constexpr bool operator ==(const YesNo lhs, const Helper rhs) noexcept
			{
				return (lhs.getBool() == rhs.yesNo);
			}
		};

	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief YesNo 値を作成します。
		/// @param yesNo YesNo 値
		[[nodiscard]]
		explicit constexpr YesNo(bool yesNo) noexcept;

		/// @brief YesNo 値を作成します。
		/// @param helper YesNo 値
		[[nodiscard]]
		constexpr YesNo(Helper helper) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator bool
		//
		////////////////////////////////////////////////////////////////

		/// @brief Yes であるかを返します。
		/// @return Yes である場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr explicit operator bool() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	getBool
		//
		////////////////////////////////////////////////////////////////

		/// @brief Yes であるかを返します。
		/// @return Yes である場合 true, それ以外の場合は false
		[[nodiscard]]
		constexpr bool getBool() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	Yes, No
		//
		////////////////////////////////////////////////////////////////

		/// @brief Yes を示す定数 | Constant that indicates Yes
		static constexpr Helper Yes{ true };

		/// @brief No を示す定数 | Constant that indicates No
		static constexpr Helper No{ false };
	};
}

# include "detail/YesNo.ipp"
