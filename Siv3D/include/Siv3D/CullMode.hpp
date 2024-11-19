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

namespace s3d
{
	struct FormatData;

	////////////////////////////////////////////////////////////////
	//
	//	CullMode
	//
	////////////////////////////////////////////////////////////////

	/// @brief カリングモード | Cull mode
	enum class CullMode : uint8
	{
		/// @brief カリングを無効にする | Disable culling
		None	= 1,

		/// @brief 前面をカリングする | Cull front faces
		Front	= 2,

		/// @brief 裏面をカリングする | Cull back faces
		Back	= 3,
	};

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	/// @brief カリングモードを文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value カリングモード
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, CullMode value);
}
