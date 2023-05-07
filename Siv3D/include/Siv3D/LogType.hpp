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
# include "Common.hpp"

namespace s3d
{
	enum class LogType
	{
		/// @brief アプリケーションの継続が不可能なエラーに関するログです。
		/// @remark デフォルトでは常に出力されます。
		Error,

		/// @brief 処理の失敗に関するログです。
		/// @remark デフォルトでは常に出力されます。
		Fail,

		/// @brief 処理の警告に関するログです。
		/// @remark デフォルトでは常に出力されます。
		Warning,

		/// @brief ユーザによるログです。
		/// @remark デフォルトでは常に出力されます。
		User,

		/// @brief エンジンの処理の一般的な情報に関するログです。
		/// @remark デフォルトでは常に出力されます。
		Info,

		/// @brief エンジンの処理の詳細な情報に関するログです。
		/// @remark デフォルトでは Debug ビルド時に出力されます。
		Trace,
	};
}
