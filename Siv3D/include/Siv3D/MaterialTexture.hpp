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
# include "String.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	MaterialTexture
	//
	////////////////////////////////////////////////////////////////

	/// @brief マテリアルが参照するテクスチャ
	struct MaterialTexture
	{
		/// @brief テクスチャファイルのパス
		FilePath path;

		/// @brief 使用する UV セットのインデックス
		/// @remark 現在の `Mesh3D` は UV セット 0 のみを持ちます。
		uint32 uvIndex = 0;
	};
}
