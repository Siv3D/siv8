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
# include "Types.hpp"
# include "String.hpp"

namespace s3d
{
	struct FormatData;

	////////////////////////////////////////////////////////////////
	//
	//	WebBrowser
	//
	////////////////////////////////////////////////////////////////

	/// @brief Web ブラウザ | Web browser
	enum class WebBrowser : uint8
	{
		/// @brief Google Chrome
		Chrome,

		/// @brief Mozilla Firefox
		Firefox,

		/// @brief Microsoft Edge
		Edge,

		/// @brief Safari
		Safari,
		
		/// @brief Opera
		Opera,

		/// @brief Brave
		Brave,

		/// @brief Vivaldi
		Vivaldi,
	};

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	/// @brief WebBrowser をフォーマットします。
	/// @param formatData フォーマットデータ
	/// @param value WebBrowser
	void Formatter(FormatData& formatData, WebBrowser value);

	namespace System
	{
		////////////////////////////////////////////////////////////////
		//
		//	GetWebBrowserPath
		//
		////////////////////////////////////////////////////////////////

		/// @brief Web ブラウザのパスを取得します。
		/// @param webBrowser Web ブラウザ | Web browser
		/// @return Web ブラウザのパス。失敗した場合は空の文字列 | The path to the web browser. An empty string if it fails.
		[[nodiscard]]
		FilePath GetWebBrowserPath(WebBrowser webBrowser);
	}
}
