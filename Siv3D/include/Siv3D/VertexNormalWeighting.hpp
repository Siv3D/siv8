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
	struct FormatData;

	////////////////////////////////////////////////////////////////
	//
	//	VertexNormalWeighting
	//
	////////////////////////////////////////////////////////////////

	/// @brief 頂点法線を計算する際の重み付け方式
	enum class VertexNormalWeighting : uint8
	{
		/// @brief 頂点における三角形の内角で重み付けします。
		Angle,

		/// @brief 三角形の面積で重み付けします。
		Area,

		/// @brief 各三角形を等しい重みで扱います。
		Uniform,
	};

	////////////////////////////////////////////////////////////////
	//
	//	Formatter
	//
	////////////////////////////////////////////////////////////////

	/// @brief 頂点法線の重み付け方式を文字列に変換します。
	/// @param formatData 文字列バッファ
	/// @param value 頂点法線の重み付け方式
	/// @remark この関数は Format 用の関数です。通常、ユーザーが直接呼び出す必要はありません。
	void Formatter(FormatData& formatData, VertexNormalWeighting value);
}
