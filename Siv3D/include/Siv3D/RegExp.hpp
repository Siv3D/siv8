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
# include <memory>
# include "Common.hpp"
# include "MatchResults.hpp"
# include "FunctionRef.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	RegExp
	//
	////////////////////////////////////////////////////////////////

	/// @brief 正規表現
	class RegExp
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルトコンストラクタ
		[[nodiscard]]
		RegExp();

		/// @brief 正規表現パターンを作成します。
		/// @param pattern 正規表現の文字列
		[[nodiscard]]
		explicit RegExp(StringView pattern);

		////////////////////////////////////////////////////////////////
		//
		//	isValid
		//
		////////////////////////////////////////////////////////////////

		/// @brief 正規表現パターンが有効かを返します。
		/// @return 正規表現パターンが有効な場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isValid() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator bool
		//
		////////////////////////////////////////////////////////////////

		/// @brief 正規表現パターンが有効かを返します。
		/// @return 正規表現パターンが有効な場合 true, それ以外の場合は false
		/// @remark isValid() と同じです。
		[[nodiscard]]
		explicit operator bool() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	fullMatch
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列全体が正規表現パターンにマッチしているかを返します。
		/// @param s 文字列
		/// @return 文字列全体が正規表現パターンにマッチしている場合 true, それ以外の場合は false
		[[nodiscard]]
		bool fullMatch(StringView s) const;

		////////////////////////////////////////////////////////////////
		//
		//	match
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列の先頭が正規表現パターンにマッチしているかを返します。
		/// @param s 文字列
		/// @return マッチオブジェクト
		[[nodiscard]]
		MatchResults match(StringView s) const;

		////////////////////////////////////////////////////////////////
		//
		//	search
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列の一部が正規表現パターンにマッチしているかを返します。
		/// @param s 文字列
		/// @return マッチオブジェクト
		[[nodiscard]]
		MatchResults search(StringView s) const;

		////////////////////////////////////////////////////////////////
		//
		//	findAll
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列内で正規表現パターンにマッチするすべての部分文字列を返します。
		/// @param s 文字列
		/// @return マッチオブジェクトの配列
		[[nodiscard]]
		Array<MatchResults> findAll(StringView s) const;

		////////////////////////////////////////////////////////////////
		//
		//	replaceFirst
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列内で正規表現パターンにマッチする最初の部分文字列を置換します。
		/// @param s 文字列
		/// @param replacement マッチした部分文字列を置換する文字列
		/// @return 置換後の文字列
		[[nodiscard]]
		String replaceFirst(StringView s, StringView replacement) const;

		////////////////////////////////////////////////////////////////
		//
		//	replaceAll
		//
		////////////////////////////////////////////////////////////////

		/// @brief 文字列内で正規表現パターンにマッチするすべての部分文字列を置換します。
		/// @param s 文字列
		/// @param replacement マッチした部分文字列を置換する文字列
		/// @return 置換後の文字列
		[[nodiscard]]
		String replaceAll(StringView s, StringView replacement) const;

		/// @brief 文字列内で正規表現パターンにマッチするすべての部分文字列を置換します。
		/// @param s 文字列
		/// @param replacementFunc マッチした部分文字列を置換する関数
		/// @return 置換後の文字列
		[[nodiscard]]
		String replaceAll(StringView s, FunctionRef<String(const MatchResults&)> replacementFunc) const;

	private:

		class RegExpDetail;

		std::shared_ptr<RegExpDetail> pImpl;
	};

	inline namespace Literals
	{
		inline namespace RegExpLiterals
		{
			////////////////////////////////////////////////////////////////
			//
			//	_re
			//
			////////////////////////////////////////////////////////////////

			/// @brief 正規表現パターンを作成します。
			/// @param s 正規表現の文字列
			/// @param length 正規表現の文字列の長さ
			/// @return 正規表現
			[[nodiscard]]
			RegExp operator ""_re(const char32* s, size_t length) noexcept;
		}
	}
}
