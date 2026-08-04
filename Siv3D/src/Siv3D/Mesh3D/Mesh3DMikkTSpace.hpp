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
# include <Siv3D/Array.hpp>
# include <Siv3D/TriangleIndex32.hpp>
# include <Siv3D/Vertex3D.hpp>

namespace s3d
{
	/// @brief MikkTSpace 互換の頂点接線を生成します。
	/// @param vertices 頂点配列
	/// @param indices 三角形インデックス配列
	/// @return 接線の生成に成功した場合 true, それ以外の場合は false
	/// @details
	/// MikkTSpace が面コーナーごとに異なる接線を生成した場合、必要に応じて
	/// 頂点を複製し、インデックス配列を再構築します。
	///
	/// 成功時、各頂点の `tangent.xyz` には単位接線ベクトルが、
	/// `tangent.w` には従接線を復元するための符号（`+1` または `-1`）が
	/// 格納されます。従接線は次の式で復元できます。
	/// @code
	/// bitangent = Cross(normal, tangent.xyz()) * tangent.w;
	/// @endcode
	///
	/// 失敗した場合、@p vertices と @p indices は変更されません。
	/// 頂点法線と UV 座標は事前に設定されている必要があります。
	[[nodiscard]]
	bool GenerateMikkTSpaceTangents(Array<Vertex3D>& vertices, Array<TriangleIndex32>& indices);
}
