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
# include "Common.hpp"
# include "Array.hpp"
# include "String.hpp"
# include "InputGroups.hpp"

namespace s3d
{
	class Image;
	class DynamicTexture;

	/// @brief スクリーンショットに関連する機能
	/// @remark 設定されたショートカットキーでも、スクリーンショットを自動命名して保存します。
	namespace ScreenCapture
	{
		////////////////////////////////////////////////////////////////
		//
		//	GetScreenshotDirectory
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在設定されている、スクリーンショットの保存先のディレクトリを取得します。
		/// @remark 既定値は OS のピクチャフォルダ内の `Screenshot/` です。
		/// @return 保存先の絶対パス（末尾は `/`）。既定のピクチャフォルダを取得できず、保存先も設定されていない場合は空の文字列
		[[nodiscard]]
		FilePath GetScreenshotDirectory();

		////////////////////////////////////////////////////////////////
		//
		//	SetScreenshotDirectory
		//
		////////////////////////////////////////////////////////////////

		/// @brief スクリーンショットの保存先のディレクトリを変更します。
		/// @param path 新しい保存先のディレクトリ。末尾の区切り文字は省略できます。
		/// @remark 相対パスは呼び出し時に `FileSystem::FullPath()` で解決します。存在しないディレクトリは保存時に作成します。
		/// @remark `SaveCurrentFrame()` とショートカットキーに適用します。予約済みの保存先や `SaveCurrentFrameTo()` には影響しません。
		/// @remark 空・NUL を含むパス、リソースパス、または絶対パスの取得に失敗した場合は、ログに記録して設定を維持します。
		void SetScreenshotDirectory(FilePathView path);

		////////////////////////////////////////////////////////////////
		//
		//	SaveCurrentFrame
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在のフレームを、自動命名した PNG ファイルとして保存予約します。
		/// @remark 呼び出し時の日時から `yyyyMMdd-HHmmss-SSS.png` 形式で命名し、設定された保存ディレクトリに保存します。
		/// @remark 保存のタイミングと失敗時の扱いは、ファイル名を指定するオーバーロードと同じです。
		void SaveCurrentFrame();

		/// @brief 現在のフレームを、設定された保存ディレクトリ内のファイルへ保存予約します。
		/// @param fileName ファイル名（例: `U"boss-clear.png"`）。空、`.`、`..`、および `/`・`\\`・`:`・NUL を含む名前は受け付けません。
		/// @remark 保存先は呼び出し時に確定し、次の `System::Update()` で保存します。同名のファイルは上書きし、画像形式は拡張子で決まります。
		/// @remark 不正な名前や保存先を解決できない要求はログに記録して無視します。画像取得やファイル保存の失敗もログに記録します。
		void SaveCurrentFrame(FilePathView fileName);

		////////////////////////////////////////////////////////////////
		//
		//	SaveCurrentFrameTo
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在のフレームを、指定したパスのファイルへ保存予約します。
		/// @param path 保存するファイルのパス。相対パスと絶対パスを指定できます。
		/// @remark 設定されたスクリーンショット保存ディレクトリは使いません。呼び出し時に `FileSystem::FullPath()` で保存先を確定します。
		/// @remark 次の `System::Update()` で保存します。親ディレクトリは必要に応じて作成し、同名のファイルは上書きします。画像形式は拡張子で決まります。
		/// @remark 空・NUL を含むパス、または保存先を解決できない要求はログに記録して無視します。画像取得やファイル保存の失敗もログに記録します。
		void SaveCurrentFrameTo(FilePathView path);

		////////////////////////////////////////////////////////////////
		//
		//	RequestCurrentFrame
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在のフレームのスクリーンショットを、次の `System::Update()` でメモリ上に保存します。
		/// @remark 保存されたスクリーンショットは、`ScreenCapture::GetFrame()` を通して取得できます。
		void RequestCurrentFrame();

		////////////////////////////////////////////////////////////////
		//
		//	HasNewFrame
		//
		////////////////////////////////////////////////////////////////

		/// @brief メモリ上に新しいスクリーンショットが保存されているかを返します。
		/// @remark 保存要求やショートカットキーによる取得も含みます。ファイル保存の成功を表すものではありません。次の `System::Update()` で更新されます。
		/// @return メモリ上に新しいスクリーンショットが保存されている場合 true, それ以外の場合は false
		[[nodiscard]]
		bool HasNewFrame();

		////////////////////////////////////////////////////////////////
		//
		//	GetFrame
		//
		////////////////////////////////////////////////////////////////

		/// @brief メモリ上に保存されているスクリーンショットを取得します。
		/// @return メモリ上に新しいスクリーンショットが保存されている場合その画像、それ以外の場合は空の画像
		[[nodiscard]]
		const Image& GetFrame();

		/// @brief メモリ上に保存されているスクリーンショットを取得します。
		/// @param image 取得した画像のコピー先
		/// @return メモリ上に新しいスクリーンショットが保存されていて、その取得に成功した場合 true, それ以外の場合は false
		bool GetFrame(Image& image);

		/// @brief メモリ上に保存されているスクリーンショットを DynamicTexture に書き込みます。
		/// @param texture スクリーンショットの書き込み先
		/// @remark DynamicTexture はスクリーンショットと同じ解像度か、空でなければこの関数は失敗します。
		/// @return メモリ上に新しいスクリーンショットが保存されていて、そのスクリーンショットの書き込みに成功した場合 true, それ以外の場合は false
		bool GetFrame(DynamicTexture& texture);

		////////////////////////////////////////////////////////////////
		//
		//	SetShortcutKeys
		//
		////////////////////////////////////////////////////////////////

		/// @brief スクリーンショットのショートカットキーを設定します。
		/// @remark デフォルトでは `{ KeyPrintScreen, KeyF12 }` です。
		/// @param screenshotShortcutKeys スクリーンショットのショートカットキー
		void SetShortcutKeys(const Array<InputGroup>& screenshotShortcutKeys);

		////////////////////////////////////////////////////////////////
		//
		//	GetShortcutKeys
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在設定されているスクリーンショットのショートカットキーを取得します。
		/// @return スクリーンショットのショートカットキー
		[[nodiscard]]
		const Array<InputGroup>& GetShortcutKeys() noexcept;
	}
}
