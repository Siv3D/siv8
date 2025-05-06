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
# include "Common.hpp"
# include "AssetHandle.hpp"
# include "FontMethod.hpp"
# include "FontStyle.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	Font
	//
	////////////////////////////////////////////////////////////////

	/// @brief 頂点シェーダ
	class Font : public AssetHandle<Font>
	{
	public:

		////////////////////////////////////////////////////////////////
		//
		//	(constructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デフォルトコンストラクタ
		[[nodiscard]]
		Font();

		Font(int32 fontSize, FilePathView path, size_t faceIndex, StringView styleName, FontStyle style = FontStyle::Default);

		////////////////////////////////////////////////////////////////
		//
		//	(destructor)
		//
		////////////////////////////////////////////////////////////////

		/// @brief デストラクタ
		virtual ~Font();

		/// @brief 別の Font と内容を交換します。
		/// @param Font 別の Font
		void swap(Font& other) noexcept;

		////////////////////////////////////////////////////////////////
		//
		//	swap
		//
		////////////////////////////////////////////////////////////////

		/// @brief 2 つの Font を入れ替えます。
		/// @param lhs 一方の Font
		/// @param rhs もう一方の Font
		friend void swap(Font& lhs, Font& rhs) noexcept
		{
			lhs.swap(rhs);
		}
	};
}

