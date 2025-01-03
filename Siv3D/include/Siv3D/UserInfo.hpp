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
# include "Common.hpp"
# include "String.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	UserInfo
	//
	////////////////////////////////////////////////////////////////

	/// @brief ユーザーの情報 | User information
	struct UserInfo
	{
		/// @brief コンピューター名 | Computer name
		String computerName;

		/// @brief ユーザー名 | User name
		String userName;

		/// @brief フルユーザー名 | Full user name
		String fullUserName;

		/// @brief ユーザーのロケール | User locale
		String defaultLocale;

		/// @brief ユーザーの言語 | User language
		String defaultLanguage;

		////////////////////////////////////////////////////////////////
		//
		//	format
		//
		////////////////////////////////////////////////////////////////

		/// @brief ユーザーの情報を文字列に変換します。
		/// @return ユーザーの情報
		[[nodiscard]]
		String format() const;

		////////////////////////////////////////////////////////////////
		//
		//	Formatter
		//
		////////////////////////////////////////////////////////////////
		
		/// @brief ユーザーの情報を文字列に変換します。
		/// @param formatData 文字列バッファ
		/// @param value ユーザーの情報
		/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
		friend void Formatter(FormatData& formatData, const UserInfo& value);
	};

	namespace System
	{
		////////////////////////////////////////////////////////////////
		//
		//	GetUserInfo
		//
		////////////////////////////////////////////////////////////////

		/// @brief ユーザーの情報を返します。 | Returns user information.
		/// @return ユーザーの情報 | User information
		[[nodiscard]]
		const UserInfo& GetUserInfo();

		////////////////////////////////////////////////////////////////
		//
		//	IsRunningInVisualStudio
		//
		////////////////////////////////////////////////////////////////

		/// @brief プログラムが Visual Studio で実行されているかを返します。 | Returns whether the program is running in Visual Studio.
		/// @return プログラムが Visual Studio で実行されている場合 true, それ以外の場合は false | Returns true if the program is running in Visual Studio, false otherwise
		[[nodiscard]]
		bool IsRunningInVisualStudio();

		////////////////////////////////////////////////////////////////
		//
		//	IsRunningInXcode
		//
		////////////////////////////////////////////////////////////////

		/// @brief プログラムが Xcode で実行されているかを返します。 | Returns whether the program is running in Xcode.
		/// @return プログラムが Xcode で実行されている場合 true, それ以外の場合は false | Returns true if the program is running in Xcode, false otherwise
		[[nodiscard]]
		bool IsRunningInXcode();
	}
}
