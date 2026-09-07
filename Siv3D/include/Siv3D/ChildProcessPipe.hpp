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
# include "Types.hpp"

namespace s3d
{
	struct FormatData;

	////////////////////////////////////////////////////////////////
	//
	//	ChildProcessPipe
	//
	////////////////////////////////////////////////////////////////

	/// @brief 子プロセスと接続する標準入出力 | Standard streams connected to a child process
	enum class ChildProcessPipe : uint8
	{
		/// @brief 標準入出力を接続しない | Do not connect standard streams
		None,

		/// @brief `ChildProcess::istream()` で子プロセスの標準出力を読み取る | Read child process standard output through `ChildProcess::istream()`
		StdIn,

		/// @brief `ChildProcess::ostream()` で子プロセスの標準入力へ書き込む | Write to child process standard input through `ChildProcess::ostream()`
		StdOut,

		/// @brief 子プロセスの標準入力へ書き込み、標準出力を読み取る | Write to standard input and read from standard output
		StdInOut,
	};

	/// @brief 子プロセスのパイプ設定を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value 子プロセスのパイプ設定
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, ChildProcessPipe value);
}
