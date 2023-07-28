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
# include <memory>
# include "Common.hpp"
# include "TextEncoding.hpp"
# include "Optional.hpp"

namespace s3d
{
	/// @brief 読み込み用テキストファイル
	class TextReader
	{
	public:

		/// @brief デフォルトコンストラクタ
		[[nodiscard]]
		TextReader();

		/// @brief テキストファイルをオープンします。
		/// @param path ファイルパス
		/// @param encoding テキストのエンコーディング形式
		[[nodiscard]]
		explicit TextReader(FilePathView path, const Optional<TextEncoding>& encoding = unspecified) {}

		template <class Reader>
			requires std::derived_from<Reader, IReader>
		[[nodiscard]]
		explicit TextReader(Reader&& reader, const Optional<TextEncoding>& encoding = unspecified) { reader, encoding; }

	private:

		class TextReaderDetail;

		std::shared_ptr<TextReaderDetail> pImpl;
	};
}