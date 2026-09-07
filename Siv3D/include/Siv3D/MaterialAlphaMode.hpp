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
	////////////////////////////////////////////////////////////////
	//
	//	MaterialAlphaMode
	//
	////////////////////////////////////////////////////////////////

	/// @brief マテリアルのアルファ値の扱い
	enum class MaterialAlphaMode : uint8
	{
		/// @brief アルファ値を無視し、不透明として扱います。
		Opaque,

		/// @brief アルファ値としきい値を使って、不透明または完全な透明として扱います。
		Mask,

		/// @brief アルファブレンディングを使用します。
		Blend,
	};
}
