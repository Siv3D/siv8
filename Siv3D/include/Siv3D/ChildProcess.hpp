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
# include "String.hpp"
# include "Array.hpp"
# include "Optional.hpp"
# include "Pipe.hpp"

namespace s3d
{
	/// @brief 子プロセス
	class ChildProcess
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルトコンストラクタ
		[[nodiscard]]
		ChildProcess();

		/// @brief 子プロセスを作成します。
		/// @param path 実行するプログラムのパス
		/// @param pipe パイプ通信の設定
		[[nodiscard]]
		ChildProcess(FilePathView path, Pipe pipe);

		/// @brief 子プロセスを作成します。
		/// @param path 実行するプログラムのパス
		/// @param command コマンド引数
		/// @param pipe パイプ通信の設定
		[[nodiscard]]
		ChildProcess(FilePathView path, StringView command = U"", Pipe pipe = Pipe::None);

		/// @brief 子プロセスを作成します。
		/// @param path 実行するプログラムのパス
		/// @param commands コマンド引数
		/// @param pipe パイプ通信の設定
		[[nodiscard]]
		ChildProcess(FilePathView path, const Array<String>& commands, Pipe pipe = Pipe::None);

		////////////////////////////////////////////////////////////////
		//
		//	(destructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デストラクタ
		~ChildProcess();

		////////////////////////////////////////////////////////////////
		//
		//	operator bool
		//
		////////////////////////////////////////////////////////////////

		/// @brief 有効な子プロセスであるかを返します。
		/// @return 有効な子プロセスである場合 true, それ以外の場合は false
		[[nodiscard]]
		explicit operator bool() const;

		////////////////////////////////////////////////////////////////
		//
		//	isValid
		//
		////////////////////////////////////////////////////////////////

		/// @brief 有効な子プロセスであるかを返します。
		/// @return 有効な子プロセスである場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isValid() const;

		////////////////////////////////////////////////////////////////
		//
		//	isRunning
		//
		////////////////////////////////////////////////////////////////

		/// @brief 子プロセスが実行中であるかを返します。
		/// @return 実行中である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isRunning();

		////////////////////////////////////////////////////////////////
		//
		//	wait
		//
		////////////////////////////////////////////////////////////////

		/// @brief 子プロセスが終了するまで待機します。
		void wait();

		////////////////////////////////////////////////////////////////
		//
		//	terminate
		//
		////////////////////////////////////////////////////////////////

		/// @brief 子プロセスを強制終了します。
		void terminate();

		////////////////////////////////////////////////////////////////
		//
		//	getExitCode
		//
		////////////////////////////////////////////////////////////////

		/// @brief 子プロセスの終了コードを取得します。
		/// @return 子プロセスの終了コード。取得に失敗した場合 none
		[[nodiscard]]
		Optional<int32> getExitCode();

		////////////////////////////////////////////////////////////////
		//
		//	istream
		//
		////////////////////////////////////////////////////////////////

		/// @brief 子プロセスの標準出力を読み取ります。
		/// @return 子プロセスの標準出力を読み取るストリーム
		[[nodiscard]]
		std::istream& istream();

		////////////////////////////////////////////////////////////////
		//
		//	ostream
		//
		////////////////////////////////////////////////////////////////

		/// @brief 子プロセスの標準入力に書き込みます。
		/// @return 子プロセスの標準入力に書き込むストリーム
		[[nodiscard]]
		std::ostream& ostream();

	private:

		class ChildProcessDetail;

		std::shared_ptr<ChildProcessDetail> pImpl;
	};
}
