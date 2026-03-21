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
# include "Optional.hpp"
# include "Array.hpp"
# include "DragStatus.hpp"
# include "DroppedFilePath.hpp"
# include "DroppedText.hpp"

namespace s3d
{
	namespace DragDrop
	{
		/// @brief ドラッグ＆ドロップでファイルパスを受け付けるかを設定します。
		/// @param accept ファイルパスを受け付ける場合は true
		/// @remark デフォルトでは true を想定します。
		/// @remark この設定は受け側の挙動にのみ影響します。
		void AcceptFilePaths(bool accept);

		/// @brief ドラッグ＆ドロップでテキストを受け付けるかを設定します。
		/// @param accept テキストを受け付ける場合は true
		/// @remark デフォルトでは false を想定します。
		/// @remark この設定は受け側の挙動にのみ影響します。
		void AcceptText(bool accept);

		/// @brief ウィンドウ上で hover 中の OS ドラッグ状態を返します。
		/// @return ドラッグ中の情報。該当するドラッグが存在しない場合は none
		/// @remark ここで返るのは、現在の受け入れ設定で扱えるファイルパスまたはテキストのみです。
		/// @remark ドロップ完了後のアイテムはこの戻り値には残らず、内部キューへ蓄積されます。
		[[nodiscard]]
		Optional<DragStatus> DragOver();

		/// @brief 未回収のドロップ済みファイルパスが存在する場合に true を返します。
		/// @return 未回収のファイルパスが 1 件以上ある場合 true
		[[nodiscard]]
		bool HasNewFilePaths();

		/// @brief 未回収のドロップ済みテキストが存在する場合に true を返します。
		/// @return 未回収のテキストが 1 件以上ある場合 true
		[[nodiscard]]
		bool HasNewText();

		/// @brief 蓄積されているドロップ済みオブジェクトの情報を消去します。
		/// @remark ファイルパス、テキストの両方の内部キューを空にします。
		void Clear();

		/// @brief 蓄積されたドロップ済みファイルパスをすべて回収し、内部キューから削除します。
		/// @return ドロップされたファイルパスの情報一覧
		/// @remark 同一の drop 操作で渡された複数ファイルは、同じ座標と時刻を持つ複数要素として返ります。
		[[nodiscard]]
		Array<DroppedFilePath> ExtractDroppedFilePaths();

		/// @brief 蓄積されたドロップ済みテキストをすべて回収し、内部キューから削除します。
		/// @return ドロップされたテキストの情報一覧
		[[nodiscard]]
		Array<DroppedText> ExtractDroppedTexts();

		/// @brief 1 つの既存ファイルを、外部アプリケーション向けの OS ドラッグとして開始します。
		/// @param path 既存ファイルパス
		/// @return OS ドラッグの開始に成功した場合 true
		/// @remark 実際のポインタ drag gesture 中に呼び出すことを想定します。
		/// @remark 任意タイミングでの開始は、プラットフォームによって失敗する場合があります。
		/// @remark この関数は drag の開始可否のみを返します。完了結果は追跡しません。
		[[nodiscard]]
		bool BeginDragFile(FilePathView path);

		/// @brief 複数の既存ファイルを、外部アプリケーション向けの OS ドラッグとして開始します。
		/// @param paths 既存ファイルパスの配列
		/// @return OS ドラッグの開始に成功した場合 true
		/// @remark 実際のポインタ drag gesture 中に呼び出すことを想定します。
		/// @remark 任意タイミングでの開始は、プラットフォームによって失敗する場合があります。
		/// @remark この関数は drag の開始可否のみを返します。完了結果は追跡しません。
		[[nodiscard]]
		bool BeginDragFiles(const Array<FilePath>& paths);

		/// @brief テキストを、外部アプリケーション向けの OS ドラッグとして開始します。
		/// @param text 提供する文字列
		/// @return OS ドラッグの開始に成功した場合 true
		/// @remark 実際のポインタ drag gesture 中に呼び出すことを想定します。
		/// @remark 任意タイミングでの開始は、プラットフォームによって失敗する場合があります。
		/// @remark この関数は drag の開始可否のみを返します。完了結果は追跡しません。
		[[nodiscard]]
		bool BeginDragText(StringView text);
	}
}
