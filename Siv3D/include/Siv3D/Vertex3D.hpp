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
# include "PointVector.hpp"

namespace s3d
{
	////////////////////////////////////////////////////////////////
	//
	//	Vertex3D
	//
	////////////////////////////////////////////////////////////////

	/// @brief 3D 形状を構成する基本頂点データ
	struct alignas(16) Vertex3D
	{
		/// @brief 3D 描画のインデックス配列に使用する型
		using IndexType = uint32;

		/// @brief 頂点の位置
		Float3 pos;

		/// @brief 頂点の法線ベクトル
		Float3 normal;

		/// @brief 頂点の UV 座標
		Float2 tex;

		/// @brief 頂点の接線ベクトルと従接線の向きを表す符号
		/// @remark `xyz` 成分には接線ベクトルを格納します。
		/// @remark `w` 成分には従接線を復元するための符号（`+1` または `-1`）を格納します。
		Float4 tangent;

		////////////////////////////////////////////////////////////////
		//
		//	bitangent
		//
		////////////////////////////////////////////////////////////////

		/// @brief 従接線ベクトルを返します。
		/// @return 法線、接線、および接空間の向きから復元した従接線ベクトル
		[[nodiscard]]
		constexpr Float3 bitangent() const noexcept;
	};
}

# include "detail/Vertex3D.ipp"
