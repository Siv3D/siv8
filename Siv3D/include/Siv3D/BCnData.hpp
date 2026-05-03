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
# include "Blob.hpp"
# include "TextureFormat.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	BCnData
	//
	////////////////////////////////////////////////////////////////

	struct BCnData
	{
		/// @brief 画像のサイズ
		Size size{ 0, 0 };

		/// @brief テクスチャフォーマット（例: BC1_RGBA_Unorm）
		TextureFormat format = TextureFormat::Unknown;

		/// @brief 圧縮されたテクスチャデータ（mip level ごと）
		Array<Blob> textures;
		
		////////////////////////////////////////////////////////////////
		//
		//	isEmpty
		//
		////////////////////////////////////////////////////////////////

		/// @brief データが空であるかを返します。
		/// @return データが空である場合 true, それ以外の場合は false
		[[nodiscard]]
		bool isEmpty() const noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	operator bool
		//
		////////////////////////////////////////////////////////////////

		/// @brief データが空でないかを返します。
		/// @return データが空でない場合 true, それ以外の場合は false
		[[nodiscard]]
		explicit operator bool() const noexcept;
	};
}

# include "detail/BCnData.ipp"
