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
# include "StringView.hpp"
# include "Format.hpp"

namespace s3d
{
	namespace Window
	{
		////////////////////////////////////////////////////////////////
		//
		//	DefaultTitle
		//
		////////////////////////////////////////////////////////////////

		/// @brief ウィンドウのデフォルトのタイトル
		inline constexpr StringView DefaultTitle = U"Siv3D App";

		////////////////////////////////////////////////////////////////
		//
		//	SetTitle
		//
		////////////////////////////////////////////////////////////////

		/// @brief ウィンドウのタイトルを変更します。
		/// @param title 新しいタイトル
		void SetTitle(const String& title);

		/// @brief ウィンドウのタイトルを変更します。
		/// @tparam ...Args 新しいタイトルの値の型
		/// @param ...args 新しいタイトル
		void SetTitle(const Concept::Formattable auto&... args);

		////////////////////////////////////////////////////////////////
		//
		//	GetTitle
		//
		////////////////////////////////////////////////////////////////

		/// @brief 現在のウィンドウのタイトルを返します。
		/// @return  現在のウィンドウのタイトル
		[[nodiscard]]
		const String& GetTitle() noexcept;
	}
}

# include "detail/Window.ipp"
